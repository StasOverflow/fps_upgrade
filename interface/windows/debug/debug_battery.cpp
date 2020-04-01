/*
 * debug_battery.cpp
 *
 *  Created on: 13 мая 2019 г.
 *      Author: V.Striukov
 */

#include "interface.h"

#include "property/battery_property.h"
#include "dialogs/dialog_numeric_editor.h"

#include "debug_battery.h"


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
#define RESET_BUTTONS_SIZE                  80, 30
#define TABLE_NAME_TO_COLUMNS_OFFSET        16
#define TABLE_COLUMNS_TO_VALS_OFFEST        3
#define TABLE_INTER_VALS_OFFSET             12
//*****************************************************************************
//
// Column description structure
//
//*****************************************************************************
typedef enum {
    StartSequence,

    Sequence_Voltage = StartSequence,
    Sequence_Current,
    Sequence_Capacity,
    Sequence_Temperature,
    Sequence_Level,
    Sequence_Status,

    EndSequence,


    Batt_Flags_StartSequence,

    Sequence_Flags = Batt_Flags_StartSequence,

    Batt_Flags_EndSequence,


    Batt_PID_StartSequence,

    Sequence_Hashtag = Batt_PID_StartSequence,
    Sequence_Value,
    Sequence_Kp,
    Sequence_Ki,
    Sequence_Kd,
    Sequence_dt,
    Sequence_Err,
    Sequence_EN,

    Batt_PID_EndSequence,


    Batt_Shunt_StartSequence,

    Sequence_Ch = Batt_Shunt_StartSequence,
    Sequence_Ibatt,
    Sequence_Uadc,
    Sequence_Coeff,
    Sequence_Offset,

    Batt_Shunt_EndSequence,

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

    Editor_Pid_Start,

    Editor_Pid_Kp_Voltage = Editor_Pid_Start,
    Editor_Pid_Kp_Current,
    Editor_Pid_Ki_Voltage,
    Editor_Pid_Ki_Current,
    Editor_Pid_Kd_Voltage,
    Editor_Pid_Kd_Current,
    Editor_Pid_dt,

    Editor_Pid_End,

    Editor_Shunt_Start,

    Editor_Shunt_Coeff_Minus,
    Editor_Shunt_Coeff_Plus,
    Editor_Shunt_Offset_Minus,
    Editor_Shunt_Offset_Plus,

    Editor_Shunt_End,

    Button_Pid_Start,

    Button_Pid_Kp_Voltage = Button_Pid_Start,
    Button_Pid_Kp_Current,
    Button_Pid_Ki_Voltage,
    Button_Pid_Ki_Current,
    Button_Pid_Kd_Voltage,
    Button_Pid_Kd_Current,
    Button_Pid_dt,
    Button_Pid_Voltage_Enable,
    Button_Pid_Current_Enable,

    Button_Pid_End,

    Button_Shunt_Start,

    Button_Shunt_Coeff_Minus = Button_Shunt_Start,
    Button_Shunt_Coeff_Plus,
    Button_Shunt_Offset_Minus,
    Button_Shunt_Offset_Plus,

    Button_Shunt_End,

    Widget_Max = 0xFF
};


static const int module_button_count_1 = Button_Pid_End - Button_Pid_Start;
static const int module_button_count_2 = Button_Shunt_End - Button_Shunt_Start;

static const int module_button_count = module_button_count_1 + module_button_count_2;

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugBatteryTab::DebugBatteryTab(unsigned char ucID, Widget *pParent)
        :HorizontalDebugTabBase(ucID, pParent)
{
    this->buttons_created = 0;

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
}
/******************************************************************************
 *
 * Helper Function
 *
 ******************************************************************************/
