/*
 * window_output_setup.cpp
 *
 *  Created on: 15 нояб. 2018 г.
 *      Author: V.Striukov
 */


#include "interface.h"

#include "dialogs/dialog_confirmation.h"

#include "property/modbus_property.h"
#include "ext_comm.h"

#include "window_setup_mbus_props.h"
#include "../../system/app_data.h"

#include "lcd_driver.h"

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************

#define SERVICE_PANEL_POS_Y                 ( 201 )
#define SERVICE_PANEL_HEIGHT                ( 39 )
#define SERVICE_PANEL_BTN_HEIGHT            ( SERVICE_PANEL_HEIGHT - 2)
#define SERVICE_PANEL_BTN_SCROLL_WIDTH      ( SERVICE_PANEL_BTN_HEIGHT )
#define SERVICE_PANEL_CLR                   ( 0x00700099 )

#define SERVICE_INFO_FONT                   g_pFontWTahoma12pt
#define SERVICE_BUTTON_FONT                 g_pFontWDoslike15pt
#define SERVICE_INFO_BACKGROUND_CLR         0x00300033
#define SERVICE_INFO_TEXT_CLR               0x00CCCCFF
#define SERVICE_BUTTON_TEXT_CLR             SERVICE_INFO_TEXT_CLR

#define WORKING_SPACE_HEIGHT                ( SERVICE_PANEL_POS_Y - 25 )

#define WINDOW_COLOR                        0x005c7b99
#define BUTTON_COLOR                        0x00285a89
#define DISP_COLOR                          0x00bcc7aa
#define OUTLINE_COLOR                       ClrWhite



/****
 *  The enum of all widget ID's used in this window
 */
enum
{
    Cvs_Main,

    Cvs_Secret_Line,

    Cvs_Header_1,
    Cvs_Header_2,
    Cvs_ReadWrite,
    Cvs_Connection,
    Cvs_Service_Panel,

    Cvs_Caption,

    Cvs_Debug,

    Dialog_Confirmation_Apply,
    Dialog_Confirmation_Format_Reset,
    Dialog_Confirmation_Defaults_Restore,

    Editor_Start,

    Editor_Left_Start = Editor_Start,
    Editor_Slave_Id = Editor_Left_Start,
    Editor_Format,              // Editor_Function
    Editor_Reg_Address,
    Editor_Reg_Quantity,
    Editor_Scroll_Mode,         // Editor_Scan_Rate
    Editor_Left_End,

    Editor_Right_Start = Editor_Left_End,
    Editor_Baud_Rate = Editor_Right_Start,
    Editor_Data_Bits,
    Editor_Parity,
    Editor_Stop_Bits,
    Editor_Right_End,

    Editor_End = Editor_Right_End,

    Button_Back,
    Button_Apply,
    Button_Format_Reset,
    Button_Restore_Defaults,

    Widget_Max = 0xFF,
};

/****
 *  Construcor
 */
