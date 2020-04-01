/*
 * control_numeric_editor_v_0.9.cpp
 *
 *  Created on: 7 мая 2019 г.
 *      Author: V.Striukov
 */

#include "interface.h"
#include "controls/control_adjustable_editor.h"

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define WINDOW_COLOR                0x005c7b99
#define BUTTON_COLOR                0x00285a89
#define DISP_COLOR                  0x00bcc7aa
#define BUTTON_TEXT_COLOR           0x00F0F0F0
#define TEXT_COLOR                  0x00000000


#define EDITOR_HEIGHTS              30

//*****************************************************************************
//
// The string buffer length
//
//*****************************************************************************
#define TEXT_LENGTH_MAX             ( 6 )

//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
    Canvas_Edge,
    Canvas_Edit,

    Button_Minus,
    Button_Plus,

    Widget_Max = 0xFF
};

//*****************************************************************************
//
// Constructor
//
//*****************************************************************************
ControlAdjustableEditor::ControlAdjustableEditor(unsigned char ucID, Widget *pParent,
                                                 long lX, long lY, short use_arrows)
              :Widget(ucID, pParent, lX, lY, 180, 30)
{
    RectangularButton *pButton;
    Canvas            *pCanvas;

    //
    // Set initial values
    //
    lValue   = 0;
    lStep    = 1;

    pcText = new char[TEXT_LENGTH_MAX + 1];
    pcText[0] = '\0';

    pCanvas = new Canvas(Canvas_Edge, this, 45, 0, 95, EDITOR_HEIGHTS);
    pCanvas->ulStyle = CANVAS_STYLE_RISING_EDGE;
    pCanvas->ulFillColor = WINDOW_COLOR;

    pCanvas = new Canvas(Canvas_Edit, this, 47, 2, 91, EDITOR_HEIGHTS - 4);
    pCanvas->ulStyle = CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT;
    pCanvas->ulFillColor = DISP_COLOR;
    pCanvas->ulTextColor = TEXT_COLOR;
    pCanvas->pFont = g_pFontWTimes22pt;
    pCanvas->pcText = this->pcText;

    pButton = new RectangularButton(Button_Minus, this,  10, 0, EDITOR_HEIGHTS, EDITOR_HEIGHTS);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT         |
                       RB_STYLE_AUTO_REPEAT;
    pButton->ulFillColor = BUTTON_COLOR;
    pButton->ulPressFillColor = BUTTON_COLOR;
    pButton->ulOutlineColor = ClrBlack;
    pButton->ulTextColor = BUTTON_TEXT_COLOR;
    if( use_arrows )
    {
        pButton->pFont = g_pFontWDoslike15pt;
        pButton->pcText = (char *)"<";
    }
    else
    {
        pButton->pFont = g_pFontWTimes25pt;
        pButton->pcText = (char *)"-";
    }
    pButton->usAutoRepeatDelay = 700;
    pButton->usAutoRepeatRate = 100;

    pButton  = new RectangularButton(Button_Plus, this, 145, 0, EDITOR_HEIGHTS, EDITOR_HEIGHTS);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT         |
                       RB_STYLE_AUTO_REPEAT;
    pButton->ulFillColor = BUTTON_COLOR;
    pButton->ulPressFillColor = BUTTON_COLOR;
    pButton->ulOutlineColor = ClrBlack;
    pButton->ulTextColor = BUTTON_TEXT_COLOR;
    if( use_arrows )
    {
        pButton->pFont = g_pFontWDoslike15pt;
        pButton->pcText = (char *)">";
    }
    else
    {
        pButton->pFont = g_pFontWTimes25pt;
        pButton->pcText = (char *)"+";
    }
    pButton->usAutoRepeatDelay = 700;
    pButton->usAutoRepeatRate = 100;
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
ControlAdjustableEditor::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                                 unsigned long ulParam2)
{
    switch( ulMsg )
    {
    case WIDGET_MSG_UPDATE:
        //
        // Notify parent
        //
        Widget::ParentNotify(this, WIDGET_NOTIFY_UPDATE);

        break;

    case WIDGET_NOTIFY_BTN_PRESSED:
        switch( ulParam1 )
        {
        case Button_Minus:
            lValue -= lStep;
            Widget::ParentNotify(this, WIDGET_NOTIFY_APPLY);
            break;

        case Button_Plus:
            lValue += lStep;
            Widget::ParentNotify(this, WIDGET_NOTIFY_APPLY);
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
// Init
//
//*****************************************************************************
void ControlAdjustableEditor::Init( long lValue, long lStep, char *pcText )
{
    this->lValue   = lValue;
    this->lStep    = lStep;

    strncpy(this->pcText, pcText, TEXT_LENGTH_MAX);
    this->pcText[TEXT_LENGTH_MAX] = '\0';
}

//*****************************************************************************
//
// Destructor
//
//*****************************************************************************
ControlAdjustableEditor::~ControlAdjustableEditor( void )
{
    delete[] pcText;
}
