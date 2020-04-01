/*
 * debug_psu.cpp
 *
 *  Created on: 11 мая 2019 г.
 *      Author: V.Striukov
 */

#include "interface.h"

#include "dialogs/dialog_numeric_editor.h"
#include "property/psu_property.h"

#include "debug_psu.h"

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
    PSU_StartSequence,

    Sequence_PSU = PSU_StartSequence,
    Sequence_Uset,
    Sequence_Uout,
    Sequence_Iout,
    Sequence_Tconv,
    Sequence_Status,
    Sequence_Errors,

    PSU_EndSequence,

    PSUGroup_StartSequence,

    GroupSequence_PSU = PSUGroup_StartSequence,
    GroupSequence_Enable,
    GroupSequence_Uset,
    GroupSequence_Uout,
    GroupSequence_Imax,
    GroupSequence_Iout,
    GroupSequence_Tcomp,
    GroupSequence_Status,

    PSUGroup_EndSequence,


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

    Uset_ButtonPSUGroupEnable_Startpoint = Button_Start,
    ButtonPSUGroupEnable,
    Uset_ButtonPSUGroupEnable_Endpoint = Uset_ButtonPSUGroupEnable_Startpoint + PSU_GROUP_COUNT,

    Uset_Button_Startpoint,
    Uset_Button = Uset_Button_Startpoint,
    Uset_Button_Endpoint = Uset_Button_Startpoint + PSU_GROUP_COUNT,

    Imax_Button_Startpoint,
    Imax_Setup_Button = Imax_Button_Startpoint,
    Imax_Button_Endpoint = Imax_Button_Startpoint + PSU_GROUP_COUNT,

    Tcomp_Button_Startpoint,
    Tcomp_Setup_Button = Tcomp_Button_Startpoint,
    Tcomp_Button_Endpoint = Tcomp_Button_Startpoint + PSU_GROUP_COUNT,

    Button_End,

    Voltage_Editor_Startpoint,
    Voltage_Editor = Voltage_Editor_Startpoint,
    Voltage_Editor_Endpoint = Voltage_Editor_Startpoint + PSU_GROUP_COUNT,

    Current_Editor_Startpoint,
    Current_Editor = Current_Editor_Startpoint,
    Current_Editor_Endpoint = Current_Editor_Startpoint + PSU_GROUP_COUNT,

    Tcomp_Editor_Startpoint,
    Tcomp_Editor = Tcomp_Editor_Startpoint,
    Tcomp_Editor_Endpoint = Tcomp_Editor_Startpoint + PSU_GROUP_COUNT,


    Widget_Max = 0xFF
};

#define BUTTON_COUNT            ( Button_End - Button_Start )

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugPSUTab::DebugPSUTab(unsigned char ucID, Widget *pParent)
        :HorizontalDebugTabBase(ucID, pParent)
{
    /*
     * Create pointers to the buttons
     */
    this->pButtons = new RectangularButton * [PSU_GROUP_COUNT * BUTTON_COUNT];

    /*
     * Clear pointers
     */
    for(int i = 0; i < PSU_GROUP_COUNT * BUTTON_COUNT; i++)
    {
        this->pButtons[i] = 0;
    }


    for(int i = 0; i < PSU_GROUP_COUNT; i++)
    {
        this->psu_group_enable[i] = PSUGroupPropertyGet(i, PSU_GROUP_ENABLE_SET);
    }
}
/******************************************************************************
 *
 *  Render inner content
 *
 ******************************************************************************/
