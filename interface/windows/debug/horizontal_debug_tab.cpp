/*
 * horizontal_debug_tab.cpp
 *
 *  Created on: 10 мая 2019 г.
 *      Author: V.Striukov
 */

#include "horizontal_debug_tab.h"

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define TABLE_CELL_TEXT_COLOR                0x005c7b99
#define WINDOW_COLOR                0x00000000
#define TEXT_COLOR                  0x00F0F0F0

//*****************************************************************************
//
// Column description structure
//
//*****************************************************************************
typedef struct
{
    const char *pcName;
          short sIndent;

} tColumn;

//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
    Canvas_Border,
    Canvas_Main,

    Widget_Max = 0xFF
};

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
HorizontalDebugTabBase::HorizontalDebugTabBase(unsigned char ucID, Widget *pParent)
        :Widget(ucID, pParent, 2, 2, 312, 192)
{
    Canvas              *pCanvas;


    /*
     * Main Canvas
     */
    pCanvas = new Canvas(Canvas_Main, this, 2, 2, 312, 192);
    pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;
}
/******************************************************************************
 *
 *  Destructor of this window
 *
 ******************************************************************************/
HorizontalDebugTabBase::~HorizontalDebugTabBase(void)
{

}
