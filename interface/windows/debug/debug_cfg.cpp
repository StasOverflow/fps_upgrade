/*
 * debug_cfg.cpp
 *
 *  Created on: 11 мая 2019 г.
 *      Author: V.Striukov
 */


#include "../debug/debug_cfg.h"

#include "interface.h"
#include "dialogs/dialog_numeric_editor.h"
#include "dialogs/dialog_adjust_double.h"

#include "dialogs/window_confirm.h"

#include "property/ups_property.h"
#include "property/input_property.h"

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

    Sequence_Model = StartSequence,
    Sequence_Serial,

    EndSequence,

    StartConnectionSequence,

    Sequence_Sends = StartConnectionSequence,
    Sequence_Errors,

    EndConnectionSequence,

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

    Button_Adjustments_Reset = Button_Begin,
    Button_Calibrations_Reset,
    Button_Factory_Reset,

    Button_Model,
    Button_Serial_Letters,
    Button_Serial_Number,

    Button_End,

    Dialog_Adjustments_Reset,
    Dialog_Calibrations_Reset,
    Dialog_Factory_Reset,

    Editor_Model,
    Adjustor_Serial_Letters,
    Editor_Serial_Number,

    Widget_Max = 0xFF
};


static const int window_button_count = Button_End - Button_Begin;

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugCfgTab::DebugCfgTab(unsigned char ucID, Widget *pParent)
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

    /*
     * Button Adjustments reset
     */
    this->pButtons[0] = new RectangularButton(Button_Adjustments_Reset,
                                                  this, 10, 150, RESET_BUTTONS_SIZE);
    this->pButtons[0]->ulStyle = RB_STYLE_OUTLINE
                                 | RB_STYLE_FALLING_EDGE
                                 | RB_STYLE_TEXT;
    this->pButtons[0]->ulTextColor = TEXT_COLOR;
    this->pButtons[0]->pFont = g_pFontWTahoma12pt;
    this->pButtons[0]->pcText = (char *)"Adjustments";

    /*
     * Button Calibrations reset
     */
    this->pButtons[1] = new RectangularButton(Button_Calibrations_Reset,
                                                  this, 120, 150, RESET_BUTTONS_SIZE);
    this->pButtons[1]->ulStyle = RB_STYLE_OUTLINE
                                 | RB_STYLE_FALLING_EDGE
                                 | RB_STYLE_TEXT;
    this->pButtons[1]->ulTextColor = TEXT_COLOR;
    this->pButtons[1]->pFont = g_pFontWTahoma12pt;
    this->pButtons[1]->pcText = (char *)"Calibrations";

    /*
     * Button Factory reset
     */
    this->pButtons[2] = new RectangularButton(Button_Factory_Reset,
                                                  this, 230, 150, RESET_BUTTONS_SIZE);
    this->pButtons[2]->ulStyle = RB_STYLE_OUTLINE
                                 | RB_STYLE_FALLING_EDGE
                                 | RB_STYLE_TEXT;
    this->pButtons[2]->ulTextColor = TEXT_COLOR;
    this->pButtons[2]->pFont = g_pFontWTahoma12pt;
    this->pButtons[2]->pcText = (char *)"Factory";
}
/******************************************************************************
 *
 *  Render inner content
 *
 ******************************************************************************/
unsigned short
DebugCfgTab::system_connection_errors_render( tContext *pContext, unsigned short sX, unsigned short sY )
{
    char            string[36];
    short           sub_sX;

    static const tColumn s_sub_sequences[] =
    {
        { Sequence_Sends,         "Sends",          20 },
        { Sequence_Errors,        "Errors",         17 },
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
            case Sequence_Sends:
                usprintf(string, "%d", inter_comm_conn_instances_get());
                break;

            case Sequence_Errors:
                usprintf(string, "%d", inter_comm_conn_errs_get());
                break;

            default:
                usprintf(string, "");
                break;
            }
            GrStringDrawCenteredL(pContext, string, -1, sub_sX, sY, 0);
        }
    }

    return sY;
}


