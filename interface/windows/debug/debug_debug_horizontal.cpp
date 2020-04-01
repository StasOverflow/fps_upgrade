/*
 * debug_debug_horizontal.cpp
 *
 *  Created on: 22 нояб. 2018 г.
 *      Author: V.Striukov
 */
#include "interface.h"

#include "dialogs/dialog_numeric_editor.h"

#include "property/debug_property.h"
#include "property/battery_property.h"

#include "protocol_driver.h"

#include "ustdlib.h"

#include "debug_debug_horizontal.h"



#define STRUCT_ARROW_POS_X                  65
#define STRUCT_ARROW_POS_Y                  150
#define STRUCT_ARROW_BUTTON_SIDE            21
#define STRUCT_ARROW_BUTTON_SIZE            STRUCT_ARROW_BUTTON_SIDE, \
                                            STRUCT_ARROW_BUTTON_SIDE


#define DATA_INDENT                         100
#define FIELD_INDENT_SIZE                   8

#define ARROW_BUTTON_SIDE                   25
#define ARROW_BUTTON_SIZE                   ARROW_BUTTON_SIDE, ARROW_BUTTON_SIDE
#define ARROWS_POSITION_X                   225
#define ARROWS_POSITION_Y                   160

#define field_indent(x)                     ( x * FIELD_INDENT_SIZE )

#define ROW_COUNT                           4
#define COL_COUNT                           8


#pragma pack( push, 2)
struct Input {
    unsigned char        status;
    unsigned char        feeder_count;
    struct Input_Feeder {
        unsigned char   status;
        unsigned char   phase_count;
        struct Input_Phase {
            unsigned long  voltage;
            unsigned long  current;
            unsigned long  power;
        } phase[1];
    } feeder[1];
} input;
#pragma pack(pop)


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

    Button_First,
    Button_Scroll_Down = Button_First,
    Button_Scroll_Up,
    Button_Next_Page,
    Button_Prev_Page,

    Button_Struct_Feeder_Next,
    Button_Struct_Feeder_Prev,
    Button_Struct_Phase_Next,
    Button_Struct_Phase_Prev,
    Button_Last,

    Widget_Max = 0xFF
};


static const unsigned char ucButtonCount = Button_Last - Button_First;


static short            cFeederCountTotal;
static short            cPhaseCountTotal[3];

