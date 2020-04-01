/*
 * debug_system.cpp
 *
 *  Created on: 13 мая 2019 г.
 *      Author: V.Striukov
 */

#include "interface.h"
#include "dialogs/dialog_numeric_editor.h"

#include "property/debug_property.h"
#include "property/im_property.h"

#include "debug_system.h"

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

    StartAcqmbSequence,

    Sequence_Acqmb_Uout = StartAcqmbSequence,
    Sequence_Acqmb_Uadc,
    Sequence_Acqmb_K,
    Sequence_Acqmb_T1,
    Sequence_Acqmb_T2,
    Sequence_Acqmb_T3,
    Sequence_Acqmb_Status,
    Sequence_Acqmb_Errors,

    EndAcqmbSequence,

    StartApsSequence,

    Sequence_Aps_Uin = StartApsSequence,
    Sequence_Aps_Uout,
    Sequence_Aps_Status,
    Sequence_Aps_Errors,

    EndApsSequence,

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

    Button_Acqmb_Coeff = Button_Begin,

    Button_End,

    Editor_Acqmb_Coeff,

    Widget_Max = 0xFF
};


static const int window_button_count = Button_End - Button_Begin;

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugSystemTab::DebugSystemTab(unsigned char ucID, Widget *pParent)
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
DebugSystemTab::system_acqmb_render( tContext *pContext, unsigned short sX, unsigned short sY )
{
    char            string[36];
    short           sub_sY;

    static const tColumn s_sub_sequences[] =
    {
        { Sequence_Acqmb_Uout,      "Uout",     28 },
        { Sequence_Acqmb_Uadc,      "Uadc",     40 },
        { Sequence_Acqmb_K,         "K",        40 },
        { Sequence_Acqmb_T1,        "T1",       30 },
        { Sequence_Acqmb_T2,        "T2",       30 },
        { Sequence_Acqmb_T3,        "T3",       30 },
        { Sequence_Acqmb_Status,    "Status",   35 },
        { Sequence_Acqmb_Errors,    "Errors",   45 },
    };

    GrContextForegroundSet(pContext, ClrYellow);

    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "Acquire Module B (ACQMB)");
    GrStringDrawCenteredL(pContext, string, -1, sX + 160, sY, 0);

    for(int seq_name = StartAcqmbSequence, seq_id = 0; seq_name < EndAcqmbSequence; seq_name++, seq_id++)
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
            case Sequence_Acqmb_Uout:
                DebugPropertyStringGet(ACQM_VOLTAGE_VALUE, string);
                break;

            case Sequence_Acqmb_Uadc:
                DebugPropertyStringGet(ACQM_VOLTAGE_ADCV, string);
                break;

            case Sequence_Acqmb_K:
                DebugPropertyStringGet(ACQM_VOLTAGE_COEFF, string);

                if( !this->pButtons[0] )
                {
                    this->pButtons[0] = new RectangularButton(Button_Acqmb_Coeff, this, sX - 19, sub_sY - 9, 33, 16);
                    this->pButtons[0]->ulStyle = RB_STYLE_OUTLINE;
                    this->pButtons[0]->ulOutlineColor = ClrWhite;
                }
                else
                {
                    if( this->pButtons[0]->ulStyle & RB_STYLE_PRESSED )
                    {
                        this->pButtons[0]->ulOutlineColor = ClrGray;
                    }
                    else
                    {
                        this->pButtons[0]->ulOutlineColor = TEXT_COLOR;
                    }
                }
                break;

            case Sequence_Acqmb_T1:
                DebugPropertyStringGet(ACQM_TEMP_1_VALUE, string);
                break;

            case Sequence_Acqmb_T2:
                DebugPropertyStringGet(ACQM_TEMP_2_VALUE, string);
                break;

            case Sequence_Acqmb_T3:
                DebugPropertyStringGet(ACQM_TEMP_3_VALUE, string);
                break;

            case Sequence_Acqmb_Status:
                DebugPropertyStringGet(ACQM_COMM_STATUS, string);
                break;

            case Sequence_Acqmb_Errors:
                DebugPropertyStringGet(ACQM_COMM_ERRORS, string);
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


unsigned short
DebugSystemTab::system_aps_render( tContext *pContext, unsigned short sX, unsigned short sY )
{
    char            string[36];
    short           sub_sY;


    static const tColumn s_sub_sequences[] =
    {
        { Sequence_Aps_Uin,         "Uin",      80 },
        { Sequence_Aps_Uout,        "Uout",     40 },
        { Sequence_Aps_Status,      "Status",   50 },
        { Sequence_Aps_Errors,      "Errors",   55 },
    };

    GrContextForegroundSet(pContext, ClrYellow);

    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "Auxiliary Power Supply (APS)");
    GrStringDrawCenteredL(pContext, string, -1, sX + 160, sY, 0);

    for(int seq_name = StartApsSequence, seq_id = 0; seq_name < EndApsSequence; seq_name++, seq_id++)
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
            case Sequence_Aps_Uin:
                DebugPropertyStringGet(APS_INPUT_VOLTAGE, string);
                break;

            case Sequence_Aps_Uout:
                DebugPropertyStringGet(APS_OUTPUT_VOLTAGE, string);
                break;

            case Sequence_Aps_Status:
                DebugPropertyStringGet(APS_COMM_STATUS, string);
                break;

            case Sequence_Aps_Errors:
                DebugPropertyStringGet(APS_COMM_ERRORS, string);
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
DebugSystemTab::inner_content_render( tContext *pContext )
{
    unsigned short sY;


    sY = 20;
    sY = this->system_aps_render( pContext, pContext->sClipRegion.sXMin, sY );

    sY += 26;
    sY = this->system_acqmb_render( pContext, pContext->sClipRegion.sXMin, sY );
}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugSystemTab::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
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
        case Button_Acqmb_Coeff:
            value = DebugPropertyGet(ACQM_VOLTAGE_COEFF);
            (new DialogNumericEditor(Editor_Acqmb_Coeff, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
            break;

        default:
            break;
        }

        break;
    case WIDGET_NOTIFY_APPLY:
        switch( ulParam1 )
        {
        case Editor_Acqmb_Coeff:
            DebugPropertySet(ACQM_VOLTAGE_COEFF, ((DialogNumericEditor *)ulParam2)->ValueGet());
            ((DialogNumericEditor *)ulParam2)->Destroy();
            break;


        default:
            break;
        }

        break;

    case WIDGET_NOTIFY_CANCEL:
        switch( ulParam1 )
        {
        case Editor_Acqmb_Coeff:
            ((DialogNumericEditor *)ulParam2)->Destroy();
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
DebugSystemTab::~DebugSystemTab(void)
{
    for( int i = 0; i < window_button_count; i++ )
    {
        delete this->pButtons[i];
    }
    delete[] this->pButtons;
}