unsigned short
DebugCfgTab::system_model_serial_render( tContext *pContext, unsigned short sX, unsigned short sY )
{
    char            string[16];
    short           sub_sX;


    static const tColumn s_sub_sequences[] =
    {
        { Sequence_Model,         "Serial",       20 },
        { Sequence_Serial,        "Model",        17 },
    };

    GrContextForegroundSet(pContext, ClrYellow);

    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    usprintf(string, "System Info");
    GrStringDrawCenteredL(pContext, string, -1, sX + 50, sY, 0);

    for(int seq_name = StartSequence, seq_id = 0; seq_name < EndSequence; seq_name++, seq_id++)
    {
        sub_sX = sX + 12;
        sY += s_sub_sequences[seq_id].sIndent;

        GrContextForegroundSet(pContext, ClrWheat);
        GrStringDrawCenteredL(pContext, s_sub_sequences[seq_id].pcName, -1, sub_sX, sY, 0);

        sub_sX += 4;

        GrContextForegroundSet(pContext, ClrWhite);
        for( int i = 0; i < 1; i++ )
        {
            sub_sX += 70;

            switch(seq_name)
            {
            case Sequence_Model:
                strcpy(string, "U   ");
                UPSPropertyStringGet(SYSTEM_MODEL_MAJOR, string + strlen(string));
                strcat(string, ".");
                UPSPropertyStringGet(SYSTEM_MODEL_MINOR, string + strlen(string));

                if( !this->pButtons[3] )
                {
                    this->pButtons[3] = new RectangularButton(Button_Model, this, sub_sX - 14, sY - 9, 40, 16);
                    this->pButtons[3]->ulStyle = RB_STYLE_OUTLINE;
                    this->pButtons[3]->ulOutlineColor = ClrWhite;
                }
                else
                {
                    if( this->pButtons[3]->ulStyle & RB_STYLE_PRESSED )
                    {
                        this->pButtons[3]->ulOutlineColor = ClrGray;
                    }
                    else
                    {
                        this->pButtons[3]->ulOutlineColor = TEXT_COLOR;
                    }
                }
                break;

            case Sequence_Serial:
                UPSPropertyStringGet(SYSTEM_SERIAL_NUMBER_0, string);

                strcat(string, "   ");

                UPSPropertyStringGet(SYSTEM_SERIAL_NUMBER_1, string + strlen(string));
                UPSPropertyStringGet(SYSTEM_SERIAL_NUMBER_2, string + strlen(string));

                if( !this->pButtons[4] )
                {
                    this->pButtons[4] = new RectangularButton(Button_Serial_Letters,
                                                              this, sub_sX - 29, sY - 9, 21, 16);
                    this->pButtons[4]->ulStyle = RB_STYLE_OUTLINE;
                    this->pButtons[4]->ulOutlineColor = ClrWhite;
                }
                else
                {
                    if( this->pButtons[4]->ulStyle & RB_STYLE_PRESSED )
                    {
                        this->pButtons[4]->ulOutlineColor = ClrGray;
                    }
                    else
                    {
                        this->pButtons[4]->ulOutlineColor = TEXT_COLOR;
                    }
                }

                if( !this->pButtons[5] )
                {
                    this->pButtons[5] = new RectangularButton(Button_Serial_Number,
                                                              this, sub_sX - 8, sY - 9, 34, 16);
                    this->pButtons[5]->ulStyle = RB_STYLE_OUTLINE;
                    this->pButtons[5]->ulOutlineColor = ClrWhite;
                }
                else
                {
                    if( this->pButtons[5]->ulStyle & RB_STYLE_PRESSED )
                    {
                        this->pButtons[5]->ulOutlineColor = ClrGray;
                    }
                    else
                    {
                        this->pButtons[5]->ulOutlineColor = TEXT_COLOR;
                    }
                }

                break;

            default:
                usprintf(string, "");
                break;
            }
            GrStringDrawCenteredL(pContext, string, -1, sub_sX, sY, 0);
        }
    }

    return sY;
}


