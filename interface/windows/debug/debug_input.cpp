/*
 * debug_input.cpp
 *
 *  Created on: 10 мая 2019 г.
 *      Author: V.Striukov
 */


#include "debug_input.h"

#include "interface.h"

#include "dialogs/dialog_numeric_editor.h"
#include "property/input_property.h"


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
typedef enum {
    StartSequence,

    Sequence_Line = StartSequence,
    Sequence_Voltage,
    Sequence_Current,
    Sequence_ADC,
    Sequence_State,
    Sequence_Coeff,

    EndSequence,

    Coef_StartSequence,

    Sequence_EsNum = Coef_StartSequence,
    Sequence_Uin,
    Sequence_MinusI,
    Sequence_PlusI,
    Sequence_Upe,
    Sequence_Rzero,
    Sequence_Get,
    Sequence_Set,

    Coef_EndSequence,

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

    Button_Start,
    Button_1 = Button_Start,
    Button_2,
    Button_3,

//    Button_Coeff_End,
//    Button_4 = Button_Coeff_End,
//    Button_5,
//    Button_6,
//    Button_7,
//    Button_8,
//    Button_9,
//
//    Button_End = Button_9,

    Dialog_Numeric_Editor_1,
    Dialog_Numeric_Editor_2,
    Dialog_Numeric_Editor_3,
    Dialog_Numeric_Editor_4,
    Dialog_Numeric_Editor_5,
    Dialog_Numeric_Editor_6,
    Dialog_Numeric_Editor_7,
    Dialog_Numeric_Editor_8,
    Dialog_Numeric_Editor_9,


    Button_Coef_Get_Es_1,
    Button_Coef_Set_Es_1,

    Button_Coef_Get_Es_2,
    Button_Coef_Set_Es_2,

    Button_Coef_Get_Es_3,
    Button_Coef_Set_Es_3,


    Widget_Max = 0xFF
};

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugInputTab::DebugInputTab(unsigned char ucID, Widget *pParent)
        :HorizontalDebugTabBase(ucID, pParent)
{
//    unsigned short      sX;
//    unsigned short      sY;

    /*
     * Create pointers to the buttons
     */
    this->pButtons = new RectangularButton *[INPUT_PHASE_COUNT * 3];

    /*
     * Clear pointers
     */
    for(int i = 0; i < INPUT_PHASE_COUNT * 3; i++ )
    {
        this->pButtons[i] = 0;
    }

//
//    for( int i = Button_Coeff_End - Button_Start, id = 0; i < Button_End; i++, id++ )
//    {
//        if( id % 2 == 0 )
//        {
//            sX = 232;
//        }
//        else
//        {
//            sX = 270;
//        }
//
//        sY = 119 + 12 * ( id / 2 );
//
//        this->pButtons[i] = new RectangularButton(Button_Coef_Get_Es_1 + (i - 2), this, sX, sY, 36, 12);
//        this->pButtons[i]->ulStyle = RB_STYLE_FALLING_EDGE | RB_STYLE_OUTLINE | RB_STYLE_FILL;
//        this->pButtons[i]->ulFillColor = ClrGray;
//        this->pButtons[i]->ulPressFillColor = ClrGray;
//    }
}
/******************************************************************************
 *
 *  Render inner content
 *
 ******************************************************************************/
