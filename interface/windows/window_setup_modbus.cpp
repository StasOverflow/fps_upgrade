/*
 * window_output_setup.cpp
 *
 *  Created on: 15 нояб. 2018 г.
 *      Author: V.Striukov
 */

#include "interface.h"

#if (INCLUDE_EXTMODBUS == 1)
#include "controls/control_adjustable_editor.h"
#include "dialogs/dialog_confirmation.h"

#include "property/modbus_property.h"
#include "ext_comm.h"

#include "window_setup_modbus.h"

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define WINDOW_COLOR                0x005c7b99
#define BUTTON_COLOR                0x00285a89
#define DISP_COLOR                  0x00bcc7aa
#define TEXT_COLOR                  0x00F0F0F0
#define OUTLINE_COLOR               ClrWhite
//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
    Canvas_Main,
    Canvas_Header,

    Dialog_Confirmation,

    NumericEditor_Start,
    ControlNumericEditor_BaudRate = NumericEditor_Start,
    ControlNumericEditor_Parity,
    ControlNumericEditor_SlaveAddress,
    NumericEditor_End,

    Button_Back,
    Button_Apply,

    Widget_Max = 0xFF,
};

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
WindowSetupModbus::WindowSetupModbus(unsigned char ucID, Widget *pParent)
                  :Widget(ucID, pParent, 0, 0, 320, 240)
{
    RectangularButton    *pButton;
    Canvas               *pCanvas;
    long                 lY;

    const static char *pControlNumericEditorNames[] =
    {
        "Baud Rate:",
        "Parity:",
        "Slave Address:",
    };

    this->usBaudrate = ModbusPropertyValueGet(EXTMODBUS_BAUDRATE);
    this->usParity = ModbusPropertyValueGet(EXTMODBUS_PARITY);
    this->usSlaveAddress = ModbusPropertyValueGet(EXTMODBUS_SLAVE_ADDRESS);


    /*
     * Main Canvas
     */
    pCanvas = new Canvas(Canvas_Main, this, 0, 0, 320, 240);
    pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY
                     | CANVAS_STYLE_FALLING_EDGE;


    /*
     * Header Canvas
     */
    pCanvas = new Canvas(Canvas_Header, this, 0, 2, 320, 25);
    pCanvas->ulStyle = CANVAS_STYLE_TEXT;
    pCanvas->ulTextColor = TEXT_COLOR;
    pCanvas->pFont = g_pFontWTimes22pt;
    pCanvas->pcText = (char *)"Setup / Modbus";

    //
    // Title
    //

    lY = 40;

    for( int i = NumericEditor_Start, iEditor = 0; i < NumericEditor_End; i++, iEditor++)
    {
        //
        // Name canvas
        //
        pCanvas = new Canvas(Widget_Max, this, 15, lY, 100, 30);
        pCanvas->ulStyle = CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_LEFT | CANVAS_STYLE_TEXT_HCENTER;
        pCanvas->ulTextColor = TEXT_COLOR;
        pCanvas->pFont = g_pFontWDoslike15pt;
        pCanvas->pcText = (char *)pControlNumericEditorNames[iEditor];

        /*
         * Adjustable editor
         */
        new ControlAdjustableEditor(i, this, 135, lY, (i == ControlNumericEditor_Parity) ? true : false);

        lY += 50;
    }

    //
    // Buttons
    //
    pButton = new RectangularButton(Button_Back, this, BUTTON_BACK_POSITION);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = BUTTON_COLOR;
    pButton->ulPressFillColor = BUTTON_COLOR;
    pButton->ulOutlineColor = ClrBlack;
    pButton->ulTextColor = TEXT_COLOR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)"< Back";


    //
    // Buttons
    //
    pButton = new RectangularButton(Button_Apply, this, BUTTON_SETUP_POSITION);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = BUTTON_COLOR;
    pButton->ulPressFillColor = BUTTON_COLOR;
    pButton->ulOutlineColor = ClrBlack;
    pButton->ulTextColor = TEXT_COLOR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)"Apply";
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
WindowSetupModbus::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                               unsigned long ulParam2)
{
    tRectangle    sPosition;
    tContext      sCtx;
    char          pcString[16];

    Canvas          *pCanvas;

    pCanvas = (Canvas *)ulParam2;

    switch( ulMsg )
    {
    case WIDGET_NOTIFY_PAINT:
        ((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&sCtx);
        GrContextClipRegionSet(&sCtx, &sPosition);

        GrContextForegroundSet(&sCtx, ClrBlack);
        GrContextBackgroundSet(&sCtx, ClrBlack);

        switch( ulParam1 )
        {
        case Canvas_Main:
            GrImageDraw(&sCtx, g_pucimg_main_with_shapka, 0, 0);
            break;


        default:
            return 0;
        }

        break;


    case WIDGET_NOTIFY_UPDATE:
        switch( ulParam1 )
        {
        case ControlNumericEditor_BaudRate:
            ModbusPropertyStringGetFromValue(EXTMODBUS_BAUDRATE, this->usBaudrate, pcString);
            ((ControlAdjustableEditor *)ulParam2)->Init(this->usBaudrate, 1, pcString);

            break;

        case ControlNumericEditor_Parity:
            ModbusPropertyStringGetFromValue(EXTMODBUS_PARITY, this->usParity, pcString);
            ((ControlAdjustableEditor *)ulParam2)->Init(this->usParity, 1, pcString);

            break;

        case ControlNumericEditor_SlaveAddress:
            ModbusPropertyStringGetFromValue(EXTMODBUS_SLAVE_ADDRESS, this->usSlaveAddress, pcString);
            ((ControlAdjustableEditor *)ulParam2)->Init(this->usSlaveAddress, 1, pcString);

            break;

        default:
            return 0;
        }

        break;

    case WIDGET_NOTIFY_APPLY:
        switch( ulParam1 )
        {
        case Dialog_Confirmation:
            ModbusPropertySet(EXTMODBUS_BAUDRATE, this->usBaudrate);
            ModbusPropertySet(EXTMODBUS_PARITY, this->usParity);
            ModbusPropertySet(EXTMODBUS_SLAVE_ADDRESS, this->usSlaveAddress);

            InterCommDataSet(REMOTE_CTRL_CMD, CMD_SAVE_MBCONFIG);

            pCanvas->Destroy();

            break;

        case ControlNumericEditor_BaudRate:
            this->usBaudrate = ((ControlAdjustableEditor *)ulParam2)->ValueGet();
            ModbusPropertyValidate(EXTMODBUS_BAUDRATE, &this->usBaudrate);

            break;

        case ControlNumericEditor_Parity:
            this->usParity = ((ControlAdjustableEditor *)ulParam2)->ValueGet();
            ModbusPropertyValidate(EXTMODBUS_SLAVE_ADDRESS, &this->usParity);

            break;

        case ControlNumericEditor_SlaveAddress:
            this->usSlaveAddress = ((ControlAdjustableEditor *)ulParam2)->ValueGet();
            ModbusPropertyValidate(EXTMODBUS_SLAVE_ADDRESS, &this->usSlaveAddress);

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
            new DialogConfirmation(Dialog_Confirmation, this, 60, 50);
            break;

        default:
            return 0;
        }

        break;


    case WIDGET_NOTIFY_CANCEL:
        switch( ulParam1 )
        {
        case Dialog_Confirmation:
            pCanvas->Destroy();
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

//*****************************************************************************
//
// Destructor of this window
//
//*****************************************************************************
WindowSetupModbus::~WindowSetupModbus(void)
{

}
#endif