WindowSetupMbusProps::WindowSetupMbusProps(unsigned char ucID, Widget *pParent)
                     :Widget(ucID, pParent, 0, 0, 320, 240)
{
    RectangularButton    *pButton;
    Canvas               *pCanvas;
    uint16_t              pos_y;
    uint16_t              pos_x;

    const static char *table_editor_names_left[] =
    {
        "Slave ID:",
        "Default Fmt:",
        "Reg Addr:",
        "Quantity:",
        "Scroll Mode:",
    };

    const static char *table_editor_names_right[] =
    {
        "Baud Rate:",
        "Data Bits:",
        "Parity:",
        "Stop Bits:",
    };

    this->editor        = new AdjEditor * [Editor_End - Editor_Start];

    this->scroll_mode   = app_data_item_get(APPDATA_IFACE_SCROLL_MODE);
    this->def_format    = app_data_item_get(APPDATA_IFACE_DEFAULT_FORMAT);

    this->baud          = app_data_item_get(APPDATA_SERIAL_BAUDRATE);
    this->parity        = app_data_item_get(APPDATA_SERIAL_PARITY);
    this->data_bits     = app_data_item_get(APPDATA_SERIAL_DATABITS);
    this->stop_bits     = app_data_item_get(APPDATA_SERIAL_STOPBITS);

    this->slave_id      = app_data_item_get(APPDATA_MBUS_SLAVE_ID);
    this->start_addr    = app_data_item_get(APPDATA_COMM_REG_START_ADDR);
    this->reg_quan      = app_data_item_get(APPDATA_COMM_REG_TOTAL_QUAN);

    // Main Canvas
    pCanvas = new Canvas(Cvs_Main, this, 0, 0, 320, 240);
    pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;
//                     | CANVAS_STYLE_FALLING_EDGE;

    // Read/Write Setup Frame
    pCanvas = new Canvas(Cvs_ReadWrite, this, 1, 24, 159, WORKING_SPACE_HEIGHT + 1);
    pCanvas->ulStyle = CANVAS_STYLE_FILL
                     | CANVAS_STYLE_RISING_EDGE;
    pCanvas->ulFillColor = SERVICE_PANEL_CLR;

    // Connection Setup Frame
    pCanvas = new Canvas(Cvs_Connection, this, 161, 24, 159, WORKING_SPACE_HEIGHT + 1);
    pCanvas->ulStyle = CANVAS_STYLE_FILL
                     | CANVAS_STYLE_RISING_EDGE;
    pCanvas->ulFillColor = SERVICE_PANEL_CLR;

    // To mask pixels in between above two
    pCanvas = new Canvas(Cvs_Secret_Line, this, 160, 0, 1, SERVICE_PANEL_POS_Y);
    pCanvas->ulStyle = CANVAS_STYLE_OUTLINE;
    pCanvas->ulOutlineColor = SERVICE_PANEL_CLR;

    // To mask mask pixels under read/write setup (horizontal line)
    pCanvas = new Canvas(Cvs_Secret_Line, this, 0, 23, 160, 1);
    pCanvas->ulStyle = CANVAS_STYLE_OUTLINE;
    pCanvas->ulOutlineColor = SERVICE_PANEL_CLR;

    // To mask mask pixels under read/write setup (vertical line)
    pCanvas = new Canvas(Cvs_Secret_Line, this, 0, 24, 0, SERVICE_PANEL_POS_Y);
    pCanvas->ulStyle = CANVAS_STYLE_OUTLINE;
    pCanvas->ulOutlineColor = SERVICE_PANEL_CLR;

    // Header Canvases
    pCanvas = new Canvas(Cvs_Header_1, this, 0, 0, 160, 23);
    pCanvas->ulStyle = CANVAS_STYLE_TEXT | CANVAS_STYLE_FILL;
    pCanvas->ulTextColor = SERVICE_BUTTON_TEXT_CLR;
    pCanvas->ulFillColor = SERVICE_PANEL_CLR;
    pCanvas->pFont = g_pFontWDoslike15pt;
    pCanvas->pcText = (char *)"Read/Write Setup";

    pCanvas = new Canvas(Cvs_Header_2, this, 160, 0, 160, 24);
    pCanvas->ulStyle = CANVAS_STYLE_TEXT | CANVAS_STYLE_FILL;
    pCanvas->ulTextColor = SERVICE_BUTTON_TEXT_CLR;
    pCanvas->ulFillColor = SERVICE_PANEL_CLR;
    pCanvas->pFont = g_pFontWDoslike15pt;
    pCanvas->pcText = (char *)"Connection Setup";

    // Right header Button names
    pCanvas = new Canvas(Cvs_Caption, this, 180, 30, 45, 24);
    pCanvas->ulStyle = CANVAS_STYLE_TEXT;
    pCanvas->ulTextColor = SERVICE_BUTTON_TEXT_CLR;
    pCanvas->pFont = g_pFontWTahoma12pt;
    pCanvas->pcText = (char *)"Serial";

    pCanvas = new Canvas(Cvs_Caption, this, 260, 30, 35, 23);
    pCanvas->ulStyle = CANVAS_STYLE_TEXT;
    pCanvas->ulTextColor = ClrGray;     //SERVICE_BUTTON_TEXT_CLR;
    pCanvas->pFont = g_pFontWTahoma12pt;
    pCanvas->pcText = (char *)"TCP";

    // Bottom Button Panel
    pCanvas = new Canvas(Cvs_Service_Panel, this, 0, SERVICE_PANEL_POS_Y, 320, 40);
    pCanvas->ulStyle = CANVAS_STYLE_FILL;
    pCanvas->ulFillColor = SERVICE_PANEL_CLR;

    for( int seq = 0; seq < 2; seq++ )
    {
        pos_y = 32;
        pos_x = 7 + 160 * seq;
        switch( seq )
        {
        case 0:
            for( int i = Editor_Left_Start, edit_id = 0; i < Editor_Left_End; i++, edit_id++ )
            {
                // Caption cvs
                pCanvas = new Canvas(Widget_Max, this, pos_x, pos_y, 65, 19);
                pCanvas->ulStyle = CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_LEFT | CANVAS_STYLE_TEXT_HCENTER;
                pCanvas->ulTextColor = SERVICE_BUTTON_TEXT_CLR;
                pCanvas->pFont = g_pFontWTahoma12pt;
                pCanvas->pcText = (char *)table_editor_names_left[edit_id];

                // Adjustable Editor
                this->editor[edit_id] = new AdjEditor(i, this, pos_x + 64, pos_y, false);

                pos_y += 35;
            }
            break;

        case 1:
            pos_y += 35;
            for( int i = Editor_Right_Start, edit_id = 0; i < Editor_Right_End; i++, edit_id++ )
            {
                // Caption cvs
                pCanvas = new Canvas(Widget_Max, this, pos_x, pos_y, 65, 19);
                pCanvas->ulStyle = CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_LEFT | CANVAS_STYLE_TEXT_HCENTER;
                pCanvas->ulTextColor = SERVICE_BUTTON_TEXT_CLR;
                pCanvas->pFont = g_pFontWTahoma12pt;
                pCanvas->pcText = (char *)table_editor_names_right[edit_id];

                // Adjustable Editor
                this->editor[edit_id] = new AdjEditor(i, this, pos_x + 64, pos_y, (i == Editor_Parity) ? true : false);

                if( i == Editor_Data_Bits )
                {
                    this->editor[edit_id]->enable_state_set( false );
                }

                pos_y += 35;
            }
            break;
        }
    }



    // Button Back
    pButton = new RectangularButton(Button_Back, this, 1, SERVICE_PANEL_POS_Y + 1,
                                    53, SERVICE_PANEL_BTN_HEIGHT);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_OUTLINE      |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = SERVICE_PANEL_CLR;
    pButton->ulPressFillColor = SERVICE_PANEL_CLR;
    pButton->ulOutlineColor = SERVICE_PANEL_CLR;
    pButton->ulTextColor = SERVICE_BUTTON_TEXT_CLR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)"< Back";

    // Button Apply
    pButton = new RectangularButton(Button_Apply, this, 320 - 53, SERVICE_PANEL_POS_Y + 1,
                                    53, SERVICE_PANEL_BTN_HEIGHT);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_OUTLINE      |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = SERVICE_PANEL_CLR;
    pButton->ulPressFillColor = SERVICE_PANEL_CLR;
    pButton->ulOutlineColor = SERVICE_PANEL_CLR;
    pButton->ulTextColor = SERVICE_BUTTON_TEXT_CLR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)"Apply";

    // Button Format Reset
    pButton = new RectangularButton(Button_Format_Reset, this, 159-67, SERVICE_PANEL_POS_Y + 1,
                                    67, SERVICE_PANEL_BTN_HEIGHT);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_OUTLINE      |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = SERVICE_PANEL_CLR;
    pButton->ulPressFillColor = SERVICE_PANEL_CLR;
    pButton->ulOutlineColor = SERVICE_PANEL_CLR;
    pButton->ulTextColor = SERVICE_BUTTON_TEXT_CLR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)"Fmt Rst";

    // Button Restore Defaults
    pButton = new RectangularButton(Button_Restore_Defaults, this, 162, SERVICE_PANEL_POS_Y + 1,
                                    67, SERVICE_PANEL_BTN_HEIGHT);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_OUTLINE      |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = SERVICE_PANEL_CLR;
    pButton->ulPressFillColor = SERVICE_PANEL_CLR;
    pButton->ulOutlineColor = SERVICE_PANEL_CLR;
    pButton->ulTextColor = SERVICE_BUTTON_TEXT_CLR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)"Defaults";


    pCanvas = new Canvas(Cvs_Debug, this, 0, 0, 320, 240);
    pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;
}

