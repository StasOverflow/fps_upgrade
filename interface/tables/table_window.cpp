/*
 * table_window.cpp
 *
 *  Created on: 10 мая 2019 г.
 *      Author: V.Striukov
 */


#include <string.h>
#include "interface.h"
#include "input_property.h"

#include "table_window.h"

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
    Canvas_Table,

    Button_Back,

//    Ind_Connection,

    Widget_Max = 0xFF,
};
//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
TableWindow::TableWindow(unsigned char ucID, Widget *pParent, short rows, short cols)
           :Widget(ucID, pParent, 0, 0, 320, 240)
{
    RectangularButton *pButton;
    Canvas            *pCanvas;

    /*
     *  Main Canvas
     */
    pCanvas = new Canvas(Canvas_Main, this, 0, 0, 320, 240);
    pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;

    /*
     * Header Canvas
     */
    this->Header = new Canvas(Canvas_Header, this, 0, 2, 320, 25);
    this->Header->ulStyle = CANVAS_STYLE_TEXT;
    this->Header->ulTextColor = TEXT_COLOR;
    this->Header->pFont = g_pFontWTimes22pt;

    pCanvas = new Canvas(Canvas_Table, this, 5, 5, 315, 190);
    pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;

    /*
     * Table Canvas
     */
    this->Table = new MainTable(Canvas_Table, this, rows, cols);

    /*
     * Go Back Button
     */
    pButton = new RectangularButton(Button_Back, this, BUTTON_BACK_POSITION);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = BUTTON_COLOR;
    pButton->ulPressFillColor = BUTTON_COLOR;
    pButton->ulOutlineColor = ClrBlack;
    pButton->ulTextColor = TEXT_COLOR;
    pCanvas->ulOutlineColor = OUTLINE_COLOR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)"< Back";

}
//*****************************************************************************
/*
 * Render Table Conents
 */
//*****************************************************************************
void
TableWindow::TableContentsUpdate(tContext *pContext)
{
    /*
     * Function to be extended by subclass.
     * Best suitable for adding table cell logic fillers here
     */
}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
TableWindow::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                        unsigned long ulParam2)
{
    tRectangle      sPosition;
    tContext        sCtx;


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
            GrImageDraw(&sCtx, g_pucimg_main_subwindow, 0, 0);
            break;

        default:
            return 0;
        }

        break;


    case WIDGET_NOTIFY_UPDATE:
        switch( ulParam1 )
        {
        case Canvas_Table:
            this->TableContentsUpdate(&sCtx);
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
TableWindow::~TableWindow(void)
{
    delete this->Table;
}