void
DebugInputTab::inner_content_render( tContext *pContext )
{
    char            string[24];
    short           sX;
    short           sY;

//    short           more_sY;

    short           index;

    static const tColumn s_sub_sequences[] =
    {
        { Sequence_Line,    "Line",           30 },
        { Sequence_Voltage, "Voltage",        40 },
        { Sequence_Current, "Current",        55 },
        { Sequence_ADC,     "ADC",            45 },
        { Sequence_State,   "State",          50 },
        { Sequence_Coeff,   "Coeff",          50 },
    };

    for( int i = 0; i < INPUT_FEEDER_COUNT; i++ )
    {
        sX = pContext->sClipRegion.sXMin;
        sY = pContext->sClipRegion.sYMin + 5 + i * 85;

        GrContextForegroundSet(pContext, ClrYellow);
        GrContextFontSet(pContext, g_pFontWTahoma13pt);

        usprintf(string, "Feeder #%d", i + 1);
        GrStringDraw(pContext, string, -1, sX + 21, sY, 0);

        usprintf(string, "Status:");
        GrStringDraw(pContext, string, -1, sX + 160, sY, 0);

        GrContextForegroundSet(pContext, ClrWhite);
        InputFeederPropertyStringGet(i, INPUT_FEEDER_STATUS, string);
        GrStringDraw(pContext, string, -1, sX + 200, sY, 0);

        for(int sequence = StartSequence; sequence < EndSequence; sequence++ )
        {
            sY = pContext->sClipRegion.sYMin + 28 + i * 85;
            sX += s_sub_sequences[sequence].sIndent;

            GrContextForegroundSet(pContext, ClrWheat);
            GrStringDrawCenteredL(pContext, s_sub_sequences[sequence].pcName, -1, sX, sY, 0);

            sY += 5;

            GrContextForegroundSet(pContext, ClrWhite);
            for( int k = 0; k < INPUT_PHASE_COUNT; k++ )
            {
                index = k + i * INPUT_PHASE_COUNT;
                sY += 12;

                switch(sequence)
                {
                case Sequence_Line:
                    usprintf(string, "%d", k + 1);
                    GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                    break;

                case Sequence_Voltage:
                    InputPhasePropertyStringGet(index, INPUT_VOLTAGE, string);
                    GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                    break;

                case Sequence_Current:
                    InputPhasePropertyStringGet(index, INPUT_CURRENT, string);
                    GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                    break;

                case Sequence_ADC:
                    InputPhasePropertyStringGet(index, INPUT_CURRENT_ADCV, string);
                    GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                    break;

                case Sequence_State:
                    InputPhasePropertyStringGet(index, INPUT_STATUS, string);
                    GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                    break;

                case Sequence_Coeff:

                    InputPhasePropertyStringGet(index, INPUT_CURRENT_CALIBRATION_COEFF, string);
                    GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);

                    /*
                     * Coeff Btns
                     */
                    if( !this->pButtons[index] )
                    {
                        this->pButtons[index] = new RectangularButton(Button_1 + index, this, sX-20, sY-7, 36, 12);
                        this->pButtons[index]->ulStyle = RB_STYLE_OUTLINE;
                        this->pButtons[index]->ulOutlineColor = ClrWhite;
                    }
                    else
                    {
                        if( this->pButtons[index]->ulStyle & RB_STYLE_PRESSED )
                        {
                            this->pButtons[index]->ulOutlineColor = ClrGray;
                        }
                        else
                        {
                            this->pButtons[index]->ulOutlineColor = TEXT_COLOR;
                        }
                    }

                    break;

                default:
                    break;
                }

            }
        }
    }


//    static const tColumn s_sub_sequences_2[] =
//    {
//        { Sequence_EsNum,   "#",            20 },
//        { Sequence_Uin,     "Uin",          30 },
//        { Sequence_MinusI,  "-I",           40 },
//        { Sequence_PlusI,   "+I",           40 },
//        { Sequence_Upe,     "Upe",          40 },
//        { Sequence_Rzero,   "R0",           40 },
//        { Sequence_Get,     "Get",          37 },
//        { Sequence_Set,     "Set",          37 },
//    };
//
//    sX = pContext->sClipRegion.sXMin;
//
//    GrContextForegroundSet(pContext, ClrYellow);
//    GrContextFontSet(pContext, g_pFontWTahoma13pt);
//
//    more_sY = sY + 20;
//
//    usprintf(string, "Coeffs");
//    GrStringDrawCenteredL(pContext, string, -1, sX + 160, more_sY, 0);

