/*
 * adj_editor_themed.cpp
 *
 *  Created on: 27 янв. 2020 г.
 *      Author: V.Striukov
 */

#include "interface.h"
#include "adj_editor_themed.h"


//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define WINDOW_COLOR                0x00700099
#define BUTTON_COLOR                0x00700099
#define DISP_COLOR                  0x00300033
#define BUTTON_TEXT_COLOR           0x00CCCCFF
#define TEXT_COLOR                  0x00CCCCFF

#define EDITOR_HEIGHTS              19
#define EDITOR_WIDTH                85
#define GAP                         2
#define BORDERED_GAP                ( GAP + 2 )

#define SCROLL_DELAY                600
#define SCROLL_RATE                 33

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
AdjEditor::AdjEditor(unsigned char ucID, Widget *pParent,
                     long lX, long lY, short use_arrows)
              :Widget(ucID, pParent, lX, lY, EDITOR_WIDTH, EDITOR_HEIGHTS)
{
    Canvas            *pCanvas;

    //
    // Set initial values
    //
    lValue   = 0;
    lStep    = 1;

    this->iface = use_arrows ? Adj_Iface_Arrows : Adj_Iface_Math_Sign;
    this->btn_color = BUTTON_COLOR;
    this->btn_text_color = BUTTON_TEXT_COLOR;

    this->bg_color = DISP_COLOR;
    this->bg_text_color = TEXT_COLOR;

    pcText = new char[TEXT_LENGTH_MAX + 1];
    pcText[0] = '\0';

    pCanvas = new Canvas(Canvas_Edit, this, EDITOR_HEIGHTS + GAP, 0,
                         EDITOR_WIDTH - EDITOR_HEIGHTS * 2 - GAP * 2,
                         EDITOR_HEIGHTS);
    pCanvas->ulStyle = CANVAS_STYLE_FILL         |
                       CANVAS_STYLE_TEXT         |
                       CANVAS_STYLE_RISING_EDGE  |
                       CANVAS_STYLE_TEXT_VCENTER;
    pCanvas->ulFillColor = this->bg_color;
    pCanvas->ulTextColor = this->bg_text_color;
    pCanvas->pFont = g_pFontWTahoma12pt;
    pCanvas->pcText = this->pcText;

//    pCanvas = new Canvas(Canvas_Edit, this, EDITOR_HEIGHTS + BORDERED_GAP, 2,
//                         EDITOR_WIDTH - EDITOR_HEIGHTS * 2 - BORDERED_GAP * 2,
//                         EDITOR_HEIGHTS - 4);
//    pCanvas->ulStyle = CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT;
//    pCanvas->ulFillColor = this->bg_color;
//    pCanvas->ulTextColor = this->bg_text_color;
//    pCanvas->pFont = g_pFontWTahoma13pt;
//    pCanvas->pcText = this->pcText;

    button[0] = new RectangularButton(Button_Minus, this, 0,
                                    0, EDITOR_HEIGHTS, EDITOR_HEIGHTS);
    button[0]->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT         |
                       RB_STYLE_TEXT         |
                       RB_STYLE_AUTO_REPEAT;
    button[0]->ulFillColor = this->btn_color;
    button[0]->ulPressFillColor = this->btn_color;
    button[0]->ulTextColor = this->btn_text_color;
    if( use_arrows )
    {
        button[0]->pFont = g_pFontWTahoma13pt;
        button[0]->pcText = (char *)"<";
    }
    else
    {
        button[0]->pFont = g_pFontWDoslike15pt;
        button[0]->pcText = (char *)"-";
    }
    button[0]->usAutoRepeatDelay = SCROLL_DELAY;
    button[0]->usAutoRepeatRate = SCROLL_RATE;

    button[1] = new RectangularButton(Button_Plus, this, EDITOR_WIDTH-EDITOR_HEIGHTS,
                                     0, EDITOR_HEIGHTS, EDITOR_HEIGHTS);
    button[1]->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT         |
                       RB_STYLE_AUTO_REPEAT;
    button[1]->ulFillColor = this->btn_color;
    button[1]->ulPressFillColor = this->btn_color;
    button[1]->ulTextColor = this->btn_text_color;
    if( use_arrows )
    {
        button[1]->pFont = g_pFontWTahoma13pt;
        button[1]->pcText = (char *)">";
    }
    else
    {
        button[1]->pFont = g_pFontWDoslike15pt;
        button[1]->pcText = (char *)"+";
    }
    button[1]->usAutoRepeatDelay = SCROLL_DELAY;
    button[1]->usAutoRepeatRate = SCROLL_RATE;

}


void
AdjEditor::enable_state_set( uint8_t enable )
{

    for( uint8_t i = 0; i < 2; i++ )
    {
        if( enable == true )
        {
            button[i]->ulStyle &= ~RB_STYLE_DISABLED;
        }
        else
        {
            button[i]->ulStyle |=  RB_STYLE_DISABLED;
        }
    }
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
AdjEditor::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
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
void AdjEditor::Init( long lValue, long lStep, char *pcText )
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
AdjEditor::~AdjEditor( void )
{
    delete[] pcText;

    for( uint8_t i = 0; i < 2; i++ )
    {
        delete button[i];
    }
}


