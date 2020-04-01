/*
 * debug_output.cpp
 *
 *  Created on: 10 мая 2019 г.
 *      Author: V.Striukov
 */
#include "interface.h"

#include "dialogs/dialog_numeric_editor.h"
#include "property/dc_output_property.h"

#include "debug_output.h"

/******************************************************************************
 *
 * Position defines
 *
 ******************************************************************************/
#define NUM_ED_CENTRAL_COORDS                       110, 45

/******************************************************************************
 *
 * The colors used in this window
 *
 ******************************************************************************/
#define TABLE_CELL_TEXT_COLOR                0x005c7b99
#define WINDOW_COLOR                0x00000000
#define TEXT_COLOR                  0x00F0F0F0

/******************************************************************************
 *
 *  Column description structure
 *
 ******************************************************************************/
typedef enum {
    StartSequence,

    Sequence_Output = StartSequence,
    Sequence_Voltage,
    Sequence_Current,
    Sequence_Power,
    Sequence_Status,

    EndSequence,

} sequence_t;

typedef struct
{
    sequence_t  seq;
    const char  *pcName;
    short       sIndent;
} tColumn;

/******************************************************************************
 *
 *  The enum of all widget ID's used in this window
 *
 ******************************************************************************/
enum
{
    Canvas_Border,
    Canvas_Main,

    Button_1,
    Button_2,
    Button_3,
    Button_4,
    Button_5,
    Button_6,

    Dialog_Numeric_Editor_1,
    Dialog_Numeric_Editor_2,
    Dialog_Numeric_Editor_3,
    Dialog_Numeric_Editor_4,
    Dialog_Numeric_Editor_5,
    Dialog_Numeric_Editor_6,


    Widget_Max = 0xFF
};


/******************************************************************************
 *
 *  Construcor of this window
 *
 ******************************************************************************/
DebugOutputTab::DebugOutputTab(unsigned char ucID, Widget *pParent)
        :HorizontalDebugTabBase(ucID, pParent)
{
    /*
     * Create pointers to the buttons
     */
    this->pButtons = new RectangularButton *[INPUT_PHASE_COUNT];

    /*
     * Clear pointers
     */
    for(int i = 0; i < INPUT_PHASE_COUNT; i++ )
    {
        this->pButtons[i] = 0;
    }
}

/******************************************************************************
 *
 *  Render inner content
 *
 ******************************************************************************/
void
DebugOutputTab::inner_content_render( tContext *pContext )
{
    char            string[24];
    short           sX;
    short           sY;

    short           index;

    unsigned short      status;

    static const tColumn s_sub_sequences[] =
    {
        { Sequence_Output,      "Out",            45 },
        { Sequence_Voltage,     "Voltage",        40 },
        { Sequence_Current,     "Current",        55 },
        { Sequence_Power,       "Power",          45 },
        { Sequence_Status,      "Status",         50 },
    };

    sX = pContext->sClipRegion.sXMin;
    sY = pContext->sClipRegion.sYMin + 12;

    GrContextForegroundSet(pContext, ClrYellow);
    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "Output DC");
    GrStringDrawCenteredL(pContext, string, -1, sX + 160, sY, 0);

    for(int sequence = StartSequence; sequence < EndSequence; sequence++ )
    {
        sY = pContext->sClipRegion.sYMin + 28;
        sX += s_sub_sequences[sequence].sIndent;

        GrContextForegroundSet(pContext, ClrWheat);
        GrStringDrawCenteredL(pContext, s_sub_sequences[sequence].pcName, -1, sX, sY, 0);

        sY += 5;

        GrContextForegroundSet(pContext, ClrWhite);
        for( int i = 0; i < OUTPUT_DC_COUNT; i++ )
        {
            index = i;
            sY += 12;

            status = DCOutputPropertyGet(index, OUTPUT_DC_STATUS);

            switch(sequence)
            {
            case Sequence_Output:
                usprintf(string, "%d", i + 1);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case Sequence_Voltage:
                if( status )
                {
                    DCOutputPropertyStringGet(index, OUTPUT_DC_VOLTAGE, string);
                }
                else
                {
                    usprintf(string, "-");
                }
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case Sequence_Current:
                if( status )
                {
                    DCOutputPropertyStringGet(index, OUTPUT_DC_CURRENT, string);
                }
                else
                {
                    usprintf(string, "-");
                }
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case Sequence_Power:
                if( status )
                {
                    DCOutputPropertyStringGet(index, OUTPUT_DC_POWER, string);
                }
                else
                {
                    usprintf(string, "-");
                }
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case Sequence_Status:
                DCOutputPropertyStringGet(index, OUTPUT_DC_STATUS, string);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            default:
                break;
            }

        }
    }
}
/******************************************************************************
 *
 *  This function handles the messages for this widget.
 *
 ******************************************************************************/
long
DebugOutputTab::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                        unsigned long ulParam2)
{
    tRectangle    sPosition;
    tContext      sCtx;

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

    default:
        return 0;
    }

    return 1;
}
/******************************************************************************
 *
 *  Destructor of this window
 *
 ******************************************************************************/
DebugOutputTab::~DebugOutputTab(void)
{
    delete[] this->pButtons;
}