void
DebugCfgTab::inner_content_render( tContext *pContext )
{
    unsigned short sY;


    sY = 20;
    this->system_model_serial_render( pContext, 180, sY );

    this->system_connection_errors_render( pContext, 30, sY );
}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugCfgTab::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                        unsigned long ulParam2)
{
    tRectangle          sPosition;
    tContext            sCtx;
    long                value;
    char                string[24];


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

            for( uint8_t i = 0; i < window_button_count; i++ )
            {
                if( i < 3 )
                {
                    if( this->pButtons[i]->ulStyle & RB_STYLE_PRESSED )
                    {
                        this->pButtons[i]->ulOutlineColor = ClrGray;
                    }
                    else
                    {
                        if( i == (Button_Adjustments_Reset - Button_Begin ) )
                        {
                            this->pButtons[i]->ulOutlineColor = 0x00DDDD00;
                        }
                        else
                        {
                            this->pButtons[i]->ulOutlineColor = ClrRed;
                        }
                    }
                }
            }
            break;

        default:
            return 0;
        }

        break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
        switch( ulParam1 )
        {
        case Button_Adjustments_Reset:

            (new WindowConfirm(Dialog_Adjustments_Reset, this, 60, 20))->Init(
                                false, (char *)"Reset Adjustments", (char *)"Are you sure?");

            break;

        case Button_Calibrations_Reset:

            (new WindowConfirm(Dialog_Calibrations_Reset, this, 60, 20))->Init(
                                true, (char *)"Reset Calibrations", (char *)"Are you sure?");

            break;

        case Button_Factory_Reset:

            (new WindowConfirm(Dialog_Factory_Reset, this, 60, 20))->Init(
                                true, (char *)"Perform a factory reset", (char *)"Are you sure?");

            break;

        case Button_Model:
            value  = UPSPropertyGet(SYSTEM_MODEL_MAJOR) * 100;
            value += UPSPropertyGet(SYSTEM_MODEL_MINOR);
            (new DialogNumericEditor(Editor_Model, this, 111, 45))->Init(value, 2);

            break;

        case Button_Serial_Letters:
            value = UPSPropertyGet(SYSTEM_SERIAL_NUMBER_0);

            if( value == 0 )
            {
                value = ('Q' << 8) | 'A';
            }
            new DialogAdjustDouble(Adjustor_Serial_Letters, this, 111, 45, value, Short, Alphabetical);

            break;

        case Button_Serial_Number:
            string[0] = UPSPropertyGet(SYSTEM_SERIAL_NUMBER_1) >> 8;
            string[1] = UPSPropertyGet(SYSTEM_SERIAL_NUMBER_1) & 0xFF;
            string[2] = UPSPropertyGet(SYSTEM_SERIAL_NUMBER_2) >> 8;
            string[3] = UPSPropertyGet(SYSTEM_SERIAL_NUMBER_2) & 0xFF;
            string[4] = '\0';

            value = ustrtoul(string, 0, 10);

            (new DialogNumericEditor(Editor_Serial_Number, this, NUM_ED_CENTRAL_COORDS))->Init(value, 0);

            break;

        default:
            break;
        }

        break;
    case WIDGET_NOTIFY_APPLY:
        switch( ulParam1 )
        {

        case Editor_Model:
            value = ((DialogNumericEditor *)ulParam2)->ValueGet();

            UPSPropertySet(SYSTEM_MODEL_MAJOR, value / 100);
            UPSPropertySet(SYSTEM_MODEL_MINOR, value % 100);

            ((DialogNumericEditor *)ulParam2)->Destroy();

            break;

        case Adjustor_Serial_Letters:
            value = ((DialogAdjustDouble *)ulParam2)->ValueGet(Whole);

            UPSPropertySet(SYSTEM_SERIAL_NUMBER_0, value);

            ((DialogAdjustDouble *)ulParam2)->Destroy();

            break;

        case Editor_Serial_Number:
            value = ((DialogNumericEditor *)ulParam2)->ValueGet();

            usprintf(string, "%04d", value);

            value  = (unsigned long)string[0] << 8;
            value |= (unsigned long)string[1] << 0;

            UPSPropertySet(SYSTEM_SERIAL_NUMBER_1, value);

            value  = (unsigned long)string[2] << 8;
            value |= (unsigned long)string[3] << 0;

            UPSPropertySet(SYSTEM_SERIAL_NUMBER_2, value);

            ((DialogNumericEditor *)ulParam2)->Destroy();

            break;

        case Dialog_Adjustments_Reset:
            //add adjustments reset here


            ((WindowConfirm *)ulParam2)->Destroy();
            break;

        case Dialog_Calibrations_Reset:
            //add calibrations reset here


            ((WindowConfirm *)ulParam2)->Destroy();
            break;

        case Dialog_Factory_Reset:
            InterCommDataSet(REMOTE_CTRL_CMD, CMD_FACTORY_RESET);
            ((WindowConfirm *)ulParam2)->Destroy();
            break;

        default:
            break;
        }

        break;

    case WIDGET_NOTIFY_CANCEL:
        switch( ulParam1 )
        {
        case Editor_Model:
        case Editor_Serial_Number:
            ((DialogNumericEditor *)ulParam2)->Destroy();
            break;

        case Adjustor_Serial_Letters:
            ((DialogAdjustDouble *)ulParam2)->Destroy();
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
DebugCfgTab::~DebugCfgTab(void)
{
    delete[] this->pButtons;
}

