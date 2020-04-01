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


int main( void )
{
    IWDGInit();

    LED_Init();

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    sys_timer_init();

    xGrnLedQueue = xQueueCreate(32, sizeof( unsigned short ));

    xTaskCreate(app_data_task,   ( portCHAR * ) "app_data_task",   configMINIMAL_STACK_SIZE * 1, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(GrnLedTask,      ( portCHAR * ) "green_led_task",  configMINIMAL_STACK_SIZE * 1, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(InterfaceTask,   ( portCHAR * ) "interface_task",  configMINIMAL_STACK_SIZE * 5, NULL, tskIDLE_PRIORITY + 1, &interface_handler);
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


