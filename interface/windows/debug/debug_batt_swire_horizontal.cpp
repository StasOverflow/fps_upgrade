
#include "interface.h"

#include "dialogs/dialog_numeric_editor.h"

#include "property/debug_property.h"
#include "property/battery_property.h"

#include "debug_batt_swire_horizontal.h"

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define TABLE_CELL_TEXT_COLOR                0x005c7b99
#define WINDOW_COLOR                0x00000000
#define TEXT_COLOR                  0x00F0F0F0

#define BUTTONS_COUNT               ( 5 )

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

	Dialog_Numeric_Editor_1,
	Dialog_Numeric_Editor_2,
	Dialog_Numeric_Editor_3,
	Dialog_Numeric_Editor_4,
	Dialog_Numeric_Editor_5,

	Button_1,
	Button_2,
	Button_3,
	Button_4,
	Button_5,

	Widget_Max = 0xFF
};

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugBattSwireHorizontal::DebugBattSwireHorizontal(unsigned char ucID, Widget *pParent)
               :Widget(ucID, pParent, 0, 0, 320, 240)
{
	Canvas              *pCanvas;

	//
	// Create pointers to the buttons
	//
	this->pButtons = new RectangularButton *[BUTTONS_COUNT];

	//
	// Clear pionters
	//
	for(int i = 0; i < BUTTONS_COUNT; i++ )
	{
		this->pButtons[i] = 0;
	}

//	//
//	// Border canvas
//	//
//	pCanvas = new Canvas(Canvas_Border, this, DEBUG_TAB_BORDER_POS);
//	pCanvas->ulStyle = CANVAS_STYLE_RISING_EDGE;
//	pCanvas->ulFillColor = BORDER_COLOR;

	//
	// Main canvas
	//
	pCanvas = new Canvas(Canvas_Main, this, DEBUG_TAB_MAIN_POS);
	pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;
}

//*****************************************************************************
//
// Draw ACQMB debug
//
//****************************************************************************
void
DebugBattSwireHorizontal::ACQMBDebugDraw(tContext *pContext)
{
	char pcString[17];
	long lColumn = 0;
	long lX;
	long lY;

	static const tColumn sColumns[] =
	{
		{ "Uout",    13 },
		{ "Uadc",    28 },
		{ "K",       30 },
		{ "T1",      33 },
		{ "T2",      25 },
		{ "T3",      25 },
		{ "Status",  30 },
		{ "Errs",    32 },
	};

	const unsigned short usColumnsNum = sizeof(sColumns) / sizeof(sColumns[0]);

	//*****************************************************************
	//
	// Table name
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrLime);

	lX = (pContext->sClipRegion.sXMax - pContext->sClipRegion.sXMin + 1) / 2;
	lY = 6;

	GrStringDrawCenteredL(pContext, "Acquire Module B (ACQMB):", -1, pContext->sClipRegion.sXMin + lX,
			                                                             pContext->sClipRegion.sYMin + lY, 0);

	//*****************************************************************
	//
	// Titles
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrYellow);

	lX = 0;
	lY = 21;

	for(int i = 0; i < usColumnsNum; i++)
	{
		lX += sColumns[i].sIndent;

		GrStringDrawCenteredL(pContext, sColumns[i].pcName, -1, pContext->sClipRegion.sXMin + lX,
				                                                pContext->sClipRegion.sYMin + lY, 0);
	}

	//*****************************************************************
	//
	// Values
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrWhite);

	lX = 0;
	lY = 33;

	//
	// Voltage
	//
	lX += sColumns[lColumn++].sIndent;

	DebugPropertyStringGet(ACQM_VOLTAGE_VALUE, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);
	//
	// ADCV
	//
	lX += sColumns[lColumn++].sIndent;

	DebugPropertyStringGet(ACQM_VOLTAGE_ADCV, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);
	//
	// Coeff
	//
	lX += sColumns[lColumn++].sIndent;

	if( !this->pButtons[0] )
	{
		//
		// Setup Button
		//
		this->pButtons[0] = new RectangularButton(Button_1 + 0, this, pContext->sClipRegion.sXMin + lX - 18,
				                                                      pContext->sClipRegion.sYMin + lY - 5, 36, 12);
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

	DebugPropertyStringGet(ACQM_VOLTAGE_COEFF, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);

	//
	// Temperature 1
	//
	lX += sColumns[lColumn++].sIndent;

	DebugPropertyStringGet(ACQM_TEMP_1_VALUE, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);

	//
	// Temperature 2
	//
	lX += sColumns[lColumn++].sIndent;

	DebugPropertyStringGet(ACQM_TEMP_2_VALUE, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);

	//
	// Temperature 3
	//
	lX += sColumns[lColumn++].sIndent;

	DebugPropertyStringGet(ACQM_TEMP_3_VALUE, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);

	//
	// Status
	//
	lX += sColumns[lColumn++].sIndent;

	DebugPropertyStringGet(ACQM_COMM_STATUS, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);

	//
	// Comm errors
	//
	lX += sColumns[lColumn++].sIndent;

	DebugPropertyStringGet(ACQM_COMM_ERRORS, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);
}

