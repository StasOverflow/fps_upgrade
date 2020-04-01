
#include "debug/debug_input.h"
#include "debug/debug_output.h"
#include "debug/debug_psu.h"
#include "debug/debug_system.h"
#include "debug/debug_battery.h"
#include "debug/debug_ins_mon.h"
#include "debug/debug_cfg.h"
#include "debug/debug_extra.h"

#include "interface.h"


#include "debug/debug_batt_swire_horizontal.h"
#include "debug/debug_io_horizontal.h"
#include "debug/debug_system_horizontal.h"
#include "debug/debug_debug_horizontal.h"

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define WINDOW_COLOR                0x005c7b99
#define BUTTON_COLOR                0x00285a89
#define DISP_COLOR                  0x00bcc7aa
#define TEXT_COLOR                  0x00F0F0F0
#define OUTLINE_COLOR               ClrWhite
//*****************************************************************************
//
// Enum of all debug tabs
//
//*****************************************************************************
enum
{
//#if ( INPUT_PHASE_COUNT > 0 )
//	Debug_AC,
//#endif
//#if ( PSU_COUNT > 0 )
//#endif
//	Debug_BattSwire,
//	Debug_IO,
//	Debug_System,
//
//	Debug_Debug,

    Debug_Input,
    Debug_Output,
    Debug_PSU,
    Debug_Battery,

    Debug_InsMon,

    Debug_System,
    Debug_Config,

    Debug_Extra,

    DEBUG_TABS_NUM,
};


static const char *tab_names[] = {
    "Input",
    "Output",
    "PSU",
    "Battery",
    "Ins Mon",
    "System",
    "Config",
    "Extra"
};

//*****************************************************************************
//
// Current selected tab widget
//
//*****************************************************************************
static Widget *pCurrentTab;

//*****************************************************************************
//
// Current selected tab index
//
//*****************************************************************************
static unsigned short usCurrentTabIndex;
//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
	Canvas_Main,

    Canvas_Connection,
	Canvas_Header,

	Button_Plus,
	Button_Minus,

	Button_Back,

    Ind_Connection,

	Widget_Max = 0xFF
};

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
WindowDebugHorizontal::WindowDebugHorizontal(unsigned char ucID, Widget *pParent)
            :Widget(ucID, pParent, 0, 0, 320, 240)
{
	RectangularButton *pButton;
	Canvas            *pCanvas;

    /*
     * Main Canvas
     */
	pCanvas = new Canvas(Canvas_Main, this, 0,  0, 320, 240);
	pCanvas->ulStyle = CANVAS_STYLE_FILL |
			           CANVAS_STYLE_FALLING_EDGE |
					   CANVAS_STYLE_PAINT_NOTIFY;
	pCanvas->ulFillColor = WINDOW_COLOR;


//    /*
//     * Header Canvas
//     */
//    pCanvas = new Canvas(Canvas_Header, this, 10, 2, 20, 25);
//    pCanvas->ulStyle = CANVAS_STYLE_TEXT;
//    pCanvas->ulTextColor = ClrLime;
//    pCanvas->pFont = g_pFontWTahoma13pt;
//    pCanvas->pcText = (char *)">_";

    //
    // Button Minus
    //
    pButton = new RectangularButton(Button_Minus, this, BUTTON_MINUS_POSITION);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = BUTTON_COLOR;
    pButton->ulPressFillColor = BUTTON_COLOR;
    pButton->ulOutlineColor = ClrBlack;
    pButton->ulTextColor = TEXT_COLOR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)"<";

	//
	// Button Plus
	//
	pButton = new RectangularButton(Button_Plus, this, BUTTON_PLUS_POSITION);
	pButton->ulStyle = RB_STYLE_FILL         |
			           RB_STYLE_FALLING_EDGE |
			           RB_STYLE_TEXT;
	pButton->ulFillColor = BUTTON_COLOR;
	pButton->ulPressFillColor = BUTTON_COLOR;
	pButton->ulOutlineColor = ClrBlack;
	pButton->ulTextColor = TEXT_COLOR;
	pButton->pFont = g_pFontWDoslike15pt;
	pButton->pcText = (char *)">";

	//
	// Button Back
	//
	pButton = new RectangularButton(Button_Back, this, 2, 198, 64, 40);
	pButton->ulStyle = RB_STYLE_FILL         |
					   RB_STYLE_FALLING_EDGE |
					   RB_STYLE_TEXT;
	pButton->ulFillColor = BUTTON_COLOR;
	pButton->ulPressFillColor = BUTTON_COLOR;
	pButton->ulOutlineColor = ClrBlack;
	pButton->ulTextColor = TEXT_COLOR;
	pButton->pFont = g_pFontWDoslike15pt;
	pButton->pcText = (char *)"< Back";

	//
	// Create a tab
	//
	pCurrentTab = this->CreateTab(usCurrentTabIndex);
}

