/*
 * ins_mon.cpp
 *
 *  Created on: 16 мая 2019 г.
 *      Author: V.Striukov
 */

#include "interface.h"

#include "property/im_property.h"
#include "dialogs/dialog_numeric_editor.h"
#include "dialogs/window_confirm.h"

#include "debug_ins_mon.h"


/******************************************************************************
 *
 *  Other useful defines
 *
 ******************************************************************************/
#define BUTTON_SIZE                         80, 30
#define TABLE_NAME_TO_COLUMNS_OFFSET        16
#define TABLE_COLUMNS_TO_VALS_OFFEST        3
#define TABLE_INTER_VALS_OFFSET             12
//*****************************************************************************
//
// Column description structure
//
//*****************************************************************************
typedef enum {

    StartInsMonSequence,

    SequenceInsMon_Supply_Voltage = StartInsMonSequence,
    SequenceInsMon_Leak_Voltage_1,
    SequenceInsMon_Leak_Voltage_2,
    SequenceInsMon_Comm_Status,
    SequenceInsMon_Comm_Errors,

    EndInsMonSequence,

    StartImSequence,

    SequenceImRdivResistance = StartImSequence,
    SequenceImLeakResistance_1,
    SequenceImLeakResistance_2,
    SequenceImStatus,

    EndImSequence,

    StartImAlarmSequence,

    SequenceImAlarmResistanceSet = StartImAlarmSequence,
    SequenceImAlarmTimeSet,
    SequenceImAlarmForwardEnableSet,

    EndImAlarmSequence,

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

    Button_Calibrate = Button_Begin,

    Button_End,

    Dialog_Calibration_Rdiv,

    Widget_Max = 0xFF
};


static const int module_button_count = Button_End - Button_Begin;
//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugInsMonTab::DebugInsMonTab(unsigned char ucID, Widget *pParent)
        :HorizontalDebugTabBase(ucID, pParent)
{
    /*
     * Create pointers to the buttons
     */
    this->pButtons = new RectangularButton *[module_button_count];

    /*
     * Clear pointers
     */
    for(int i = 0; i < module_button_count; i++ )
    {
        this->pButtons[i] = 0;
    }

    this->pButtons[0] = new RectangularButton(Button_Calibrate,
                                                  this, 225, 150, BUTTON_SIZE);
    this->pButtons[0]->ulStyle = RB_STYLE_OUTLINE
                                 | RB_STYLE_FALLING_EDGE
                                 | RB_STYLE_TEXT;
    this->pButtons[0]->ulTextColor = ClrWhite;
    this->pButtons[0]->pFont = g_pFontWTahoma12pt;
    this->pButtons[0]->pcText = (char *)"Calibrate";
    this->pButtons[0]->ulOutlineColor = ClrLime;
}
/******************************************************************************
 *
 *  Render inner content
 *
 ******************************************************************************/
unsigned short
DebugInsMonTab::system_im_render( tContext *pContext, unsigned short sX, unsigned short sY )
{
    char            string[36];
    short           sub_sY;

    static const tColumn s_sub_sequences[] =
    {
        { SequenceInsMon_Supply_Voltage,    "Usupply",      50 },
        { SequenceInsMon_Leak_Voltage_1,    "Uleak1",       55 },
        { SequenceInsMon_Leak_Voltage_2,    "Uleak2",       55 },
        { SequenceInsMon_Comm_Status,       "Status",       55 },
        { SequenceInsMon_Comm_Errors,       "Errors",       50 },
    };

    GrContextForegroundSet(pContext, ClrYellow);

    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "Insulation Monitor Module (U712036)");
    GrStringDrawCenteredL(pContext, string, -1, sX + 160, sY, 0);

    for(int seq_name = StartInsMonSequence, seq_id = 0; seq_name < EndInsMonSequence; seq_name++, seq_id++)
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
            case SequenceInsMon_Supply_Voltage:
                im_property_string_get(U712036_SUPPLY_VOLTAGE, string);
                break;

            case SequenceInsMon_Leak_Voltage_1:
                im_property_string_get(U712036_LEAK_VOLTAGE_1, string);
                break;

            case SequenceInsMon_Leak_Voltage_2:
                im_property_string_get(U712036_LEAK_VOLTAGE_2, string);
                break;

            case SequenceInsMon_Comm_Errors:
                im_property_string_get(U712036_COMM_ERRORS, string);
                break;

            case SequenceInsMon_Comm_Status:
                im_property_string_get(U712036_COMM_STATUS, string);
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
DebugInsMonTab::system_im_status_render( tContext *pContext, unsigned short sX, unsigned short sY )
{
    char            string[36];
    short           sub_sY;

    static const tColumn s_sub_sequences[] =
    {
        { SequenceImRdivResistance,             "Rdiv",         20 },
        { SequenceImLeakResistance_1,           "Rleak1",       40 },
        { SequenceImLeakResistance_2,           "Rleak2",       40 },
        { SequenceImStatus,                     "Status",       40 },
    };

    GrContextForegroundSet(pContext, ClrYellow);

    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "Insulation Monitor Status");
    GrStringDrawCenteredL(pContext, string, -1, sX + 80, sY, 0);

    for(int seq_name = StartImSequence, seq_id = 0; seq_name < EndImSequence; seq_name++, seq_id++)
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
            case SequenceImRdivResistance:
                im_property_string_get(IM_RDIV_RESISTANCE, string);
                break;

            case SequenceImLeakResistance_1:
                im_property_string_get(IM_LEAK_RESISTANCE_1, string);
                break;

            case SequenceImLeakResistance_2:
                im_property_string_get(IM_LEAK_RESISTANCE_2, string);
                break;

            case SequenceImStatus:
                im_property_string_get(IM_STATUS, string);
                break;

            default:
                usprintf(string, "");
                break;
            }
            GrStringDrawCenteredL(pContext, string, -1, sX, sub_sY, 0);
        }
    }

    return sX;
}


