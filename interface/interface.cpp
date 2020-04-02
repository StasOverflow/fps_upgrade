
#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "drivers/sram.h"
#include "drivers/lcd_driver.h"
#include "drivers/touch.h"
#include "interface.h"

#include "tables/modbus_table.h"

#include "images/img_main.c"
#include "images/img_main_subwindow.c"
#include "images/img_main_with_shapka.c"
#include "images/img_debug.c"
#include "images/img_info.c"

#include "images/img_all_conns_dis.c"

#include "images/img_bad_crosshair.c"

#include "images/img_led_yellow_small.c"
#include "images/img_led_green_small.c"
#include "images/img_led_green_upgraded_small.c"
#include "images/img_led_red_small.c"
#include "images/img_led_disabled_small.c"
#include "images/img_led_disabled_dark_small.c"
#include "images/img_led_empty_small.c"
#include "images/img_led_cyan_small.c"

#include "images/img_led_green_medium.c"
#include "images/img_led_red_medium.c"
#include "images/img_led_disabled_medium.c"

#include "images/img_btn_ohm.c"
#include "images/img_btn_battery.c"
#include "images/img_btn_settings.c"

#include "images/img_icon_battery.c"
#include "images/img_icon_air.c"

#include "images/img_mode_btn_selected.c"

#include "images/img_pulse_logo.c"

#include "images/img_icon_conn_disconnected_alarm.c"
#include "images/img_icon_conn_connecting_alarm.c"
#include "images/img_icon_conn_connected_alarm.c"

#include "timers.h"


extern TaskHandle_t interface_handler;
extern TaskHandle_t display_handler;
static WindowManager    *manager;

//*****************************************************************************
//
// Сodepage mapping functions
//
//*****************************************************************************
const static tCodePointMap g_psCodepointMappings[] =
{   /* usSrcCodepage */     /* usFontCodepage */    /* pfnMapChar */
	{ CODEPAGE_UTF_8,        CODEPAGE_UNICODE,     GrMapUTF8_Unicode },
};

//*****************************************************************************
//
// Default text rendering parameters
//
//*****************************************************************************
const static tGrLibDefaults g_psGrLibSettingDefaults =
{
	GrDefaultStringRenderer,          /* *pfnStringRenderer  */
	g_psCodepointMappings,            /* *pCodePointMapTable */
	CODEPAGE_UTF_8,                   /* usCodepage          */
	sizeof(g_psCodepointMappings) /
	sizeof(g_psCodepointMappings[0]), /* ucNumCodePointMaps  */
	0                                 /* ucReserved          */
};

//*****************************************************************************
static
void InterfaceUpdateProc( void )
{
	static uint8_t      times_updated = 0;

	{
//	    if( times_updated < 63 )
	    {
	        times_updated++;

            WIDGET_ROOT->Update();
	    }
//	    else
//	    {
//            PIN_SET(LED_RED_PIN);
//	        vTaskDelay(30);
//            PIN_CLR(LED_RED_PIN);
//	    }
	}
}


//*****************************************************************************
//
// Interface main task
//
//*****************************************************************************
void InterfaceTask( void *pvParameters )
{
	static TickType_t       xBacklightOnTimer;
    extern volatile unsigned char   frame_loading_done;


//	/* Await for notify to launch */
//    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

	/* Initialize the external SRAM. */
	SRAM_Init();

	/* Run external SRAM test */
//	SRAM_Test();

//    GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_RESET);
//	if( SRAM_TestStatusGet() )
//	{
//	    GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_RESET);
//	}
//	else
//	{
//        GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_SET);
//	}

	/* Initialize the display driver. */
	LCD_Init();

	/* Set the display orientation */
	DisplayOrientationSet(DISPLAY_ORIENTATION_LANDSCAPE);

	/* Initialize the touch screen driver */
	TouchScreenInit();

	/* Route touch screen messages to the widget tree */
	TouchScreenCallbackSet(WidgetPointerMessage);

	GrLibInit(&g_psGrLibSettingDefaults);

	/* Create a root widget */
	WIDGET_ROOT = new Widget(0, 0, 0, 0, DisplayWidthGet(), DisplayHeightGet());
	manager = new WindowManager();

	/* Post show window message and handle it */
//    manager->MessagePost(WM_MSG_SHOW, Window_Irps_Tester);
    manager->MessagePost(WM_MSG_SHOW, Window_Main);

	/* Start LCD */
	DisplayEnable();

	xBacklightOnTimer = xTaskGetTickCount();

	for( ;; )
	{
        if( !DisplayBacklightStateGet() )
        {
            /* Wait some time before turning on backlight */
            if( (xTaskGetTickCount() - xBacklightOnTimer) >= 250 )
            {
                /* Turn ON backlight */
                DisplayBacklightStateSet(1);
            }
        }

        /* Process any messages in the widget message queue */
        WIDGET_ROOT->MessageQueueProcess();
        manager->MessageQueueProcess();

        if( !frame_loading_done )
        {
            InterfaceUpdateProc();
            frame_loading_done = 1;
        }

        vTaskSuspend(interface_handler);
	}
}