static short            cFeederCountCurrent;
static short            cPhaseCountCurrent[3];
//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugDebugHorizontal::DebugDebugHorizontal(unsigned char ucID, Widget *pParent)
               :Widget(ucID, pParent, 0, 0, 320, 240)
{
    Canvas              *pCanvas;
    unsigned short       btn = 0;

    this->sRowMin = 0;

    //
    // Create pointers to the buttons
    //
    this->pButtons = new RectangularButton *[ucButtonCount];

    //
    // Clear pionters
    //
    for(int i = 0; i < ucButtonCount; i++ )
    {
        this->pButtons[i] = 0;
    }

//    //
//    // Border canvas
//    //
//    pCanvas = new Canvas(Canvas_Border, this, DEBUG_TAB_BORDER_POS);
//    pCanvas->ulStyle = CANVAS_STYLE_RISING_EDGE;
//    pCanvas->ulFillColor = BORDER_COLOR;

    //
    // Main canvas
    //
    pCanvas = new Canvas(Canvas_Main, this, DEBUG_TAB_MAIN_POS);
    pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;

    this->pButtons[btn] = new RectangularButton(Button_Scroll_Up, this,
                                                ARROWS_POSITION_X
                                                + ( ARROW_BUTTON_SIDE * 1 ),
                                                ARROWS_POSITION_Y
                                                - ( ARROW_BUTTON_SIDE * 1),
                                                ARROW_BUTTON_SIZE);
    this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE
                                 | RB_STYLE_FILL
                                 | RB_STYLE_TEXT
                                 | RB_STYLE_AUTO_REPEAT;
    this->pButtons[btn]->usAutoRepeatDelay = 500;
    this->pButtons[btn]->usAutoRepeatRate = 100;
    this->pButtons[btn]->ulOutlineColor = ClrWhite;
    this->pButtons[btn]->pFont  = g_pFontWTahoma12pt;
    this->pButtons[btn]->ulTextColor = ClrWhite;
    this->pButtons[btn]->ulPressFillColor = ClrGray;
    this->pButtons[btn]->pcText = (char *)" ^";

    this->pButtons[++btn] = new RectangularButton(Button_Prev_Page, this,
                                                  ARROWS_POSITION_X
                                                  + ( ARROW_BUTTON_SIDE * 0 ),
                                                  ARROWS_POSITION_Y,
                                                  ARROW_BUTTON_SIZE);
    this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE
                                 | RB_STYLE_FILL
                                 | RB_STYLE_TEXT
                                 | RB_STYLE_AUTO_REPEAT;
    this->pButtons[btn]->usAutoRepeatDelay = 500;
    this->pButtons[btn]->usAutoRepeatRate = 100;
    this->pButtons[btn]->ulOutlineColor = ClrWhite;
    this->pButtons[btn]->pFont  = g_pFontWTahoma12pt;
    this->pButtons[btn]->ulTextColor = ClrWhite;
    this->pButtons[btn]->ulPressFillColor = ClrGray;
    this->pButtons[btn]->pcText = (char *)"<";

    this->pButtons[++btn] = new RectangularButton(Button_Scroll_Down, this,
                                                  ARROWS_POSITION_X
                                                  + ( ARROW_BUTTON_SIDE * 1 ),
                                                  ARROWS_POSITION_Y,
                                                  ARROW_BUTTON_SIZE);
    this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE
                                 | RB_STYLE_FILL
                                 | RB_STYLE_TEXT
                                 | RB_STYLE_AUTO_REPEAT;
    this->pButtons[btn]->usAutoRepeatDelay = 500;
    this->pButtons[btn]->usAutoRepeatRate = 100;
    this->pButtons[btn]->ulOutlineColor = ClrWhite;
    this->pButtons[btn]->pFont  = g_pFontWTahoma12pt;
    this->pButtons[btn]->ulTextColor = ClrWhite;
    this->pButtons[btn]->ulPressFillColor = ClrGray;
    this->pButtons[btn]->pcText = (char *)" v";

    this->pButtons[++btn] = new RectangularButton(Button_Next_Page, this,
                                                  ARROWS_POSITION_X
                                                  + ( ARROW_BUTTON_SIDE * 2 ),
                                                  ARROWS_POSITION_Y,
                                                  ARROW_BUTTON_SIZE);
    this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE
                                 | RB_STYLE_FILL
                                 | RB_STYLE_TEXT
                                 | RB_STYLE_AUTO_REPEAT;
    this->pButtons[btn]->usAutoRepeatDelay = 500;
    this->pButtons[btn]->usAutoRepeatRate = 100;
    this->pButtons[btn]->ulOutlineColor = ClrWhite;
    this->pButtons[btn]->pFont  = g_pFontWTahoma12pt;
    this->pButtons[btn]->ulTextColor = ClrWhite;
    this->pButtons[btn]->ulPressFillColor = ClrGray;
    this->pButtons[btn]->pcText = (char *)">";

    for( int i = 0; i < 4; i++ )
    {
        unsigned char val = i % 2;
        this->pButtons[++btn] = new RectangularButton(Button_Struct_Feeder_Next + i,
                                                      this,
                                                      STRUCT_ARROW_POS_X
                                                      + ( STRUCT_ARROW_BUTTON_SIDE
                                                      * val ),
                                                      STRUCT_ARROW_POS_Y
                                                      + ( STRUCT_ARROW_BUTTON_SIDE
                                                      * ( i / 2 ) ),
                                                      STRUCT_ARROW_BUTTON_SIZE);
        this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE
                                     | RB_STYLE_FILL
                                     | RB_STYLE_TEXT
                                     | RB_STYLE_AUTO_REPEAT;
        this->pButtons[btn]->usAutoRepeatDelay = 500;
        this->pButtons[btn]->usAutoRepeatRate = 100;
        this->pButtons[btn]->ulOutlineColor = ClrWhite;
        this->pButtons[btn]->pFont  = g_pFontWTahoma12pt;
        this->pButtons[btn]->ulTextColor = ClrWhite;
        this->pButtons[btn]->ulPressFillColor = ClrGray;
        this->pButtons[btn]->pcText = (val ? (char *)">" : (char *)"<" ) ;
    }

}