unsigned short
DebugInsMonTab::system_im_alarm_render( tContext *pContext, unsigned short sX, unsigned short sY )
{
    char            string[36];
    short           sub_sY;


    static const tColumn s_sub_sequences[] =
    {
        { SequenceImAlarmResistanceSet,     "Rleak",      30 },
        { SequenceImAlarmTimeSet,           "Time",       30 },
        { SequenceImAlarmForwardEnableSet,  "Frwd",       30 },
    };

    GrContextForegroundSet(pContext, ClrYellow);

    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "Insulation Monitor Alarm");
    GrStringDrawCenteredL(pContext, string, -1, sX + 60, sY, 0);

    for(int seq_name = StartImAlarmSequence, seq_id = 0; seq_name < EndImAlarmSequence; seq_name++, seq_id++)
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
            case SequenceImAlarmResistanceSet:
                im_property_string_get(IM_ALARM_RESISTANCE_SET, string);
                break;

            case SequenceImAlarmTimeSet:
                im_property_string_get(IM_ALARM_TIME_SET, string);
                break;

            case SequenceImAlarmForwardEnableSet:
                im_property_string_get(IM_ALARM_FORWARD_ENABLE_SET, string);
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
DebugInsMonTab::inner_content_render( tContext *pContext )
{
    unsigned short      sY;
    unsigned short      sX;

    sX = pContext->sClipRegion.sXMin;
    sY = pContext->sClipRegion.sYMin + 12;

    sY = this->system_im_render( pContext, sX, sY );
    sY += 18;

    sX = this->system_im_status_render( pContext, sX, sY );
    sX += 40;

    sY = this->system_im_alarm_render( pContext, sX, sY );

    GrContextFontSet(pContext, g_pFontWTahoma12pt);
    GrContextForegroundSet(pContext, ClrGray);

    sX = 15;

    sY += 12;
    GrStringDraw(pContext, "1. Установить на шине BATT 280.0 V", -1, sX, sY, 0);
    sY += 12;

    GrStringDraw(pContext, "2. Подстроить напряжение* Usupply  до 280.0 V", -1, sX, sY, 0);
    sY += 12;

    GrStringDraw(pContext, "3. Подстроить напряжение* Uleak1  до 260.0 V", -1, sX, sY, 0);
    sY += 12;

    GrStringDraw(pContext, "4. Нажать кнопку Сalibrate, Apply", -1, sX, sY, 0);
    sY += 9;

    GrStringDraw(pContext, "_____________________________", -1, sX, sY, 0);
    sY += 12;
    GrStringDraw(pContext, "*Для платы 712 036", -1, sX, sY, 0);




    if( this->pButtons[0]->ulStyle & RB_STYLE_PRESSED )
    {
        this->pButtons[0]->ulOutlineColor = ClrGray;
    }
    else
    {
        this->pButtons[0]->ulOutlineColor = ClrLime;
    }
}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugInsMonTab::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
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
            case Button_Calibrate:

                (new WindowConfirm(Dialog_Calibration_Rdiv, this, 60, 20))->Init(
                                    false, (char *)"Calibrate Rdiv", (char *)"Are you sure?");
                break;
            }

            break;

        case WIDGET_NOTIFY_APPLY:
            switch( ulParam1 )
            {

            case Dialog_Calibration_Rdiv:
                InterCommDataSet(REMOTE_CTRL_CMD, CMD_FORCE_CALIBRATE_IM_RDIV);
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
DebugInsMonTab::~DebugInsMonTab(void)
{
    for( int i = 0; i < module_button_count; i++)
    {
        delete this->pButtons[i];
    }
    delete[] this->pButtons;
}

