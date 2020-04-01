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

#include "windows/window_main.h"

#include "lcd_driver.h"


static uint8_t      mod = 0;
static uint8_t      mode_setup = 0;

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

    Cvs_Service_Panel,
    Cvs_Slave_Id_Caption,
    Cvs_Slave_Id_Cell,
    Cvs_Status_Panel,

    Canvas_Filler,

    Button_Start,

    Btn_Reg_Dec = Button_Start,
    Btn_Reg_Inc,

    Btn_Slave_Addr_Plus,
    Btn_Slave_Addr_Minus,

    Btn_Setup,
    Btn_Connect,

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
WindowMain::WindowMain(unsigned char ucID, Widget *pParent)
           :Widget(ucID, pParent, 0, 0, 240, 320)
{
    Canvas                      *cvs;
    uint8_t                      btn_id;
    uint16_t                     scroll_mode;


    scroll_mode = app_data_item_get(APPDATA_IFACE_SCROLL_MODE);
    mode_setup = 0;

    this->adjust_timer = xTaskGetTickCount();

    this->table = new ModbusTable(Canvas_Filler, this, 0, 0);

    cvs = new Canvas(Cvs_Service_Panel, this, 0, SERVICE_PANEL_POS_Y, 320, SERVICE_PANEL_HEIGHT);
    cvs->ulStyle        = CANVAS_STYLE_OUTLINE | CANVAS_STYLE_FILL;
    cvs->ulFillColor    = SERVICE_PANEL_CLR;
    cvs->ulOutlineColor = SERVICE_PANEL_CLR;

    this->button = new RectangularButton * [6];

    btn_id = BTN_ENUM_TO_IDX(Btn_Reg_Dec);
    this->button[btn_id] = new RectangularButton(Btn_Reg_Dec, this, 1,
                                      SERVICE_PANEL_POS_Y + 1, SERVICE_PANEL_BTN_SCROLL_WIDTH,
                                      SERVICE_PANEL_BTN_HEIGHT);
    this->button[btn_id]->ulStyle = RB_STYLE_FILL            |
                                    RB_STYLE_AUTO_REPEAT     |
                                    RB_STYLE_TEXT            |
                                    RB_STYLE_OUTLINE         |
                                    RB_STYLE_FALLING_EDGE;
    this->button[btn_id]->ulFillColor      = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulOutlineColor   = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulPressFillColor = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulTextColor      = SERVICE_BUTTON_TEXT_CLR;
    this->button[btn_id]->usAutoRepeatDelay = 600;
    this->button[btn_id]->usAutoRepeatRate  = 75;
    this->button[btn_id]->pFont   = g_pFontWDoslike15pt;
    if( scroll_mode == Scroll_Mode_Page )
        this->button[btn_id]->pcText  = (char *) "<";
    else
        this->button[btn_id]->pcText  = (char *) "-";
    if( this->table->at_left_limit() )
        this->button[btn_id]->ulStyle |=  RB_STYLE_DISABLED;
    else
        this->button[btn_id]->ulStyle &= ~RB_STYLE_DISABLED;

    btn_id = BTN_ENUM_TO_IDX(Btn_Reg_Inc);
    this->button[btn_id] = new RectangularButton(Btn_Reg_Inc, this,
                                      1 + SERVICE_PANEL_BTN_SCROLL_WIDTH, SERVICE_PANEL_POS_Y + 1,
                                      SERVICE_PANEL_BTN_SCROLL_WIDTH, SERVICE_PANEL_BTN_HEIGHT);
    this->button[btn_id]->ulStyle = RB_STYLE_FILL            |
                                    RB_STYLE_AUTO_REPEAT     |
                                    RB_STYLE_TEXT            |
                                    RB_STYLE_OUTLINE         |
                                    RB_STYLE_FALLING_EDGE;
    this->button[btn_id]->ulFillColor      = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulOutlineColor   = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulPressFillColor = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulTextColor      = SERVICE_BUTTON_TEXT_CLR;
    this->button[btn_id]->usAutoRepeatDelay = 600;
    this->button[btn_id]->usAutoRepeatRate  = 75;
    this->button[btn_id]->pFont   = g_pFontWDoslike15pt;
    if( scroll_mode == Scroll_Mode_Page )
        this->button[btn_id]->pcText  = (char *) ">";
    else
        this->button[btn_id]->pcText  = (char *) "+";
    if( this->table->at_right_limit() )
        this->button[btn_id]->ulStyle |=  RB_STYLE_DISABLED;
    else
        this->button[btn_id]->ulStyle &= ~RB_STYLE_DISABLED;

    cvs = new Canvas(Cvs_Slave_Id_Cell, this, 1 + SERVICE_PANEL_BTN_SCROLL_WIDTH * 2 + 1,
                     SERVICE_PANEL_POS_Y + 2, 42, SERVICE_PANEL_BTN_HEIGHT - 2);
    cvs->ulStyle        = CANVAS_STYLE_FILL | CANVAS_STYLE_RISING_EDGE;
    cvs->ulFillColor    = SERVICE_INFO_BACKGROUND_CLR;

    cvs = new Canvas(Cvs_Slave_Id_Caption, this, 1 + SERVICE_PANEL_BTN_SCROLL_WIDTH * 2 + 2,
                     SERVICE_PANEL_POS_Y + 2, 40, SERVICE_PANEL_BTN_HEIGHT / 2 + 1);
    cvs->ulStyle        = CANVAS_STYLE_TEXT;
    cvs->ulTextColor    = SERVICE_INFO_TEXT_CLR;
    cvs->pFont          = SERVICE_INFO_FONT;
    cvs->pcText         = (char *) "Slave";

    btn_id = BTN_ENUM_TO_IDX(Btn_Slave_Addr_Plus);
    this->button[btn_id] = new RectangularButton(Btn_Slave_Addr_Plus, this,
                                      1 + SERVICE_PANEL_BTN_SCROLL_WIDTH * 2 + 44, SERVICE_PANEL_POS_Y + 1,
                                      SERVICE_PANEL_BTN_SCROLL_WIDTH / 2, SERVICE_PANEL_BTN_HEIGHT / 2);
    this->button[btn_id]->ulStyle = RB_STYLE_AUTO_REPEAT     |
                                    RB_STYLE_FILL            |
                                    RB_STYLE_TEXT            |
                                    RB_STYLE_OUTLINE         |
                                    RB_STYLE_AUTO_REPEAT     |
                                    RB_STYLE_FALLING_EDGE;
    this->button[btn_id]->ulFillColor       = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulOutlineColor    = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulPressFillColor  = SERVICE_PANEL_CLR;
    this->button[btn_id]->usAutoRepeatDelay = 700;
    this->button[btn_id]->usAutoRepeatRate  = 50;
    this->button[btn_id]->ulTextColor       = SERVICE_BUTTON_TEXT_CLR;
    this->button[btn_id]->pFont   = g_pFontWDoslike15pt;
    this->button[btn_id]->pcText  = (char *) "+";

    btn_id = BTN_ENUM_TO_IDX(Btn_Slave_Addr_Minus);
    this->button[btn_id] = new RectangularButton(Btn_Slave_Addr_Minus, this,
                                      1 + SERVICE_PANEL_BTN_SCROLL_WIDTH * 2 + 44,
                                      SERVICE_PANEL_POS_Y + SERVICE_PANEL_BTN_SCROLL_WIDTH / 2 + 1,
                                      SERVICE_PANEL_BTN_SCROLL_WIDTH / 2, SERVICE_PANEL_BTN_HEIGHT / 2);
    this->button[btn_id]->ulStyle = RB_STYLE_AUTO_REPEAT     |
                                    RB_STYLE_FILL            |
                                    RB_STYLE_TEXT            |
                                    RB_STYLE_OUTLINE         |
                                    RB_STYLE_AUTO_REPEAT     |
                                    RB_STYLE_FALLING_EDGE;
    this->button[btn_id]->ulFillColor       = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulOutlineColor    = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulPressFillColor  = SERVICE_PANEL_CLR;
    this->button[btn_id]->usAutoRepeatDelay = 700;
    this->button[btn_id]->usAutoRepeatRate  = 50;
    this->button[btn_id]->ulTextColor       = SERVICE_BUTTON_TEXT_CLR;
    this->button[btn_id]->pFont   = g_pFontWDoslike15pt;
    this->button[btn_id]->pcText  = (char *) "-";

    btn_id = BTN_ENUM_TO_IDX(Btn_Setup);
    this->button[btn_id] = new RectangularButton(Btn_Setup, this, 140, SERVICE_PANEL_POS_Y + 1,
                                                 127, SERVICE_PANEL_BTN_HEIGHT);
    this->button[btn_id]->ulStyle = RB_STYLE_FALLING_EDGE   |
                                    RB_STYLE_FILL           |
                                    RB_STYLE_OUTLINE;
    this->button[btn_id]->ulFillColor      = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulOutlineColor   = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulPressFillColor = SERVICE_PANEL_CLR;

    cvs = new Canvas(Cvs_Status_Panel, this, 140 + 3, SERVICE_PANEL_POS_Y + 4,
                     120, SERVICE_PANEL_BTN_HEIGHT - 6);
    cvs->ulStyle        = CANVAS_STYLE_FILL | CANVAS_STYLE_RISING_EDGE;
    cvs->ulFillColor    = SERVICE_INFO_BACKGROUND_CLR;

    btn_id = BTN_ENUM_TO_IDX(Btn_Connect);
    this->button[btn_id] = new RectangularButton(Btn_Connect, this,
                                            320 - SERVICE_PANEL_BTN_SCROLL_WIDTH - 15,
                                            SERVICE_PANEL_POS_Y + 1, SERVICE_PANEL_BTN_SCROLL_WIDTH + 15,
                                            SERVICE_PANEL_BTN_HEIGHT);
    this->button[btn_id]->ulStyle = RB_STYLE_FALLING_EDGE   |
                                    RB_STYLE_FILL           |
                                    RB_STYLE_OUTLINE        |
                                    RB_STYLE_AUTO_REPEAT    |
                                    RB_STYLE_TEXT;
    this->button[btn_id]->ulFillColor      = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulOutlineColor   = SERVICE_PANEL_CLR;
    this->button[btn_id]->ulPressFillColor = SERVICE_PANEL_CLR;
    this->button[btn_id]->usAutoRepeatRate = 1;
    this->button[btn_id]->usAutoRepeatDelay = 500;
    this->button[btn_id]->pFont = SERVICE_BUTTON_FONT;
    this->button[btn_id]->ulTextColor = SERVICE_BUTTON_TEXT_CLR;
    this->button[btn_id]->pcText = (char *) "Conn";

    cvs = new Canvas(Canvas_Main, this, 0,  0, 320, 240);
    cvs->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;
}