//*****************************************************************************
//
// Draw ACQMB debug
//
//****************************************************************************
void
DebugDebugHorizontal::StructRender(tContext *pContext)
{
    char                    pcString[17];
    static unsigned char    rawData[255];
    long                    lY;

    unsigned char           ucMinRow;
    unsigned char           ucField = 0;
    static char             field[15][10];

    short                   sPhaseCell;
    short                   sFeederCell;

    ucMinRow = this->sRowMin;

    static const tColumn sColumns[] =
    {
        { "struct",         field_indent(1)},
        { "{",              field_indent(1)},
        { "status",         field_indent(2)},
        { "feeder",         field_indent(2)},
        { "{",              field_indent(2)},
        { "status",         field_indent(3)},
        { "phase",          field_indent(3)},
        { "{",              field_indent(3)},
        { "voltage",        field_indent(4)},
        { "current",        field_indent(4)},
        { "power",          field_indent(4)},
        { "}",              field_indent(3)},
        { "}",              field_indent(2)},
        { "}",              field_indent(1)},
    };

    const unsigned short usColumnsNum = sizeof(sColumns) / sizeof(sColumns[0]);


//    for( int i = 0; i < usColumnsNum; i++ )
//    {
//        if( !ustrnicmp("{", sColumns[i].pcName, 2)
//         || !ustrnicmp("}", sColumns[i].pcName, 2) )
//        {
//            ucField++;
//        }
//    }

    //*****************************************************************
    //
    // Titles
    //
    //*****************************************************************

    GrContextFontSet(pContext, g_pFontWTahoma12pt);
    GrContextForegroundSet(pContext, ClrYellow);

    lY  = 4;

    for( int i = 0; i < usColumnsNum; i++ )
    {
        GrStringDraw(pContext, sColumns[i].pcName, -1, pContext->sClipRegion.sXMin + sColumns[i].sIndent,
                                                       pContext->sClipRegion.sYMin + lY, 0);
        lY += 10;
    }


    memcpy(&rawData, protocol_driver_message_data_get(), 255);

    unsigned short position;

    position = 1;

    cFeederCountTotal = rawData[position];
    for( int i = 0; i < cFeederCountTotal; i++ )
    {
        cPhaseCountTotal[i] = rawData[position + 2];
    }

    GrContextForegroundSet(pContext, ClrLime);

    usprintf(pcString, "package raw data");

    GrStringDrawCenteredL(pContext, pcString, -1,
                          pContext->sClipRegion.sXMin + 225,
                          pContext->sClipRegion.sYMin + 10, 0);



    for( int i = 0; i < ROW_COUNT; i++ )
    {
        ucMinRow = sRowMin + i;

        usprintf(pcString, "%02x:", ucMinRow);

        GrContextForegroundSet(pContext, ClrLime);

        GrStringDrawCenteredL(pContext, pcString, -1,
                              pContext->sClipRegion.sXMin + 160,
                              pContext->sClipRegion.sYMin + 30 + i * 20, 0);

        for( int j = 0; j < COL_COUNT; j++ )
        {
            GrContextForegroundSet(pContext, ClrWhiteSmoke);

            usprintf(pcString, "%u", rawData[j + (ucMinRow * 8)]);

            GrStringDrawCenteredL(pContext, pcString, -1,
                                  pContext->sClipRegion.sXMin + 180 + j * 17,
                                  pContext->sClipRegion.sYMin + 30 + i * 20, 0);
        }

    }

    GrContextForegroundSet(pContext, ClrLime);

    lY = 140;

    usprintf(pcString, "payld:");
    GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + 155,
                                         pContext->sClipRegion.sYMin + 110, 0);

    usprintf(pcString, "total:");
    GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + 155,
                                         pContext->sClipRegion.sYMin + 130, 0);

    GrContextForegroundSet(pContext, ClrWhiteSmoke);

    usprintf(pcString,  "%d", protocol_driver_payload_len_get());
    GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + 190,
                                         pContext->sClipRegion.sYMin + 110, 0);

    usprintf(pcString, "%d", protocol_driver_message_len_get());
    GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + 190,
                                         pContext->sClipRegion.sYMin + 130, 0);


    lY  = 4;

    memcpy(&input, protocol_driver_message_data_get(), sizeof(input));


    if( cFeederCountCurrent > cFeederCountTotal - 1)
    {
        cFeederCountCurrent = cFeederCountTotal - 1;
    }
    if( cFeederCountCurrent < 0)
    {
        cFeederCountCurrent = 0;
    }
    if( cPhaseCountCurrent[cFeederCountCurrent] > cPhaseCountTotal[cFeederCountCurrent] - 1)
    {
        cPhaseCountCurrent[cFeederCountCurrent] = cPhaseCountTotal[cFeederCountCurrent] - 1;
    }
    if( cPhaseCountCurrent[cFeederCountCurrent] < 0)
    {
        cPhaseCountCurrent[cFeederCountCurrent] = 0;
    }

    ucField = 0;

    sFeederCell = cFeederCountCurrent;
    sPhaseCell = cPhaseCountCurrent[sFeederCell];

    usprintf(field[ucField++], " %d ", (sizeof(input)));
    usprintf(field[ucField++], " %d ", input.status);
    if( cFeederCountTotal )
    {
        usprintf(field[ucField++], "[%d]/%d", sFeederCell + 1,
                                   input.feeder_count);
        usprintf(field[ucField++], " %d ", input.feeder[sFeederCell].status);
        usprintf(field[ucField++], "[%d]/%d", sPhaseCell + 1,
                                   input.feeder[sFeederCell].phase_count);
    }
    else
    {
        usprintf(field[ucField++], "[0]/0");
        usprintf(field[ucField++], " %d ", input.feeder[sFeederCell].status);
        usprintf(field[ucField++], "[0]/0");
    }
    usprintf(field[ucField++], " %d ", input.feeder[sFeederCell].phase[sPhaseCell].voltage);
    usprintf(field[ucField++], " %d ", input.feeder[sFeederCell].phase[sPhaseCell].current);
    usprintf(field[ucField++], " %d ", input.feeder[sFeederCell].phase[sPhaseCell].power);

    ucField = 0;

    for( int i = 0; i < usColumnsNum; i++ )
    {
        if( ustrnicmp("{", sColumns[i].pcName, 2)
         && ustrnicmp("}", sColumns[i].pcName, 2) )
        {
            GrStringDraw(pContext, field[ucField++], -1,
                                   pContext->sClipRegion.sXMin + DATA_INDENT,
                                   pContext->sClipRegion.sYMin + lY, 0);
        }
        lY += 10;
    }
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugDebugHorizontal::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
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

        switch( ulParam1 )
        {
        case Canvas_Main:
            this->StructRender(&sCtx);
            break;

        default:
            return 0;
        }

        break;

    case WIDGET_NOTIFY_BTN_PRESSED:
        switch( ulParam1 )
        {
        case Button_Scroll_Up:
            this->sRowMin--;
            if( this->sRowMin < 0)
            {
                this->sRowMin = 0;
            }
            break;

        case Button_Next_Page:
            this->sRowMin += ROW_COUNT;
            if( this->sRowMin > ( 256 - ROW_COUNT ) )
            {
                this->sRowMin = ( 256 - ROW_COUNT );
            }
            break;

        case Button_Scroll_Down:
            this->sRowMin++;
            if( this->sRowMin > ( 256 - ROW_COUNT ) )
            {
                this->sRowMin = ( 256 - ROW_COUNT );
            }
            break;

        case Button_Prev_Page:
            this->sRowMin -= ROW_COUNT;
            if( this->sRowMin < 0 )
            {
                this->sRowMin = 0;
            }
            break;

        case Button_Struct_Feeder_Next:
            cFeederCountCurrent++;
            break;

        case Button_Struct_Feeder_Prev:
            cFeederCountCurrent--;
            break;

        case Button_Struct_Phase_Next:
            cPhaseCountCurrent[cFeederCountCurrent]++;
            break;

        case Button_Struct_Phase_Prev:
            cPhaseCountCurrent[cFeederCountCurrent]--;
            break;
        }

        break;

    case WIDGET_NOTIFY_APPLY:
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
DebugDebugHorizontal::~DebugDebugHorizontal(void)
{
    delete[] this->pButtons;
}