//
//    for(int sequence = Coef_StartSequence, j = 0; sequence < Coef_EndSequence; sequence++, j++ )
//    {
//        sX += s_sub_sequences_2[j].sIndent;
//        sY = more_sY + 16;
//
//        GrContextForegroundSet(pContext, ClrWheat);
//        GrStringDrawCenteredL(pContext, s_sub_sequences_2[j].pcName, -1, sX, sY, 0);
//
//        sY += 5;
//
//        GrContextForegroundSet(pContext, ClrWhite);
//        for( int k = 0; k < INPUT_PHASE_COUNT; k++ )
//        {
//            index = k;
//            sY += 12;
//
//            switch(sequence)
//            {
//            case Sequence_EsNum:
//                usprintf(string, "%d", k + 1);
//                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
//                break;
//
//            case Sequence_Uin:
//                usprintf(string, "%d", k + 1);
//                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
//                break;
//
//            case Sequence_MinusI:
//                usprintf(string, "%d", k + 1);
//                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
//                break;
//
//            case Sequence_PlusI:
//                usprintf(string, "%d", k + 1);
//                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
//                break;
//
//            case Sequence_Upe:
//                usprintf(string, "%d", k + 1);
//                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
//                break;
//
//            case Sequence_Rzero:
//                usprintf(string, "%d", k + 1);
//                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
//
//                break;
//
//            default:
//                break;
//            }
//
//        }
//    }
}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugInputTab::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                        unsigned long ulParam2)
{
    tRectangle      sPosition;
    tContext        sCtx;
    long            lValue;
//    static char     string[12] = "None";


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

//            GrContextFontSet(&sCtx, g_pFontWTahoma13pt);
//            GrContextForegroundSet(&sCtx, ClrLime);
//            GrStringDrawCenteredL(&sCtx, (char *) "Pressed:", -1, 245, (INPUT_PHASE_COUNT + 1) * 12  + 44, 0);
//            GrStringDrawCenteredL(&sCtx, string, -1, 280, (INPUT_PHASE_COUNT + 1) * 12  + 44, 0);

            break;

        default:
            return 0;
        }

        break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
        switch( ulParam1 )
        {
        case Button_1:
            lValue = InputPhasePropertyGet(0, INPUT_CURRENT_CALIBRATION_COEFF);
            (new DialogNumericEditor(Dialog_Numeric_Editor_1, this, NUM_ED_CENTRAL_COORDS))->Init(lValue, 0);
            break;

        case Button_2:
            lValue = InputPhasePropertyGet(1, INPUT_CURRENT_CALIBRATION_COEFF);
            (new DialogNumericEditor(Dialog_Numeric_Editor_2, this, NUM_ED_CENTRAL_COORDS))->Init(lValue, 0);
            break;

        case Button_3:
            lValue = InputPhasePropertyGet(2, INPUT_CURRENT_CALIBRATION_COEFF);
            (new DialogNumericEditor(Dialog_Numeric_Editor_3, this, NUM_ED_CENTRAL_COORDS))->Init(lValue, 0);
            break;
//
//        case Button_4:
//            lValue = InputPhasePropertyGet(3, INPUT_CURRENT_CALIBRATION_COEFF);
//            (new DialogNumericEditor(Dialog_Numeric_Editor_4, this, NUM_ED_CENTRAL_COORDS))->Init(lValue, 0);
//            break;
//
//        case Button_5:
//            lValue = InputPhasePropertyGet(4, INPUT_CURRENT_CALIBRATION_COEFF);
//            (new DialogNumericEditor(Dialog_Numeric_Editor_5, this, NUM_ED_CENTRAL_COORDS))->Init(lValue, 0);
//            break;
//
//        case Button_6:
//            lValue = InputPhasePropertyGet(5, INPUT_CURRENT_CALIBRATION_COEFF);
//            (new DialogNumericEditor(Dialog_Numeric_Editor_6, this, NUM_ED_CENTRAL_COORDS))->Init(lValue, 0);
//            break;

//        case Button_Coef_Get_Es_1:
//            usprintf(string, " Get 1");
//
//            break;
//
//        case Button_Coef_Set_Es_1:
//            usprintf(string, " Set 1");
//
//            break;
//
//        case Button_Coef_Get_Es_2:
//            usprintf(string, " Get 2");
//
//            break;
//
//        case Button_Coef_Set_Es_2:
//            usprintf(string, " Set 2");
//            break;
        }

        break;
    case WIDGET_NOTIFY_APPLY:
        switch( ulParam1 )
        {
        case Dialog_Numeric_Editor_1:
            InputPhasePropertySet(0, INPUT_CURRENT_CALIBRATION_COEFF, ((DialogNumericEditor *)ulParam2)->ValueGet());
            ((DialogNumericEditor *)ulParam2)->Destroy();
            break;

        case Dialog_Numeric_Editor_2:
            InputPhasePropertySet(1, INPUT_CURRENT_CALIBRATION_COEFF, ((DialogNumericEditor *)ulParam2)->ValueGet());
            ((DialogNumericEditor *)ulParam2)->Destroy();
            break;

        case Dialog_Numeric_Editor_3:
            InputPhasePropertySet(2, INPUT_CURRENT_CALIBRATION_COEFF, ((DialogNumericEditor *)ulParam2)->ValueGet());
            ((DialogNumericEditor *)ulParam2)->Destroy();
            break;
//
//        case Dialog_Numeric_Editor_4:
//            InputPhasePropertySet(3, INPUT_CURRENT_CALIBRATION_COEFF, ((DialogNumericEditor *)ulParam2)->ValueGet());
//            ((DialogNumericEditor *)ulParam2)->Destroy();
//            break;
//
//        case Dialog_Numeric_Editor_5:
//            InputPhasePropertySet(4, INPUT_CURRENT_CALIBRATION_COEFF, ((DialogNumericEditor *)ulParam2)->ValueGet());
//            ((DialogNumericEditor *)ulParam2)->Destroy();
//            break;
//
//        case Dialog_Numeric_Editor_6:
//            InputPhasePropertySet(5, INPUT_CURRENT_CALIBRATION_COEFF, ((DialogNumericEditor *)ulParam2)->ValueGet());
//            ((DialogNumericEditor *)ulParam2)->Destroy();
//            break;
        }

        break;

    case WIDGET_NOTIFY_CANCEL:
        switch( ulParam1 )
        {
        case Dialog_Numeric_Editor_1:
        case Dialog_Numeric_Editor_2:
        case Dialog_Numeric_Editor_3:
//        case Dialog_Numeric_Editor_4:
//        case Dialog_Numeric_Editor_5:
//        case Dialog_Numeric_Editor_6:
            ((DialogNumericEditor *)ulParam2)->Destroy();
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
DebugInputTab::~DebugInputTab(void)
{
    delete[] this->pButtons;
}