//*****************************************************************************
//
// This function creates a debug tab.
//
//*****************************************************************************
Widget*
WindowDebugHorizontal::CreateTab(unsigned short usTab)
{
	Widget *pTab = 0;

	switch(usTab)
	{
	/*
#if ( INPUT_PHASE_COUNT > 0 )
	case Debug_AC:
		pTab = new DebugACHorizontal(usTab, this);
		break;
#endif
	case Debug_IO:
		pTab = new DebugIOHorizontal(usTab, this);
		break;

    case Debug_Debug:
        pTab = new DebugDebugHorizontal(usTab, this);
        break;
*/

	case Debug_Input:
	    pTab = new DebugInputTab(usTab, this);
	    break;

    case Debug_Output:
        pTab = new DebugOutputTab(usTab, this);
        break;

    case Debug_PSU:
        pTab = new DebugPSUTab(usTab, this);
        break;

    case Debug_Battery:
        pTab = new DebugBatteryTab(usTab, this);
        break;

    case Debug_InsMon:
        pTab = new DebugInsMonTab(usTab, this);
        break;

    case Debug_System:
        pTab = new DebugSystemTab(usTab, this);
        break;

    case Debug_Config:
        pTab = new DebugCfgTab(usTab, this);
        break;

    case Debug_Extra:
        pTab = new DebugExtraTab(usTab, this);
        break;
	}

	return pTab;
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
WindowDebugHorizontal::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                unsigned long ulParam2)
{
	tRectangle    sPosition;
	tContext      sCtx;
	char          pcString[17];

    switch( ulMsg )
    {
    case WIDGET_NOTIFY_PAINT:
    	((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&sCtx);

        GrContextClipRegionSet(&sCtx, &sPosition);

		GrContextFontSet(&sCtx, g_pFontWDoslike15pt);

		GrContextForegroundSet(&sCtx, ClrWhite);

		switch( ulParam1 )
		{
		case Canvas_Main:
		    GrImageDraw(&sCtx, g_pucimg_debug, 0, 0);
			usprintf(pcString, "%s  (%d/%d)", tab_names[usCurrentTabIndex], usCurrentTabIndex + 1, DEBUG_TABS_NUM);
			GrStringDrawCenteredL(&sCtx, pcString, -1,
			                      sCtx.sClipRegion.sXMin + 150,
			                      sCtx.sClipRegion.sYMin + 217, 0);
			break;

		default:
			return 0;
		}

    	break;

	case WIDGET_NOTIFY_BTN_UNPRESSED:
		switch( ulParam1 )
		{
		case Button_Plus:
			pCurrentTab->Destroy();

			if( usCurrentTabIndex < (DEBUG_TABS_NUM - 1) )
			{
				usCurrentTabIndex++;
			}

			pCurrentTab = this->CreateTab(usCurrentTabIndex);

			break;

		case Button_Minus:
			pCurrentTab->Destroy();

			if( usCurrentTabIndex > 0 )
			{
				usCurrentTabIndex--;
			}

			pCurrentTab = this->CreateTab(usCurrentTabIndex);

			break;

		case Button_Back:
			WindowManager::MessagePost(WM_MSG_CLOSE, 0);
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

//*****************************************************************************
//
// Destructor of this window
//
//*****************************************************************************
WindowDebugHorizontal::~WindowDebugHorizontal(void)
{
}