/****
 *  This function handles the messages for this widget.
 */
long
WindowSetupMbusProps::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                               unsigned long ulParam2)
{
    tRectangle      sPosition;
    tContext        sCtx;
    char            string[16];
    int32_t         value;

    static const uint16_t   regs_1[] = {
        APPDATA_MBUS_SLAVE_ID,
        APPDATA_IFACE_DEFAULT_FORMAT,
        APPDATA_COMM_REG_START_ADDR,
        APPDATA_COMM_REG_TOTAL_QUAN,
        APPDATA_IFACE_SCROLL_MODE,
    };
    static const uint16_t   regs_2[] = {
            APPDATA_SERIAL_BAUDRATE,
            APPDATA_SERIAL_DATABITS,
            APPDATA_SERIAL_PARITY,
            APPDATA_SERIAL_STOPBITS,
    };

    switch( ulMsg )
    {
    case WIDGET_NOTIFY_PAINT:
        ((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&sCtx);
        GrContextClipRegionSet(&sCtx, &sPosition);

        GrContextForegroundSet(&sCtx, ClrLime);
        GrContextFontSet(&sCtx, g_pFontWDoslike15pt);

        switch( ulParam1 )
        {
        case Cvs_Debug:

            for( uint8_t i = 0; i < 5; i++ )
            {
                usprintf(string, "%d", app_data_item_get(regs_1[i]));
                GrStringDraw(&sCtx, string, strlen(string), 12, 35 + 35 * i, 0);
            }

            for( uint8_t i = 0; i < 4; i++ )
            {
                usprintf(string, "%d", app_data_item_get(regs_2[i]));
                GrStringDraw(&sCtx, string, strlen(string), 172, 70 + 35 * i, 0);
            }

//            GrImageDraw(&sCtx, g_pucimg_main_with_shapka, 0, 0);
            GrContextForegroundSet(&sCtx, ClrLime);
            GrContextFontSet(&sCtx, g_pFontWTahoma12pt);

            usprintf(string, "%d", lcd_refresh_rate_get());
            GrStringDraw(&sCtx, string, -1, 5, 210, 0);

            usprintf(string, "%d", lcd_fps_get());
            GrStringDraw(&sCtx, string, -1, 5, 222, 0);
            break;

        default:
            return 0;
        }
        break;


    case WIDGET_NOTIFY_UPDATE:
        switch( ulParam1 )
        {
        case Editor_Baud_Rate:
            app_data_str_from_val(APPDATA_SERIAL_BAUDRATE, this->baud, string);
            ((AdjEditor *)ulParam2)->Init(this->baud, app_data_item_step_get(APPDATA_SERIAL_BAUDRATE), string);
            break;

        case Editor_Parity:
            app_data_str_from_val(APPDATA_SERIAL_PARITY, this->parity, string);
            ((AdjEditor *)ulParam2)->Init(this->parity, app_data_item_step_get(APPDATA_SERIAL_PARITY), string);
            break;

        case Editor_Slave_Id:
            app_data_str_from_val(APPDATA_MBUS_SLAVE_ID, this->slave_id, string);
            ((AdjEditor *)ulParam2)->Init(this->slave_id, app_data_item_step_get(APPDATA_MBUS_SLAVE_ID), string);
            break;

        case Editor_Data_Bits:
            app_data_str_from_val(APPDATA_SERIAL_DATABITS, this->data_bits, string);
            ((AdjEditor *)ulParam2)->Init(this->data_bits, app_data_item_step_get(APPDATA_SERIAL_DATABITS), string);
            break;

        case Editor_Stop_Bits:
            app_data_str_from_val(APPDATA_SERIAL_STOPBITS, this->stop_bits, string);
            ((AdjEditor *)ulParam2)->Init(this->stop_bits, app_data_item_step_get(APPDATA_SERIAL_STOPBITS), string);
            break;

        case Editor_Reg_Address:
            app_data_str_from_val(APPDATA_COMM_REG_START_ADDR, this->start_addr, string);
            ((AdjEditor *)ulParam2)->Init(this->start_addr, app_data_item_step_get(APPDATA_COMM_REG_START_ADDR), string);
            break;

        case Editor_Reg_Quantity:
            app_data_str_from_val(APPDATA_COMM_REG_TOTAL_QUAN, this->reg_quan, string);
            ((AdjEditor *)ulParam2)->Init(this->reg_quan, app_data_item_step_get(APPDATA_COMM_REG_TOTAL_QUAN), string);
            break;

        case Editor_Format:
            app_data_str_from_val(APPDATA_IFACE_DEFAULT_FORMAT, this->def_format, string);
            ((AdjEditor *)ulParam2)->Init(this->def_format, app_data_item_step_get(APPDATA_IFACE_DEFAULT_FORMAT), string);
            break;

        case Editor_Scroll_Mode:
            app_data_str_from_val(APPDATA_IFACE_SCROLL_MODE, this->scroll_mode, string);
            ((AdjEditor *)ulParam2)->Init(this->scroll_mode, app_data_item_step_get(APPDATA_IFACE_SCROLL_MODE), string);
            break;

        default:
            return 0;
        }

        break;

    case WIDGET_NOTIFY_APPLY:
        switch( ulParam1 )
        {
        case Dialog_Confirmation_Apply:
            app_data_item_set(APPDATA_SERIAL_BAUDRATE,      this->baud);
            app_data_item_set(APPDATA_SERIAL_PARITY,        this->parity);
            app_data_item_set(APPDATA_SERIAL_DATABITS,      this->data_bits);
            app_data_item_set(APPDATA_SERIAL_STOPBITS,      this->stop_bits);
            app_data_item_set(APPDATA_MBUS_SLAVE_ID,        this->slave_id);

            app_data_item_set(APPDATA_COMM_REG_TOTAL_QUAN,  this->reg_quan);
            app_data_item_set(APPDATA_COMM_REG_START_ADDR,  this->start_addr);

            app_data_item_set(APPDATA_IFACE_DEFAULT_FORMAT, this->def_format);
            app_data_item_set(APPDATA_IFACE_SCROLL_MODE,    this->scroll_mode);

            ((DialogConfirmation *)ulParam2)->Destroy();
            break;

        case Dialog_Confirmation_Defaults_Restore:
            app_data_restore_defaults();

            this->baud          = app_data_item_get(APPDATA_SERIAL_BAUDRATE);
            this->parity        = app_data_item_get(APPDATA_SERIAL_PARITY);
            this->data_bits     = app_data_item_get(APPDATA_SERIAL_DATABITS);
            this->stop_bits     = app_data_item_get(APPDATA_SERIAL_STOPBITS);
            this->slave_id      = app_data_item_get(APPDATA_MBUS_SLAVE_ID);

            this->reg_quan      = app_data_item_get(APPDATA_COMM_REG_TOTAL_QUAN);
            this->start_addr    = app_data_item_get(APPDATA_COMM_REG_START_ADDR);

            this->def_format    = app_data_item_get(APPDATA_IFACE_DEFAULT_FORMAT);
            this->scroll_mode   = app_data_item_get(APPDATA_IFACE_SCROLL_MODE);

            ((DialogConfirmation *)ulParam2)->Destroy();
            break;

        case Dialog_Confirmation_Format_Reset:
            inter_comm_format_reset_to_defaults();
            ((DialogConfirmation *)ulParam2)->Destroy();
            break;

        case Editor_Baud_Rate:
            value = ((AdjEditor *)ulParam2)->ValueGet();
            this->baud = value < 0 ? 0 : value;

            sys_param_validate(APPDATA_SERIAL_BAUDRATE, this->baud);
            break;

        case Editor_Data_Bits:
            value = ((AdjEditor *)ulParam2)->ValueGet();
            this->data_bits = value < 0 ? 0 : value;

            sys_param_validate(APPDATA_SERIAL_DATABITS, this->data_bits);
            break;

        case Editor_Stop_Bits:
            value = ((AdjEditor *)ulParam2)->ValueGet();
            this->stop_bits = value < 0 ? 0 : value;

            sys_param_validate(APPDATA_SERIAL_STOPBITS, this->stop_bits);
            break;

        case Editor_Parity:
            value = ((AdjEditor *)ulParam2)->ValueGet();
            this->parity = value < 0 ? 0 : value;

            sys_param_validate(APPDATA_SERIAL_PARITY, this->parity);
            break;

        case Editor_Slave_Id:
            value = ((AdjEditor *)ulParam2)->ValueGet();
            this->slave_id = value < 0 ? 0 : value;

            sys_param_validate(APPDATA_MBUS_SLAVE_ID, this->slave_id);
            break;

        case Editor_Reg_Address:
            value = ((AdjEditor *)ulParam2)->ValueGet();
            this->start_addr = value < 0 ? 0 : value;

            sys_param_validate(APPDATA_COMM_REG_START_ADDR, this->start_addr);
            break;

        case Editor_Reg_Quantity:
            value = ((AdjEditor *)ulParam2)->ValueGet();
            this->reg_quan = value < 0 ? 0 : value;

            sys_param_validate(APPDATA_COMM_REG_TOTAL_QUAN, this->reg_quan);
            break;

        case Editor_Format:
            value = ((AdjEditor *)ulParam2)->ValueGet();
            this->def_format = value < 0 ? 0 : value;

            sys_param_validate(APPDATA_IFACE_DEFAULT_FORMAT, this->def_format);
            break;

        case Editor_Scroll_Mode:
            value = ((AdjEditor *)ulParam2)->ValueGet();
            this->scroll_mode = value < 0 ? 0 : value;

            sys_param_validate(APPDATA_IFACE_SCROLL_MODE, this->scroll_mode);
            break;

        default:
            return 0;
        }
        break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
        switch( ulParam1 )
        {
        case Button_Back:
            WindowManager::MessagePost(WM_MSG_CLOSE, 0);
            break;

        case Button_Apply:
            new DialogConfirmation(Dialog_Confirmation_Apply, this, 60, 50);
            break;

        case Button_Restore_Defaults:
            new DialogConfirmation(Dialog_Confirmation_Defaults_Restore, this, 60, 50);
            break;

        case Button_Format_Reset:
            new DialogConfirmation(Dialog_Confirmation_Format_Reset, this, 60, 50);
            break;

        default:
            return 0;
        }
        break;

    case WIDGET_NOTIFY_CANCEL:
        switch( ulParam1 )
        {
        case Dialog_Confirmation_Apply:
            ((DialogConfirmation *)ulParam2)->Destroy();
            WindowManager::MessagePost(WM_MSG_CLOSE, 0);
            break;
        case Dialog_Confirmation_Defaults_Restore:
            ((DialogConfirmation *)ulParam2)->Destroy();
            break;
        case Dialog_Confirmation_Format_Reset:
            ((DialogConfirmation *)ulParam2)->Destroy();
            break;

        default:
            return 0;
        }
        break;

    default:
        return 0;
    }

    return 1;
}

/***
 *  Destructor
 */
WindowSetupMbusProps::~WindowSetupMbusProps(void)
{
    delete[] this->editor;
}

