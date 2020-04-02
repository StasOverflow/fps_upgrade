/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* lwIP includes. */
#include "lwipopts.h"
#include "lwip/api.h"

#include "config.h"

/* Hardware library includes. */
#include <string.h>
#include <lib/types.h>
#include <lib/ustdlib.h>
#include <drivers/uart.h>
#include <drivers/sram.h>
#include <drivers/lcd_driver.h>

#include <interface/interface.h>
#include "comm/inter_comm.h"
#include "sys_error.h"
#include "ext_comm.h"
#include "net_property.h"

#include "protocol_driver.h"
#include "system/app_data.h"

#include "timers.h"

#define mainWEBSERVER_PRIORITY      ( tskIDLE_PRIORITY + 2 )

QueueHandle_t      xGrnLedQueue;


extern void InterfaceTask( void *pvParameters );


void LED_Init( void )
{
    GPIO_InitTypeDef   GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
}

static unsigned long LEDGrnState;

void GrnLedTask( void *pvParameters )
{

    for( ;; )
    {
        GPIO_WriteBit(GPIOE, GPIO_Pin_4, ( LEDGrnState ) ? Bit_SET : Bit_RESET); // Green LED

        LEDGrnState ^= 1;

        IWDG_ReloadCounter();

        vTaskDelay(250);
    }
}


#include "i2c_sw.h"
#include "sys_timer.h"



void IWDGInit( void )
{
    IWDG_Enable();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    /* 2 second period */
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_SetReload(0x138);
}

TaskHandle_t inter_comm_handler = NULL;
TaskHandle_t ext_comm_handler = NULL;
TaskHandle_t interface_handler = NULL;

TimerHandle_t dma_timer;


void vTimerCallback( TimerHandle_t xTimer  )
{
    static uint8_t     trigger;


    if( trigger )
    {
        vTaskSuspend(interface_handler);

        xTimerChangePeriod( dma_timer, 13, 0 );
        DMA2_Stream0->CR   |= DMA_SxCR_EN;  /* Front porch stream Enable */

//        PIN_SET(LED_RED_PIN);
    }
    else
    {
        vTaskResume(interface_handler);
        xTimerChangePeriod( dma_timer, 4, 0 );
    }
    xTimerStart( dma_timer, 0 );

    GPIO_WriteBit(GPIOE, GPIO_Pin_2, (trigger == 0) ? Bit_SET : Bit_RESET);
    trigger ^= 1;

}

int main( void )
{
    IWDGInit();

    LED_Init();

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    sys_timer_init();

    xGrnLedQueue = xQueueCreate(32, sizeof( unsigned short ));

    dma_timer = xTimerCreate
                       ( /* Just a text name, not used by the RTOS
                         kernel. */
                         "Timer",
                         /* The timer period in ticks, must be
                         greater than 0. */
                         3,
                         /* The timers will auto-reload themselves
                         when they expire. */
                         pdFALSE,
                         /* The ID is used to store a count of the
                         number of times the timer has expired, which
                         is initialised to 0. */
                         ( void * ) 0,
                         /* Each timer calls the same callback when
                         it expires. */
                         vTimerCallback
                       );

    if( xTimerStart( dma_timer, 0 ) != pdPASS )
    {
        /* The timer could not be set into the Active
        state. */
    }


    xTaskCreate(app_data_task,   ( portCHAR * ) "app_data_task",   configMINIMAL_STACK_SIZE * 1, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(GrnLedTask,      ( portCHAR * ) "green_led_task",  configMINIMAL_STACK_SIZE * 1, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(InterfaceTask,   ( portCHAR * ) "interface_task",  configMINIMAL_STACK_SIZE * 5, NULL, tskIDLE_PRIORITY + 2, &interface_handler);
    xTaskCreate(inter_comm_task, ( portCHAR * ) "intercomm_task",  configMINIMAL_STACK_SIZE * 1, NULL, tskIDLE_PRIORITY + 1, &inter_comm_handler);
#if ( INCLUDE_EXTMODBUS == 1 )
//    xTaskCreate(ExtCommTask,        ( portCHAR * ) "extcomm_task",    configMINIMAL_STACK_SIZE * 1, NULL, tskIDLE_PRIORITY + 1, &ext_comm_handler);
#endif
    /* Start the scheduler. */
    vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle task */
    for(;;);

    return 0;
}