void
DebugPSUTab::inner_content_render( tContext *pContext )
{
    char                string[36];
    short               sX;
    short               sY;
    unsigned short      button_idx = 0;

    short               index;
    unsigned short      status;



    static const tColumn s_sub_sequences_1[] =
    {
        { Sequence_PSU,         "PSU",            18 },
        { Sequence_Uset,        "Uset",           40 },
        { Sequence_Uout,        "Uout",           45 },
        { Sequence_Iout,        "Iout",           40 },
        { Sequence_Tconv,       "Tconv",          45 },
        { Sequence_Status,      "Status",         50 },
        { Sequence_Errors,      "Errors",         50 },
    };


    static const tColumn s_sub_sequences_2[] =
    {
        { GroupSequence_PSU,    "#",                10 },
        { GroupSequence_Enable, "Enable",           28 },
        { GroupSequence_Uset,   "Uset",             45 },
        { GroupSequence_Uout,   "Uout",             40 },
        { GroupSequence_Imax,   "Imax",             40 },
        { GroupSequence_Iout,   "Iout",             40 },
        { GroupSequence_Tcomp,  "Tcomp",            35 },
        { GroupSequence_Status, "Status",           47 },
    };

    GrContextForegroundSet(pContext, ClrYellow);
    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    sX = pContext->sClipRegion.sXMin;
    sY = pContext->sClipRegion.sYMin + 12;

    usprintf(string, "Power Supply Units (PSU)");
    GrStringDrawCenteredL(pContext, string, -1, sX + 160, sY, 0);

    for(int seq_name = PSU_StartSequence, seq_id = 0; seq_name < PSU_EndSequence; seq_name++, seq_id++)
    {
        sY = pContext->sClipRegion.sYMin + 28;
        sX += s_sub_sequences_1[seq_id].sIndent;

        GrContextForegroundSet(pContext, ClrWheat);
        GrStringDrawCenteredL(pContext, s_sub_sequences_1[seq_id].pcName, -1, sX, sY, 0);

        sY += 5;

        GrContextForegroundSet(pContext, ClrWhite);
        for( int i = 0; i < PSU_COUNT; i++ )
        {
            index = i;
            sY += 12;

            status = PSUPropertyGet(index, PSU_STATUS);

            switch(seq_name)
            {
            case Sequence_PSU:
                usprintf(string, "%d", index + 1);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case Sequence_Uset:
                if( status )
                {
                    PSUPropertyStringGet(index, PSU_SETUP_VOLTAGE, string);
                }
                else
                {
                    usprintf(string, "-");
                }
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case Sequence_Uout:
                if( status )
                {
                    PSUPropertyStringGet(index, PSU_OUTPUT_VOLTAGE, string);
                }
                else
                {
                    usprintf(string, "-");
                }
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case Sequence_Iout:
                if( status )
                {
                    PSUPropertyStringGet(index, PSU_OUTPUT_CURRENT, string);
                }
                else
                {
                    usprintf(string, "-");
                }
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case Sequence_Tconv:
                if( status )
                {
                    PSUPropertyStringGet(index, PSU_TEMPERATURE, string);
                }
                else
                {
                    usprintf(string, "-");
                }
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case Sequence_Status:
                PSUPropertyStringGet(index, PSU_STATUS, string);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case Sequence_Errors:
                PSUPropertyStringGet(index, PSU_COMM_ERROR_COUNTER, string);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            default:
                break;
            }
        }
    }

    sX = pContext->sClipRegion.sXMin;
    sY = pContext->sClipRegion.sYMin + 107;

    GrContextForegroundSet(pContext, ClrYellow);

    usprintf(string, "PSU group parameters:");
    GrStringDrawCenteredL(pContext, string, -1, sX + 160, sY, 0);

    for(int seq_name = PSUGroup_StartSequence, seq_id = 0; seq_name < PSUGroup_EndSequence; seq_name++, seq_id++ )
    {
        sY = pContext->sClipRegion.sYMin + 123;
        sX += s_sub_sequences_2[seq_id].sIndent;

        GrContextForegroundSet(pContext, ClrWheat);
        GrStringDrawCenteredL(pContext, s_sub_sequences_2[seq_id].pcName, -1, sX, sY, 0);

        sY += 5;

        GrContextForegroundSet(pContext, ClrWhite);
        for( int i = 0; i < PSU_GROUP_COUNT; i++ )
        {
            index = i;
            sY += 12;

            switch(seq_name)
            {
            case GroupSequence_PSU:
                usprintf(string, "%d", index + 1);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case GroupSequence_Enable:
                PSUGroupPropertyStringGet(index, PSU_GROUP_ENABLE_SET, string);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);

                if( !this->pButtons[button_idx] )
                {
                    /*
                     * Setup Button
                     */
                    this->pButtons[button_idx] = new RectangularButton(
                                            Uset_ButtonPSUGroupEnable_Startpoint + i,
                                            this, sX-20, sY-7, 36, 12
                                           );
                    this->pButtons[button_idx]->ulStyle = RB_STYLE_OUTLINE;
                    this->pButtons[button_idx]->ulOutlineColor = ClrWhite;
                }
                else
                {
                    if( this->pButtons[button_idx]->ulStyle & RB_STYLE_PRESSED )
                    {
                        this->pButtons[button_idx]->ulOutlineColor = ClrGray;
                    }
                    else
                    {
                        this->pButtons[button_idx]->ulOutlineColor = TEXT_COLOR;
                    }
                }

                button_idx++;

                break;

            case GroupSequence_Uset:
                PSUGroupPropertyStringGet(index, PSU_GROUP_SETUP_VOLTAGE, string);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);

                if( !this->pButtons[button_idx] )
                {
                    /*
                     * Setup Button
                     */
                    this->pButtons[button_idx] = new RectangularButton(
                                            Uset_Button_Startpoint + index,
                                            this, sX-20, sY-7, 36, 12
                                           );
                    this->pButtons[button_idx]->ulStyle = RB_STYLE_OUTLINE;
                    this->pButtons[button_idx]->ulOutlineColor = ClrWhite;
                }
                else
                {
                    if( this->pButtons[button_idx]->ulStyle & RB_STYLE_PRESSED )
                    {
                        this->pButtons[button_idx]->ulOutlineColor = ClrGray;
                    }
                    else
                    {
                        this->pButtons[button_idx]->ulOutlineColor = TEXT_COLOR;
                    }
                }

                button_idx++;

                break;

            case GroupSequence_Uout:
                PSUGroupPropertyStringGet(index, PSU_GROUP_OUTPUT_VOLTAGE, string);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case GroupSequence_Imax:
                PSUGroupPropertyStringGet(index, PSU_GROUP_CURRENT_LIMIT, string);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);

                if( !this->pButtons[button_idx] )
                {
                    /*
                     * Setup Button
                     */
                    this->pButtons[button_idx] = new RectangularButton(
                                                Imax_Button_Startpoint + index,
                                                this, sX-20, sY-7, 36, 12
                                               );

                    this->pButtons[button_idx]->ulStyle = RB_STYLE_OUTLINE;
                    this->pButtons[button_idx]->ulOutlineColor = ClrWhite;
                }
                else
                {
                    if( this->pButtons[button_idx]->ulStyle & RB_STYLE_PRESSED )
                    {
                        this->pButtons[button_idx]->ulOutlineColor = ClrGray;
                    }
                    else
                    {
                        this->pButtons[button_idx]->ulOutlineColor = TEXT_COLOR;
                    }
                }

                button_idx++;

                break;

            case GroupSequence_Iout:
                PSUGroupPropertyStringGet(index, PSU_GROUP_OUTPUT_CURRENT, string);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);
                break;

            case GroupSequence_Tcomp:
                PSUGroupPropertyStringGet(index, PSU_GROUP_TEMPERATURE_COMPENSATION, string);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);

                if( !this->pButtons[button_idx] )
                {
                    /*
                     * Setup Button
                     */
                    this->pButtons[button_idx] = new RectangularButton(
                                                Tcomp_Button_Startpoint + index,
                                                this, sX-20, sY-7, 36, 12
                                               );

                    this->pButtons[button_idx]->ulStyle = RB_STYLE_OUTLINE;
                    this->pButtons[button_idx]->ulOutlineColor = ClrWhite;
                }
                else
                {
                    if( this->pButtons[button_idx]->ulStyle & RB_STYLE_PRESSED )
                    {
                        this->pButtons[button_idx]->ulOutlineColor = ClrGray;
                    }
                    else
                    {
                        this->pButtons[button_idx]->ulOutlineColor = TEXT_COLOR;
                    }
                }

                button_idx++;

                break;

            case GroupSequence_Status:
                PSUGroupPropertyStringGet(index, PSU_GROUP_STATUS, string);
                GrStringDrawCenteredL(pContext, string, -1, sX, sY, 0);

                break;

            default:
                break;
            }
        }
    }
}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugPSUTab::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                        unsigned long ulParam2)
{
    tRectangle    sPosition;
    tContext      sCtx;
    long          lVoltageValue;
    long          lCurrentValue;
    long          lTcompValue;
    short         sGroup;

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

        if( ( ulParam1 >= Uset_ButtonPSUGroupEnable_Startpoint ) &&
            ( ulParam1 <= Uset_ButtonPSUGroupEnable_Endpoint ) )
        {
            sGroup = ulParam1 - Uset_ButtonPSUGroupEnable_Startpoint;
            this->psu_group_enable[ulParam1 - Uset_ButtonPSUGroupEnable_Startpoint] =
                    !this->psu_group_enable[ulParam1 - Uset_ButtonPSUGroupEnable_Startpoint];
            PSUGroupPropertySet(sGroup, PSU_GROUP_ENABLE_SET,
                    this->psu_group_enable[ulParam1 - Uset_ButtonPSUGroupEnable_Startpoint]);
            break;
        }

        if( ( ulParam1 >= Uset_Button_Startpoint ) &&
            ( ulParam1 <= Uset_Button_Endpoint ) )
        {
            sGroup = ulParam1 - Uset_Button_Startpoint;
            lVoltageValue = PSUGroupPropertyGet( sGroup, PSU_GROUP_SETUP_VOLTAGE );
            (new DialogNumericEditor(Voltage_Editor_Startpoint + sGroup, this, NUM_ED_CENTRAL_COORDS))->Init(lVoltageValue, 0);
            break;
        }
        else
        if( ( ulParam1 >= Imax_Button_Startpoint ) &&
            ( ulParam1 <= Imax_Button_Endpoint ) )
        {
            sGroup = ulParam1 - Imax_Button_Startpoint;
            lCurrentValue = PSUGroupPropertyGet( sGroup, PSU_GROUP_CURRENT_LIMIT );
            (new DialogNumericEditor(Current_Editor_Startpoint + sGroup, this, NUM_ED_CENTRAL_COORDS))->Init(lCurrentValue, 0);
            break;
        }
        else
        if( ( ulParam1 >= Tcomp_Button_Startpoint ) &&
            ( ulParam1 <= Tcomp_Button_Endpoint ) )
        {
            sGroup = ulParam1 - Tcomp_Button_Startpoint;
            lTcompValue = PSUGroupPropertyGet( sGroup, PSU_GROUP_TEMPERATURE_COMPENSATION );
            (new DialogNumericEditor(Tcomp_Editor_Startpoint + sGroup, this, NUM_ED_CENTRAL_COORDS))->Init(lTcompValue, 0);
            break;
        }
        break;

    case WIDGET_NOTIFY_APPLY:

        if( ( ulParam1 >= Voltage_Editor_Startpoint ) &&
            ( ulParam1 <= Voltage_Editor_Endpoint) )
        {
            sGroup = ulParam1 - Voltage_Editor_Startpoint;
            PSUGroupPropertySet(sGroup, PSU_GROUP_SETUP_VOLTAGE, ((DialogNumericEditor *)ulParam2)->ValueGet());
            ((DialogNumericEditor *)ulParam2)->Destroy();
        }
        else
        if( ( ulParam1 >= Current_Editor_Startpoint ) &&
            ( ulParam1 <= Current_Editor_Endpoint) )
        {
            sGroup = ulParam1 - Current_Editor_Startpoint;
            PSUGroupPropertySet(sGroup, PSU_GROUP_CURRENT_LIMIT, ((DialogNumericEditor *)ulParam2)->ValueGet());
            ((DialogNumericEditor *)ulParam2)->Destroy();
        }
        else
        if( ( ulParam1 >= Tcomp_Editor_Startpoint ) &&
            ( ulParam1 <= Tcomp_Editor_Endpoint) )
        {
            sGroup = ulParam1 - Tcomp_Editor_Startpoint;
            PSUGroupPropertySet(sGroup, PSU_GROUP_TEMPERATURE_COMPENSATION, ((DialogNumericEditor *)ulParam2)->ValueGet());
            ((DialogNumericEditor *)ulParam2)->Destroy();
        }

        break;

    case WIDGET_NOTIFY_CANCEL:

        if( ( ( ulParam1 >= Voltage_Editor_Startpoint ) &&
              ( ulParam1 <= Voltage_Editor_Endpoint) ) ||
            ( ( ulParam1 >= Current_Editor_Startpoint ) &&
              ( ulParam1 <= Current_Editor_Endpoint) )  ||
            ( ( ulParam1 >= Tcomp_Editor_Startpoint ) &&
              ( ulParam1 <= Tcomp_Editor_Endpoint) ) )
        {
            ((DialogNumericEditor *)ulParam2)->Destroy();
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
DebugPSUTab::~DebugPSUTab(void)
{
    delete[] this->pButtons;

}