//*****************************************************************************
//
// Draw APS debug
//
//****************************************************************************
void
DebugBattSwireHorizontal::APSDebugDraw(tContext *pContext)
{
	char pcString[17];
	long lColumn = 0;
	long lX;
	long lY;

	const tColumn sColumns[] =
	{
		{ "Uin",     60 },
		{ "Uout",    25 },
		{ "Status",  35 },
		{ "Errs",    35 },
	};

	const unsigned short usColumnsNum = sizeof(sColumns) / sizeof(sColumns[0]);

	//*****************************************************************
	//
	// Table name
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrLime);

	lX = (pContext->sClipRegion.sXMax - pContext->sClipRegion.sXMin + 1) / 2;
	lY = 50;

	GrStringDrawCenteredL(pContext, "Auxiliary Power Supply (APS):", -1, pContext->sClipRegion.sXMin + lX,
			                                                             pContext->sClipRegion.sYMin + lY, 0);

	//*****************************************************************
	//
	// Titles
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrYellow);

	lX = 0;
	lY = 65;

	for(int i = 0; i < usColumnsNum; i++)
	{
		lX += sColumns[i].sIndent;

		GrStringDrawCenteredL(pContext, sColumns[i].pcName, -1, pContext->sClipRegion.sXMin + lX,
				                                                pContext->sClipRegion.sYMin + lY, 0);
	}

	//*****************************************************************
	//
	// Values
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrWhite);

	lX = 0;
	lY = 77;

	//
	// Uin
	//
	lX += sColumns[lColumn++].sIndent;

	DebugPropertyStringGet(APS_INPUT_VOLTAGE, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);
	//
	// Uout
	//
	lX += sColumns[lColumn++].sIndent;

	DebugPropertyStringGet(APS_OUTPUT_VOLTAGE, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);
	//
	// Status
	//
	lX += sColumns[lColumn++].sIndent;

	DebugPropertyStringGet(APS_COMM_STATUS, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);
	//
	// Comm errors
	//
	lX += sColumns[lColumn++].sIndent;

	DebugPropertyStringGet(APS_COMM_ERRORS, pcString);
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);
}

