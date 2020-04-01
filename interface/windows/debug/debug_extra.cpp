/*
 * debug_cfg.cpp
 *
 *  Created on: 11 мая 2019 г.
 *      Author: V.Striukov
 */


#include "interface.h"
#include "dialogs/dialog_numeric_editor.h"
#include "dialogs/dialog_adjust_double.h"

#include "dialogs/window_confirm.h"

#include "property/ups_property.h"
#include "property/input_property.h"

#include "debug_extra.h"

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
    StartSequence,

    Seq_1 = StartSequence,
    Seq_2,
    Seq_3,
    Seq_4,
    Seq_5,
    Seq_6,
    Seq_7,
    Seq_8,

    EndSequence,

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
    Canvas_Border,
    Canvas_Main,

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
DebugExtraTab::DebugExtraTab(unsigned char ucID, Widget *pParent)
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
DebugExtraTab::extra_debug_info_render( tContext *pContext, unsigned short sX, unsigned short sY )
{
    char            string[36];
    short           sub_sY;

    static const tColumn s_sub_sequences[] =
    {
        { Seq_1,         "ex_1",          35 },
        { Seq_2,         "ex_2",          35 },
        { Seq_3,         "ex_3",          35 },
        { Seq_4,         "ex_4",          35 },
        { Seq_5,         "ex_5",          35 },
        { Seq_6,         "ex_6",          35 },
        { Seq_7,         "ex_7",          35 },
        { Seq_8,         "ex_8",          35 },
    };

    GrContextForegroundSet(pContext, ClrYellow);

    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "Extra Debug Info");
    GrStringDrawCenteredL(pContext, string, -1, sX + 160, sY, 0);

    for(int seq_name = StartSequence, seq_id = 0; seq_name < EndSequence; seq_name++, seq_id++)
    {
        sub_sY = sY + 16;
        sX += s_sub_sequences[seq_id].sIndent;

        GrContextForegroundSet(pContext, ClrWheat);
        GrStringDrawCenteredL(pContext, s_sub_sequences[seq_id].pcName, -1, sX, sub_sY, 0);

        sub_sY += 3;

        GrContextForegroundSet(pContext, ClrWhite);
        for( int i = 0; i < 1; i++ )
        {
            sub_sY += 12;

            switch(seq_name)
            {
            case Seq_1:
            case Seq_2:
            case Seq_3:
            case Seq_4:
            case Seq_5:
            case Seq_6:
            case Seq_7:
            case Seq_8:
                usprintf(string, "%d", InterCommDataGet(DEBUG_EXTRA_FIELD_1 + seq_id));
                break;


            default:
                usprintf(string, "");
                break;
            }
            GrStringDrawCenteredL(pContext, string, -1, sX, sub_sY, 0);
        }
    }

    return sub_sY;
}


void
DebugExtraTab::inner_content_render( tContext *pContext )
{
    unsigned short sY;


    sY = 20;

    this->extra_debug_info_render( pContext, 0, sY );
}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugExtraTab::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                        unsigned long ulParam2)
{
    tRectangle          sPosition;
    tContext            sCtx;


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
            break;

        default:
            return 0;
        }

        break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
        switch( ulParam1 )
        {
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
DebugExtraTab::~DebugExtraTab(void)
{
    delete[] this->pButtons;
}