void
WindowMain::actual_background_render(tContext *pContext)
{
    GrImageDraw(pContext, g_pucimg_main, 0, 0);
}


#include "i2c_sw.h"
void
WindowMain::inner_context_render(tContext * context)
{
    uint16_t            gap_y;
    uint16_t            gap_x;
    char                string[32];
    char                sub_str[4][10];
    uint16_t            width;
    uint16_t            value_1;
    uint16_t            value_2;

    uint8_t             conn_ok_state = inter_comm_conn_present();

    GrContextForegroundSet(context, SERVICE_INFO_TEXT_CLR);
    GrContextFontSet(context, SERVICE_INFO_FONT);

    gap_x = 100;
    gap_y = 227;

    usprintf(string, "%d", app_data_item_get(APPDATA_MBUS_SLAVE_ID));
    GrStringDrawCenteredL(context, string, -1, gap_x, gap_y, 0);

    gap_x = 310;
    gap_y = 229;
    if( inter_comm_conn_state_get() )
    {
        if( conn_ok_state )
        {
            if( inter_comm_activity_get() )
                GrTransparentImageDraw(context, g_pucimg_led_cyan_small, gap_x, gap_y, 0);
            else
                GrTransparentImageDraw(context, g_pucimg_led_disabled_dark_small, gap_x, gap_y, 0);
        }
        else
        {
            GrTransparentImageDraw(context, g_pucimg_led_red_small, gap_x, gap_y, 1);
        }
    }
    else
    {
        GrTransparentImageDraw(context, g_pucimg_led_disabled_dark_small, gap_x, gap_y, 0);
    }

    gap_x = 147;
    gap_y = 206;
    usprintf(string, "Tx  : %d", inter_comm_conn_instances_get());
    GrStringDraw(context, string, -1, gap_x, gap_y, 0);

    gap_x = 147;
    gap_y = 221;
    usprintf(string, "Err: %d", inter_comm_conn_errs_get());
    GrStringDraw(context, string, -1, gap_x, gap_y, 0);


    gap_x = 258;
    gap_y = 221;
    app_data_str_get(APPDATA_SERIAL_BAUDRATE, sub_str[0]);
    app_data_str_get(APPDATA_SERIAL_DATABITS, sub_str[1]);
    app_data_str_get(APPDATA_SERIAL_PARITY,   sub_str[2]);
    app_data_str_get(APPDATA_SERIAL_STOPBITS, sub_str[3]);

    usprintf(string, "%s-%s-%c-%s", sub_str[0], sub_str[1], sub_str[2][0], sub_str[3]);
    width = GrStringWidthGet(context, string, strlen(string));
    GrStringDraw(context, string, -1, gap_x - width, gap_y, 0);

    gap_x = 258;
    gap_y = 206;
    value_1 = app_data_item_get(APPDATA_COMM_REG_START_ADDR);
    value_2 = app_data_item_get(APPDATA_COMM_REG_TOTAL_QUAN) + value_1 - 1;
    usprintf(string, "%d-%d", value_1, value_2);
    width = GrStringWidthGet(context, string, strlen(string));
    GrStringDraw(context, string, -1, gap_x - width, gap_y, 0);

    GrContextForegroundSet(context, ClrLime);

    gap_x = 5;
    gap_y = 210;
    usprintf(string, "%d", lcd_refresh_rate_get());
    GrStringDraw(context, string, -1, gap_x, gap_y, 0);

    gap_x = 5;
    gap_y = 222;
    usprintf(string, "%d", lcd_fps_get());
    GrStringDraw(context, string, -1, gap_x, gap_y, 0);
//
//    for( uint8_t i = 0; i < 10; i++ )
//    {
//        if( i & 1 )
//        {
//            gap_y  = 222;
//        }
//        else
//        {
//            gap_x += 30;
//            gap_y  = 210;
//        }
//        usprintf(string, "%d", app_data_item_get(APPDATA_DEBUG_FIELD_1 + i));
//        GrStringDraw(context, string, -1, gap_x, gap_y, 0);
//    }
}


