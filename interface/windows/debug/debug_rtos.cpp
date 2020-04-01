/*
 * debug_rtos.cpp
 *
 *  Created on: 14 мая 2019 г.
 *      Author: V.Striukov
 */

#include "interface.h"

#include "debug_rtos.h"


//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define TABLE_CELL_TEXT_COLOR                0x005c7b99
#define WINDOW_COLOR                0x00000000
#define TEXT_COLOR                  0x00F0F0F0
/******************************************************************************
 *
 *  Other useful defines
 *
 ******************************************************************************/
#define RESET_BUTTONS_SIZE          80, 30
//*****************************************************************************
//
// Column description structure
//
//*****************************************************************************
typedef enum {

    StartRtosSequence,

    EndRtosSequence,

} sequence_t;

typedef struct
{
    sequence_t  seq;
    const char  *pcName;
    short       sIndent;
} tColumn;

//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
    Button_Begin,


    Button_End,


    Widget_Max = 0xFF
};


static const int window_button_count = Button_End - Button_Begin;

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugRtosTab::DebugRtosTab(unsigned char ucID, Widget *pParent)
        :HorizontalDebugTabBase(ucID, pParent)
{
    /*
     * Create pointers to the buttons
     */
    this->pButtons = new RectangularButton *[window_button_count];

    /*
     * Clear pointers
     */
    for(int i = 0; i < window_button_count; i++ )
    {
        this->pButtons[i] = 0;
    }
}
/******************************************************************************
 *
 *  Render inner content
 *
 ******************************************************************************/
unsigned short
DebugRtosTab::rtos_debug_info_render( tContext *pContext, unsigned short sX, unsigned short sY )
{
    /*
    char            string[36];
    short           sub_sX;

    static const tColumn s_sub_sequences[] =
    {
        { Sequence_Sends,         "Sends",       20 },
        { Sequence_Errors,        "Errors",        17 },
    };

    GrContextForegroundSet(pContext, ClrYellow);

    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "Connection Quality");
    GrStringDrawCenteredL(pContext, string, -1, sX + 50, sY, 0);

    for(int seq_name = StartConnectionSequence, seq_id = 0; seq_name < EndConnectionSequence; seq_name++, seq_id++)
    {
        sub_sX = sX + 12;
        sY += s_sub_sequences[seq_id].sIndent;

        GrContextForegroundSet(pContext, ClrWheat);
        GrStringDraw(pContext, s_sub_sequences[seq_id].pcName, -1, sub_sX - 10, sY - 6, 0);

        sub_sX += 4;

        GrContextForegroundSet(pContext, ClrWhite);
        for( int i = 0; i < 1; i++ )
        {
            sub_sX += 70;

            switch(seq_name)
            {

            default:
                usprintf(string, "");
                break;
            }
            GrStringDrawCenteredL(pContext, string, -1, sub_sX, sY, 0);
        }
    }
    */
    return sY;
}


void
DebugRtosTab::inner_content_render( tContext *pContext )
{
    unsigned short sY;
}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugRtosTab::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                        unsigned long ulParam2)
{
    tRectangle          sPosition;
    tContext            sCtx;
    long                value;


    switch( ulMsg )
    {
    case WIDGET_NOTIFY_PAINT:
        ((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&sCtx);
        GrContextClipRegionSet(&sCtx, &sPosition);
        GrContextFontSet(&sCtx, g_pFontWTahoma13pt);

        switch( ulParam1 )
        {
        case Canvas_Main:
            this->inner_content_render(&sCtx);

        default:
            return 0;
        }

        break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
        switch( ulParam1 )
        {

        default:
            break;
        }

        break;
    case WIDGET_NOTIFY_APPLY:
        switch( ulParam1 )
        {


        default:
            break;
        }

        break;

    case WIDGET_NOTIFY_CANCEL:
        switch( ulParam1 )
        {

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
DebugRtosTab::~DebugRtosTab(void)
{
    for( int i = 0; i < window_button_count; i++ )
    {
        delete this->pButtons[i];
    }
    delete[] this->pButtons;
}