void
DebugBatteryTab::button_create_or_update_borders( unsigned short index, unsigned short id,
                                                  unsigned short sX, unsigned short sY)
{
    if( !this->pButtons[index] )
    {
        this->pButtons[index] = new RectangularButton(id, this, sX - 20, sY - 7, 36, 12);
        this->pButtons[index]->ulStyle = RB_STYLE_OUTLINE;
        this->pButtons[index]->ulOutlineColor = ClrWhite;

        this->buttons_created++;
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
}
/******************************************************************************
 *
 *  Render inner content
 *
 ******************************************************************************/
unsigned short
DebugBatteryTab::battery_status_render( tContext *pContext, unsigned short sX, unsigned short sY)
{
    char            string[24];
    short           sub_sY;

    char            helper_string_1[16];
    char            helper_string_2[16];

    static const tColumn s_sub_sequences[] =
    {
        { Sequence_Voltage,         "Voltage",        30 },
        { Sequence_Current,         "Current",        45 },
        { Sequence_Capacity,        "Capacity",       55 },
        { Sequence_Temperature,     "Temp",           50 },
        { Sequence_Level,           "Level",          45 },
        { Sequence_Status,          "Status",         55 },
    };

    GrContextForegroundSet(pContext, ClrYellow);

    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "Battery Status");
    GrStringDrawCenteredL(pContext, string, -1, sX + 160, sY, 0);

    for(int seq_name = StartSequence, seq_id = 0; seq_name < EndSequence; seq_name++, seq_id++)
    {
        sub_sY = sY + TABLE_NAME_TO_COLUMNS_OFFSET;
        sX += s_sub_sequences[seq_id].sIndent;

        GrContextForegroundSet(pContext, ClrWheat);
        GrStringDrawCenteredL(pContext, s_sub_sequences[seq_id].pcName, -1, sX, sub_sY, 0);

        sub_sY += TABLE_COLUMNS_TO_VALS_OFFEST;

        GrContextForegroundSet(pContext, ClrWhite);
        for( int i = 0; i < BATTERY_GROUP_COUNT; i++ )
        {
            sub_sY += TABLE_INTER_VALS_OFFSET;

            switch(seq_name)
            {
            case Sequence_Voltage:
                BatteryPropertyStringGet(i, BATTERY_VOLTAGE, string);
                break;

            case Sequence_Current:
                BatteryPropertyStringGet(i, BATTERY_CURRENT, string);
                break;

            case Sequence_Capacity:
                BatteryPropertyStringGet(i, BATTERY_USED_CAPACITY, helper_string_1);
                BatteryPropertyStringGet(i, BATTERY_FULL_CAPACITY, helper_string_2);
                break;

            case Sequence_Status:
                BatteryPropertyStringGet(i, BATTERY_STATUS, string);
                break;

            case Sequence_Temperature:
                BatteryPropertyStringGet(i, BATTERY_TEMPERATURE, string);
                break;

            case Sequence_Level:
                BatteryPropertyStringGet(i, BATTERY_LEVEL, string);
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
DebugBatteryTab::battery_pids_render( tContext *pContext, unsigned short sX, unsigned short sY)
{
    char            string[24];
    short           sub_sY;

    short           offset;
    short           index;


    static const tColumn s_sub_sequences[] =
    {
        { Sequence_Hashtag,      "#",            25 },
        { Sequence_Kp,           "Value",        35 },
        { Sequence_Kp,           "Kp",           35 },
        { Sequence_Ki,           "Ki",           35 },
        { Sequence_Kd,           "Kd",           35 },
        { Sequence_dt,           "dt",           45 },
        { Sequence_Err,          "Err",          35 },
        { Sequence_EN,           "EN",           35 },
    };

    static const char       *pid_string_names[] = {
        "VC",
        "CC",
    };

    const unsigned short pids_quantity = sizeof(pid_string_names) / sizeof(pid_string_names[0]);


    GrContextForegroundSet(pContext, ClrYellow);
    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "Battery PIDs");
    GrStringDrawCenteredL(pContext, string, -1, sX + 160, sY, 0);

    index = 0;

    for(int seq_name = Batt_PID_StartSequence, seq_id = 0; seq_name < Batt_PID_EndSequence; seq_name++, seq_id++)
    {

        sub_sY = sY + TABLE_NAME_TO_COLUMNS_OFFSET;
        sX += s_sub_sequences[seq_id].sIndent;

        GrContextForegroundSet(pContext, ClrWheat);
        GrStringDrawCenteredL(pContext, s_sub_sequences[seq_id].pcName, -1, sX, sub_sY, 0);

        sub_sY += TABLE_COLUMNS_TO_VALS_OFFEST;

        offset = BATTERY_CC_PID_KD - BATTERY_VC_PID_KD;

        GrContextForegroundSet(pContext, ClrWhite);
        for( int i = 0; i < pids_quantity; i++ )
        {
            sub_sY += TABLE_INTER_VALS_OFFSET;

            switch(seq_name)
            {
            case Sequence_Hashtag:
                usprintf(string, pid_string_names[i]);
                break;

            case Sequence_Value:
                switch( i )
                {
                case 0:
                    BatteryPropertyStringGet(0, BATTERY_VOLTAGE_SETUP, string);
                    break;

                case 1:
                    BatteryPropertyStringGet(0, BATTERY_CURRENT_LIMIT, string);
                    break;
                }
                break;

            case Sequence_Kp:
                BatteryPropertyStringGet(0, BATTERY_VC_PID_KP + (i * offset), string);

                this->button_create_or_update_borders(index, Button_Pid_Kp_Voltage + i, sX, sub_sY);
                index++;
                break;

            case Sequence_Ki:
                BatteryPropertyStringGet(0, BATTERY_VC_PID_KI + (i * offset), string);

                this->button_create_or_update_borders(index, Button_Pid_Ki_Voltage + i, sX, sub_sY);
                index++;
                break;

            case Sequence_Kd:
                BatteryPropertyStringGet(0, BATTERY_VC_PID_KD + (i * offset), string);

                this->button_create_or_update_borders(index, Button_Pid_Kd_Voltage + i, sX, sub_sY);
                index++;
                break;

            case Sequence_dt:
                if( i == 0 )
                {
                    BatteryPropertyStringGet(0, BATTERY_PID_DT, string);
                    GrStringDrawCenteredL(pContext, string, -1, sX, sub_sY + 6, 0);
                    this->button_create_or_update_borders(index, Button_Pid_dt, sX, sub_sY + 6);
                    index++;
                    usprintf(string, "");
                }
                else
                {
                    usprintf(string, "");
                }
                break;

            case Sequence_Err:
                BatteryPropertyStringGet(0, BATTERY_VC_PID_ERROR + (i * offset), string);
                break;

            case Sequence_EN:
                BatteryPropertyStringGet(0, BATTERY_VC_ENABLE + (i * offset), string);
                this->button_create_or_update_borders(index, Button_Pid_Voltage_Enable + i, sX, sub_sY);
                index++;
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
DebugBatteryTab::battery_shunt_render( tContext *pContext, unsigned short sX, unsigned short sY )
{
    char            string[24];
    short           sub_sY;

    short           index;


    static const tColumn s_sub_sequences[] =
    {
        { Sequence_Ch,      "Ch",           80 },
        { Sequence_Ibatt,   "Ibatt",        35 },
        { Sequence_Uadc,    "Uadc",         35 },
        { Sequence_Coeff,   "Coeff",        35 },
        { Sequence_Offset,  "Offset",       35 },
    };

    static const char       *shunt_ch_names[] = {
        "-",
        "+",
    };


    GrContextForegroundSet(pContext, ClrYellow);
    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "Battery Shunt");
    GrStringDrawCenteredL(pContext, string, -1, sX + 160, sY, 0);

    index = module_button_count_1;

    for(int seq_name = Batt_Shunt_StartSequence, seq_id = 0; seq_name < Batt_Shunt_EndSequence; seq_name++, seq_id++)
    {
        sub_sY = sY + TABLE_NAME_TO_COLUMNS_OFFSET;
        sX += s_sub_sequences[seq_id].sIndent;

        GrContextForegroundSet(pContext, ClrWheat);
        GrStringDrawCenteredL(pContext, s_sub_sequences[seq_id].pcName, -1, sX, sub_sY, 0);

        sub_sY += TABLE_COLUMNS_TO_VALS_OFFEST;

        GrContextForegroundSet(pContext, ClrWhite);
        for( int i = 0; i < 3; i++ )
        {
            sub_sY += TABLE_INTER_VALS_OFFSET;

            if( i > 1 )
            {
                if( seq_name == Sequence_Ibatt )
                {
                    BatteryPropertyStringGet(0, BATTERY_CURRENT, string);
                    GrStringDrawCenteredL(pContext, string, -1, sX, sub_sY, 0);
                }
            }
            else
            {
                switch(seq_name)
                {
                case Sequence_Ch:
                    usprintf(string, shunt_ch_names[i]);
                    break;

                case Sequence_Ibatt:
                    BatteryPropertyStringGet(0, BATTERY_CURRENT_NEG + i, string);
                    break;

                case Sequence_Uadc:
                    BatteryPropertyStringGet(0, BATTERY_CURRENT_NEG_ADCV + i, string);
                    break;

                case Sequence_Coeff:
                    BatteryPropertyStringGet(0, BATTERY_CURRENT_CALIBRATION_COEFF_NEG + i, string);
                    this->button_create_or_update_borders(index, Button_Shunt_Coeff_Minus + i, sX, sub_sY);
                    index++;
                    break;

                case Sequence_Offset:
                    BatteryPropertyStringGet(0, BATTERY_CURRENT_CALIBRATION_OFFSET_NEG + i, string);
                    this->button_create_or_update_borders(index, Button_Shunt_Offset_Minus + i, sX, sub_sY);
                    index++;
                    break;

                default:
                    usprintf(string, "");
                    break;
                }
                GrStringDrawCenteredL(pContext, string, -1, sX, sub_sY, 0);
            }
        }
    }

    return sY;
}


void
DebugBatteryTab::inner_content_render( tContext *pContext )
{
    short           sX;
    short           sY;


    sX = pContext->sClipRegion.sXMin;
    sY = pContext->sClipRegion.sYMin + 12;

    sY = this->battery_status_render(pContext, sX, sY);
    sY += TABLE_NAME_TO_COLUMNS_OFFSET + 2;

    sY = this->battery_pids_render(pContext, sX, sY);
    sY += TABLE_NAME_TO_COLUMNS_OFFSET + 2;

    sY = this->battery_shunt_render(pContext, sX, sY);
}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugBatteryTab::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                        unsigned long ulParam2)
{
    tRectangle      sPosition;
    tContext        sCtx;

    unsigned short  value;

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
            case Button_Pid_Kp_Voltage:
                value = BatteryPropertyGet(0, BATTERY_VC_PID_KP);
                (new DialogNumericEditor(Editor_Pid_Kp_Voltage, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
                break;

            case Button_Pid_Kp_Current:
                value = BatteryPropertyGet(0, BATTERY_CC_PID_KP);
                (new DialogNumericEditor(Editor_Pid_Kp_Current, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
                break;

            case Button_Pid_Ki_Voltage:
                value = BatteryPropertyGet(0, BATTERY_VC_PID_KI);
                (new DialogNumericEditor(Editor_Pid_Ki_Voltage, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
                break;

            case Button_Pid_Ki_Current:
                value = BatteryPropertyGet(0, BATTERY_CC_PID_KI);
                (new DialogNumericEditor(Editor_Pid_Ki_Current, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
                break;

            case Button_Pid_Kd_Voltage:
                value = BatteryPropertyGet(0, BATTERY_VC_PID_KD);
                (new DialogNumericEditor(Editor_Pid_Kd_Voltage, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
                break;

            case Button_Pid_Kd_Current:
                value = BatteryPropertyGet(0, BATTERY_CC_PID_KD);
                (new DialogNumericEditor(Editor_Pid_Kd_Current, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
                break;

            case Button_Pid_dt:
                value = BatteryPropertyGet(0, BATTERY_PID_DT);
                (new DialogNumericEditor(Editor_Pid_dt, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
                break;

            case Button_Pid_Voltage_Enable:
                value = BatteryPropertyGet(0, BATTERY_VC_ENABLE);
                value ^= 1;
                BatteryPropertySet(0, BATTERY_VC_ENABLE, value);
                break;

            case Button_Pid_Current_Enable:
                value = BatteryPropertyGet(0, BATTERY_CC_ENABLE);
                value ^= 1;
                BatteryPropertySet(0, BATTERY_CC_ENABLE, value);
                break;

            case Button_Shunt_Coeff_Minus:
                value = BatteryPropertyGet(0, BATTERY_CURRENT_CALIBRATION_COEFF_NEG);
                (new DialogNumericEditor(Editor_Shunt_Coeff_Minus, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
                break;

            case Button_Shunt_Coeff_Plus:
                value = BatteryPropertyGet(0, BATTERY_CURRENT_CALIBRATION_COEFF_POS);
                (new DialogNumericEditor(Editor_Shunt_Coeff_Plus, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
                break;

            case Button_Shunt_Offset_Minus:
                value = BatteryPropertyGet(0, BATTERY_CURRENT_CALIBRATION_OFFSET_NEG);
                (new DialogNumericEditor(Editor_Shunt_Offset_Minus, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
                break;

            case Button_Shunt_Offset_Plus:
                value = BatteryPropertyGet(0, BATTERY_CURRENT_CALIBRATION_OFFSET_POS);
                (new DialogNumericEditor(Editor_Shunt_Offset_Plus, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);
                break;
            }

            break;

        case WIDGET_NOTIFY_APPLY:
            switch( ulParam1 )
            {
            case Editor_Pid_Kp_Voltage:
                BatteryPropertySet(0, BATTERY_VC_PID_KP, ((DialogNumericEditor *)ulParam2)->ValueGet());
                ((DialogNumericEditor *)ulParam2)->Destroy();
                break;

            case Editor_Pid_Kp_Current:
                BatteryPropertySet(0, BATTERY_CC_PID_KP, ((DialogNumericEditor *)ulParam2)->ValueGet());
                ((DialogNumericEditor *)ulParam2)->Destroy();
                break;

            case Editor_Pid_Ki_Voltage:
                BatteryPropertySet(0, BATTERY_VC_PID_KI, ((DialogNumericEditor *)ulParam2)->ValueGet());
                ((DialogNumericEditor *)ulParam2)->Destroy();
                break;

            case Editor_Pid_Ki_Current:
                BatteryPropertySet(0, BATTERY_CC_PID_KI, ((DialogNumericEditor *)ulParam2)->ValueGet());
                ((DialogNumericEditor *)ulParam2)->Destroy();
                break;

            case Editor_Pid_Kd_Voltage:
                BatteryPropertySet(0, BATTERY_VC_PID_KD, ((DialogNumericEditor *)ulParam2)->ValueGet());
                ((DialogNumericEditor *)ulParam2)->Destroy();
                break;

            case Editor_Pid_Kd_Current:
                BatteryPropertySet(0, BATTERY_CC_PID_KD, ((DialogNumericEditor *)ulParam2)->ValueGet());
                ((DialogNumericEditor *)ulParam2)->Destroy();
                break;

            case Editor_Pid_dt:
                BatteryPropertySet(0, BATTERY_PID_DT, ((DialogNumericEditor *)ulParam2)->ValueGet());
                ((DialogNumericEditor *)ulParam2)->Destroy();
                break;

            case Editor_Shunt_Coeff_Minus:
                BatteryPropertySet(0, BATTERY_CURRENT_CALIBRATION_COEFF_NEG, ((DialogNumericEditor *)ulParam2)->ValueGet());
                ((DialogNumericEditor *)ulParam2)->Destroy();
                break;

            case Editor_Shunt_Coeff_Plus:
                BatteryPropertySet(0, BATTERY_CURRENT_CALIBRATION_COEFF_POS, ((DialogNumericEditor *)ulParam2)->ValueGet());
                ((DialogNumericEditor *)ulParam2)->Destroy();
                break;

            case Editor_Shunt_Offset_Minus:
                BatteryPropertySet(0, BATTERY_CURRENT_CALIBRATION_OFFSET_NEG, ((DialogNumericEditor *)ulParam2)->ValueGet());
                ((DialogNumericEditor *)ulParam2)->Destroy();
                break;

            case Editor_Shunt_Offset_Plus:
                BatteryPropertySet(0, BATTERY_CURRENT_CALIBRATION_OFFSET_POS, ((DialogNumericEditor *)ulParam2)->ValueGet());
                ((DialogNumericEditor *)ulParam2)->Destroy();
                break;
            }

            break;

        case WIDGET_NOTIFY_CANCEL:
            switch( ulParam1 )
            {
            case Editor_Pid_Kp_Voltage:
            case Editor_Pid_Kp_Current:
            case Editor_Pid_Ki_Voltage:
            case Editor_Pid_Ki_Current:
            case Editor_Pid_Kd_Voltage:
            case Editor_Pid_Kd_Current:
            case Editor_Pid_dt:
            case Editor_Shunt_Coeff_Minus:
            case Editor_Shunt_Coeff_Plus:
            case Editor_Shunt_Offset_Minus:
            case Editor_Shunt_Offset_Plus:
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
DebugBatteryTab::~DebugBatteryTab(void)
{
    for( int i = 0; i < this->buttons_created; i++)
    {
        delete this->pButtons[i];
    }
    delete[] this->pButtons;
}