void
WindowMain::button_disable_handler( void )
{
    static uint16_t     btn_at_limit_state_prev[2];
    uint16_t            compare_val;


    for( uint8_t btn_id = 0; btn_id < 2; btn_id++ )
    {
        compare_val = btn_id == 0 ? this->table->at_left_limit() : this->table->at_right_limit();
        if( compare_val != btn_at_limit_state_prev[btn_id] )
        {
            btn_at_limit_state_prev[btn_id] = compare_val;

            if( compare_val )
            {
                this->button[BTN_ENUM_TO_IDX(Btn_Reg_Dec + btn_id)]->ulStyle |=  RB_STYLE_DISABLED;
            }
            else
            {
                this->button[BTN_ENUM_TO_IDX(Btn_Reg_Dec + btn_id)]->ulStyle &= ~RB_STYLE_DISABLED;
            }

            this->button[BTN_ENUM_TO_IDX(Btn_Reg_Dec + btn_id)]->ulStyle &= ~RB_STYLE_PRESSED;
        }
    }
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************

#include "storage.h"
long
WindowMain::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
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
            this->button_disable_handler();
            break;

        default:
            break;
        }

        break;

    case WIDGET_NOTIFY_BTN_PRESSED:
        switch( ulParam1 )
        {
        case Btn_Reg_Dec:
            this->table->page_scroll(Prev);
            break;

        case Btn_Reg_Inc:
            this->table->page_scroll(Next);
            break;

        case Btn_Slave_Addr_Plus:
        case Btn_Slave_Addr_Minus:
            app_data_item_adjust(APPDATA_MBUS_SLAVE_ID, (ulParam1 == Btn_Slave_Addr_Minus) ? -1 : 1);
            break;

        default:
            break;
        }
        break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
        switch( ulParam1 )
        {
        case Btn_Setup:
            this->~WindowMain();
            WindowManager::MessagePost(WM_MSG_SHOW, Window_Setup_Mbus_Props);
            break;

        case Btn_Connect:
            inter_comm_conn_state_set(inter_comm_conn_state_get() ^ 1);
            break;

        default:
            break;
        }
        break;

    default:
        return 0;
    }

    return 1;
}

//*****************************************************************************
//
// Destructor of this window
//
//*****************************************************************************
WindowMain::~WindowMain(void)
{
    this->table->~ModbusTable();
    delete this->table;

    for( uint8_t btn = BTN_ENUM_TO_IDX(Button_Start); btn < BTN_ENUM_TO_IDX(Button_End); btn++ )
    {
        delete this->button[btn];
    }
    delete this->button;
}
