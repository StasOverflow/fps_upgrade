/*
 * window_irps_tester.cpp
 *
 *  Created on: 25 мар. 2020 г.
 *      Author: V.Striukov
 */


#include "FreeRTOS.h"
#include "task.h"

#include "interface.h"
#include "system/system.h"

#include "property/input_property.h"
#include "property/battery_property.h"
#include "property/inverter_property.h"
#include "property/dc_output_property.h"
#include "property/ac_output_property.h"

#include "property/ups_property.h"
#include "property/psu_property.h"
#include "property/dc_output_property.h"
#include "property/im_property.h"
#include "property/modbus_property.h"

#include "windows/window_irps_tester.h"

#include "lcd_driver.h"



#define TABLE_CELL_TEXT_COLOR               0x0083919f
#define TABLE_CELL_BORDER_COLOR             0x00c9d7e6

#define SERVICE_PANEL_POS_Y                 ( 200 )
#define SERVICE_PANEL_HEIGHT                ( 40 )
#define SERVICE_PANEL_BTN_HEIGHT            ( SERVICE_PANEL_HEIGHT - 2)
#define SERVICE_PANEL_BTN_SCROLL_WIDTH      ( SERVICE_PANEL_BTN_HEIGHT )
#define SERVICE_PANEL_CLR                   ( 0x00700099 )

#define SERVICE_INFO_FONT                   g_pFontWTahoma12pt
#define SERVICE_BUTTON_FONT                 g_pFontWDoslike15pt
#define SERVICE_INFO_BACKGROUND_CLR         0x00300033
#define SERVICE_INFO_TEXT_CLR               0x00CCCCFF
#define SERVICE_BUTTON_TEXT_CLR             0x00CCCCFF
#define SELECTION_COLOR                     ClrCyan

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define WINDOW_COLOR                0x005c7b99
#define BUTTON_COLOR                0x00006fef
#define DISP_COLOR                  0x00285a89
#define TEXT_COLOR                  0x00F0F0F0

#define DEBUG                       0
//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
    Canvas_Main,

    Cvs_Front_Panel,

    Cvs_Service_Panel,
    Cvs_Slave_Id_Caption,
    Cvs_Slave_Id_Cell,
    Cvs_Status_Panel,

    Canvas_Filler,

    Button_Start,

    Btn_Bg_Setup_Fast = Button_Start,
    Btn_Bg_Setup_1,
    Btn_Bg_Setup_3,

    Btn_Setup_F,
    Btn_Setup_1,
    Btn_Setup_3,

    Button_End,

    Widget_Max = 0xFF,
};

#define BTN_ENUM_TO_IDX(x)                  (x - Button_Start)


struct name_param{
    char        name[8];
    void        (*string_get)(unsigned short usUnit, unsigned short usProperty, char *pcString);
    uint8_t     idx;
    uint16_t    param;
    char        unit[2];
};

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
WindowIrpsTester::WindowIrpsTester(unsigned char ucID, Widget *pParent)
                 :Widget(ucID, pParent, 0, 0, 240, 320)
{
    Canvas                      *cvs;


    this->adjust_timer = xTaskGetTickCount();

    cvs = new Canvas(Cvs_Front_Panel, this, 0, 0, 320, 240);
    cvs->ulStyle        = CANVAS_STYLE_OUTLINE      |
                          CANVAS_STYLE_FILL         |
                          CANVAS_STYLE_PAINT_NOTIFY |
                          CANVAS_STYLE_RISING_EDGE;
    cvs->ulFillColor    = SERVICE_PANEL_CLR;
    cvs->ulOutlineColor = SERVICE_PANEL_CLR;

    this->button = new RectangularButton * [6];

    for( uint8_t i = 0, btn_id = 0; btn_id < 3; btn_id++, i++ )
    {
        this->button[i] = new RectangularButton(Btn_Bg_Setup_Fast + btn_id, this,
                                                     35, 65 + btn_id * 51, 50, 51);
//        this->button[i]->ulStyle          = RB_STYLE_OUTLINE | RB_STYLE_FILL;
//        this->button[i]->ulOutlineColor   = SELECTION_COLOR;
//        this->button[i]->ulPressFillColor = SELECTION_COLOR;
    }

    for( uint8_t i = 3, btn_id = 0; btn_id < 3; btn_id++, i++ )
    {
        this->button[i] = new RectangularButton(Btn_Setup_F + btn_id, this,
                                                43, 73 + btn_id * 51, 36, 35);
        this->button[i]->ulStyle          = RB_STYLE_FILL           |
                                            RB_STYLE_FALLING_EDGE;
        this->button[i]->ulFillColor      = SERVICE_PANEL_CLR;
        this->button[i]->ulPressFillColor = SELECTION_COLOR;
    }

    cvs = new Canvas(Canvas_Main, this, 0,  0, 320, 240);
    cvs->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;

    this->adjust_timer = xTaskGetTickCount();
}


void
WindowIrpsTester::handle_interface_cmd(tContext *pContext)
{
    /*
     *  TODO: Dout Hooks goes here
     */
}