//*****************************************************************************
//
// Draw Batt Shunt debug
//
//****************************************************************************
void
DebugBattSwireHorizontal::BattShuntDebugDraw(tContext *pContext)
{
	char pcString[17];
	long lColumn = 0;
	long lX;
	long lY;

	const tColumn sColumns[] =
	{
		{ "Ch",     40 },
		{ "Ibatt",  30 },
		{ "Uadc",   38 },
		{ "Coeff",  38 },
		{ "Offset", 38 },
	};

	const unsigned short usColumnsNum = sizeof(sColumns) / sizeof(sColumns[0]);

	const char *pcChannelNames[] =
	{
		"-",
		"+",
	};

	//*****************************************************************
	//
	// Table name
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrLime);

	lX = (pContext->sClipRegion.sXMax - pContext->sClipRegion.sXMin + 1) / 2;
	lY = 94;

	GrStringDrawCenteredL(pContext, "Battery Shunt:", -1, pContext->sClipRegion.sXMin + lX,
			                                              pContext->sClipRegion.sYMin + lY, 0);

	//*****************************************************************
	//
	// Titles
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrYellow);

	lX = 0;
	lY = 109;

	for(int i = 0; i < usColumnsNum; i++)
	{
		lX += sColumns[i].sIndent;

		GrStringDrawCenteredL(pContext, sColumns[i].pcName, -1, pContext->sClipRegion.sXMin + lX,
				                                                pContext->sClipRegion.sYMin + lY, 0);
	}

	//*****************************************************************
	//
	// Values
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrWhite);

	lX = 0;

	//
	// Channel
	//
	lX += sColumns[lColumn++].sIndent;
	lY = 121;

	for( int i = 0; i < 2; i++ )
	{
		GrStringDrawCenteredL(pContext, pcChannelNames[i], -1, pContext->sClipRegion.sXMin + lX,
												               pContext->sClipRegion.sYMin + lY, 0);
		lY += 12;
	}

	//
	// Ibatt +/-
	//
	lX += sColumns[lColumn++].sIndent;
	lY = 121;

	for( int i = 0; i < 2; i++ )
	{
#if ( BATTERY_GROUP_COUNT > 0 )
		BatteryPropertyStringGet(0, BATTERY_CURRENT_NEG + i, pcString);
#else
		usprintf(pcString, "%s", "---");
#endif
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 12;
	}

	//
	// Ibatt
	//
#if ( BATTERY_GROUP_COUNT > 0 )
	BatteryPropertyStringGet(0, BATTERY_CURRENT, pcString);
#else
	usprintf(pcString, "%s", "---");