void
WindowIrpsTester::inner_context_render(tContext * context)
{
    uint16_t            gap_y;
    uint16_t            gap_x;
    char                string[32];


    GrContextForegroundSet(context, SERVICE_INFO_TEXT_CLR);
    GrContextFontSet(context, g_pFontWCourier21pt);

    gap_x  = 130;
    gap_y  = 30;
    usprintf(string, "Контакт Авария");
    GrStringDraw(context, string, -1, gap_x, gap_y, 0);

    gap_y += 50;
    usprintf(string, "Ускоренный заряд");
    GrStringDraw(context, string, -1, gap_x, gap_y, 0);

    gap_y += 50;
    usprintf(string, "Установка I");
    GrStringDraw(context, string, -1, gap_x, gap_y, 0);

    gap_y += 50;
    usprintf(string, "Установка III");
    GrStringDraw(context, string, -1, gap_x, gap_y, 0);

    gap_x = 47;
    gap_y = 25;

    if( xTaskGetTickCount() - this->adjust_timer >= 2000 )
    {
        this->adjust_timer = xTaskGetTickCount();
        GrTransparentImageDraw(context, g_pucimg_led_disabled_medium, gap_x, gap_y, 0);
    }
    else
    if( xTaskGetTickCount() - this->adjust_timer >= 1000 )
    {
        GrTransparentImageDraw(context, g_pucimg_led_red_medium, gap_x, gap_y, 1);
    }
    else
    {
        GrTransparentImageDraw(context, g_pucimg_led_disabled_medium, gap_x, gap_y, 0);
    }


    RCC_ClocksTypeDef clks;

    RCC_GetClocksFreq(&clks);

    GrContextFontSet(context, SERVICE_INFO_FONT);
    GrContextForegroundSet(context, ClrLime);

    gap_x = 5;
    gap_y = 210;
    usprintf(string, "%d", lcd_refresh_rate_get());
    GrStringDraw(context, string, -1, gap_x, gap_y, 0);

    gap_x = 5;
    gap_y = 222;
    usprintf(string, "%d", lcd_fps_get());
    GrStringDraw(context, string, -1, gap_x, gap_y, 0);
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************

#include "storage.h"
long
WindowIrpsTester::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                        unsigned long ulParam2)
{
    tRectangle      sPosition;
    tContext        context;


    switch( ulMsg )
    {
    case WIDGET_NOTIFY_PAINT:

        ((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);
        GrContextInit(&context);

        GrContextClipRegionSet(&context, &sPosition);

        switch( ulParam1 )
        {
        case Canvas_Main:
            this->inner_context_render(&context);
            break;

        default:
            break;
        }

        break;

    case WIDGET_NOTIFY_BTN_PRESSED:
        switch( ulParam1 )
        {
        case Btn_Bg_Setup_Fast:
        case Btn_Setup_F:
            this->unprsd = 0;
            if( this->curr_btn_pressed == Btn_Setup_F )
            {
                this->curr_btn_pressed = 0;
            }
            else
            {
                this->curr_btn_pressed = Btn_Setup_F;
                this->button[BTN_ENUM_TO_IDX(Btn_Setup_1)]->ulStyle &= ~RB_STYLE_PRESSED;
                this->button[BTN_ENUM_TO_IDX(Btn_Setup_3)]->ulStyle &= ~RB_STYLE_PRESSED;
            }
            break;

        case Btn_Bg_Setup_1:
        case Btn_Setup_1:
            this->unprsd = 0;
            if( this->curr_btn_pressed == Btn_Setup_1 )
            {
                this->curr_btn_pressed = 0;
            }
            else
            {
                this->curr_btn_pressed = Btn_Setup_1;
                this->button[BTN_ENUM_TO_IDX(Btn_Setup_F)]->ulStyle &= ~RB_STYLE_PRESSED;
                this->button[BTN_ENUM_TO_IDX(Btn_Setup_3)]->ulStyle &= ~RB_STYLE_PRESSED;
            }
            break;

        case Btn_Bg_Setup_3:
        case Btn_Setup_3:
            this->unprsd = 0;
            if( this->curr_btn_pressed == Btn_Setup_3 )
            {
                this->curr_btn_pressed = 0;
            }
            else
            {
                this->curr_btn_pressed = Btn_Setup_3;
                this->button[BTN_ENUM_TO_IDX(Btn_Setup_F)]->ulStyle &= ~RB_STYLE_PRESSED;
                this->button[BTN_ENUM_TO_IDX(Btn_Setup_1)]->ulStyle &= ~RB_STYLE_PRESSED;
            }
            break;

        default:
            break;
        }
        break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
        switch( ulParam1 )
        {
        case Btn_Bg_Setup_Fast:
        case Btn_Setup_F:
        case Btn_Bg_Setup_1:
        case Btn_Setup_1:
        case Btn_Bg_Setup_3:
        case Btn_Setup_3:
            this->unprsd = 1;
            if( this->curr_btn_pressed > 0 )
            {
                this->button[BTN_ENUM_TO_IDX(this->curr_btn_pressed)]->ulStyle |= RB_STYLE_PRESSED;
            }
            break;

        default:
            break;
        }
        break;

    default:
        if( this->curr_btn_pressed > 0 )
        {
            this->button[BTN_ENUM_TO_IDX(this->curr_btn_pressed)]->ulStyle |= RB_STYLE_PRESSED;
        }
        return 0;
    }

    return 1;
}

//*****************************************************************************
//
// Destructor of this window
//
//*****************************************************************************
WindowIrpsTester::~WindowIrpsTester(void)
{
    for( uint8_t btn = BTN_ENUM_TO_IDX(Button_Start); btn < BTN_ENUM_TO_IDX(Button_End); btn++ )
    {
        delete this->button[btn];
    }
    delete this->button;
}