#endif
	GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
			                                      pContext->sClipRegion.sYMin + lY, 0);
	lY += 13;

	//
	// Uadc
	//
	lX += sColumns[lColumn++].sIndent;
	lY = 121;

	for( int i = 0; i < 2; i++ )
	{
#if ( BATTERY_GROUP_COUNT > 0 )
		BatteryPropertyStringGet(0, BATTERY_CURRENT_NEG_ADCV + i, pcString);
#else
		usprintf(pcString, "%s", "---");
#endif
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 12;
	}

	//
	// Coeff
	//
	lX += sColumns[lColumn++].sIndent;
	lY = 121;

	for( int i = 0; i < 2; i++ )
	{
		if( !this->pButtons[1 + i] )
		{
			//
			// Setup Button
			//
			this->pButtons[1 + i] = new RectangularButton(Button_1 + 1 + i, this, pContext->sClipRegion.sXMin + lX - 18,
					                                                      pContext->sClipRegion.sYMin + lY - 5, 36, 12);
			this->pButtons[1 + i]->ulStyle = RB_STYLE_OUTLINE;
			this->pButtons[1 + i]->ulOutlineColor = ClrWhite;
		}
		else
		{
			if( this->pButtons[1 + i]->ulStyle & RB_STYLE_PRESSED )
			{
				this->pButtons[1 + i]->ulOutlineColor = ClrGray;
			}
			else
			{
				this->pButtons[1 + i]->ulOutlineColor = TEXT_COLOR;
			}
		}

#if ( BATTERY_GROUP_COUNT > 0 )
		BatteryPropertyStringGet(0, BATTERY_CURRENT_CALIBRATION_COEFF_NEG + i, pcString);
#else
		usprintf(pcString, "%s", "---");
#endif
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 12;
	}

	//
	// Offset
	//
	lX += sColumns[lColumn++].sIndent;
	lY = 121;

	for( int i = 0; i < 2; i++ )
	{
		if( !this->pButtons[3 + i] )
		{
			//
			// Setup Button
			//
			this->pButtons[3 + i] = new RectangularButton(Button_1 + 3 + i, this, pContext->sClipRegion.sXMin + lX - 18,
					                                                      pContext->sClipRegion.sYMin + lY - 5, 36, 12);
			this->pButtons[3 + i]->ulStyle = RB_STYLE_OUTLINE;
			this->pButtons[3 + i]->ulOutlineColor = ClrWhite;
		}
		else
		{
			if( this->pButtons[3 + i]->ulStyle & RB_STYLE_PRESSED )
			{
				this->pButtons[3 + i]->ulOutlineColor = ClrGray;
			}
			else
			{
				this->pButtons[3 + i]->ulOutlineColor = TEXT_COLOR;
			}
		}

#if ( BATTERY_GROUP_COUNT > 0 )
		BatteryPropertyStringGet(0, BATTERY_CURRENT_CALIBRATION_OFFSET_NEG + i, pcString);
#else
		usprintf(pcString, "%s", "---");
#endif
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 12;
	}
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugBattSwireHorizontal::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                unsigned long ulParam2)
{
	tRectangle    sPosition;
	tContext      sCtx;
	long          lValue;

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
			this->ACQMBDebugDraw(&sCtx);
			this->APSDebugDraw(&sCtx);
			this->BattShuntDebugDraw(&sCtx);
			break;

		default:
			return 0;
		}

    	break;

	case WIDGET_NOTIFY_BTN_UNPRESSED:
		switch( ulParam1 )
		{
		case Button_1:
			lValue = DebugPropertyGet(ACQM_VOLTAGE_COEFF);
			(new DialogNumericEditor(Dialog_Numeric_Editor_1, this, 80, 70))->Init(lValue, 0);

			break;

		case Button_2:
#if ( BATTERY_GROUP_COUNT > 0 )
			lValue = BatteryPropertyGet(0, BATTERY_CURRENT_CALIBRATION_COEFF_NEG);
			(new DialogNumericEditor(Dialog_Numeric_Editor_2, this, 80, 70))->Init(lValue, 0);
#endif
			break;

		case Button_3:
#if ( BATTERY_GROUP_COUNT > 0 )
			lValue = BatteryPropertyGet(0, BATTERY_CURRENT_CALIBRATION_COEFF_POS);
			(new DialogNumericEditor(Dialog_Numeric_Editor_3, this, 80, 70))->Init(lValue, 0);
#endif
			break;

		case Button_4:
#if ( BATTERY_GROUP_COUNT > 0 )
			lValue = BatteryPropertyGet(0, BATTERY_CURRENT_CALIBRATION_OFFSET_NEG);
			(new DialogNumericEditor(Dialog_Numeric_Editor_4, this, 80, 70))->Init(lValue, 0);
#endif
			break;

		case Button_5:
#if ( BATTERY_GROUP_COUNT > 0 )
			lValue = BatteryPropertyGet(0, BATTERY_CURRENT_CALIBRATION_OFFSET_POS);
			(new DialogNumericEditor(Dialog_Numeric_Editor_5, this, 80, 70))->Init(lValue, 0);
#endif
			break;
		}

		break;

	case WIDGET_NOTIFY_APPLY:
		switch( ulParam1 )
		{
		case Dialog_Numeric_Editor_1:
			DebugPropertySet(ACQM_VOLTAGE_COEFF, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
			break;

		case Dialog_Numeric_Editor_2:
#if ( BATTERY_GROUP_COUNT > 0 )
			BatteryPropertySet(0, BATTERY_CURRENT_CALIBRATION_COEFF_NEG, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
#endif
			break;

		case Dialog_Numeric_Editor_3:
#if ( BATTERY_GROUP_COUNT > 0 )
			BatteryPropertySet(0, BATTERY_CURRENT_CALIBRATION_COEFF_POS, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
#endif
			break;

		case Dialog_Numeric_Editor_4:
#if ( BATTERY_GROUP_COUNT > 0 )
			BatteryPropertySet(0, BATTERY_CURRENT_CALIBRATION_OFFSET_NEG, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
#endif
			break;

		case Dialog_Numeric_Editor_5:
#if ( BATTERY_GROUP_COUNT > 0 )
			BatteryPropertySet(0, BATTERY_CURRENT_CALIBRATION_OFFSET_POS, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
#endif
			break;
		}

		break;

	case WIDGET_NOTIFY_CANCEL:
		switch( ulParam1 )
		{
		case Dialog_Numeric_Editor_1:
		case Dialog_Numeric_Editor_2:
		case Dialog_Numeric_Editor_3:
		case Dialog_Numeric_Editor_4:
		case Dialog_Numeric_Editor_5:
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
DebugBattSwireHorizontal::~DebugBattSwireHorizontal(void)
{
	delete[] this->pButtons;
}
