
#include "../debug_backup/debug_batt_pid.h"

#include "interface.h"
#include "dialogs/dialog_numeric_editor.h"

#include "property/debug_property.h"
#include "property/dc_output_property.h"
#include "property/battery_property.h"
#include "property/psu_property.h"

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define TABLE_CELL_TEXT_COLOR                0x005c7b99
#define WINDOW_COLOR                0x00000000
#define TEXT_COLOR                  0x00F0F0F0

#define BUTTONS_COUNT               ( 10 )

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
	Dialog_Numeric_Editor_6,
	Dialog_Numeric_Editor_7,
	Dialog_Numeric_Editor_8,

	Button_1,
	Button_2,
	Button_3,
	Button_4,
	Button_5,
	Button_6,
	Button_7,
	Button_8,
	Button_9,
	Button_10,

	Widget_Max = 0xFF
};

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugBattPID::DebugBattPID(unsigned char ucID, Widget *pParent)
               :Widget(ucID, pParent, 0, 0, 240, 320)
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

	//
	// Border canvas
	//
	pCanvas = new Canvas(Canvas_Border, this, 2, 27, 236, 239);
	pCanvas->ulStyle = CANVAS_STYLE_RISING_EDGE;
	pCanvas->ulFillColor = TABLE_CELL_TEXT_COLOR;

	//
	// Main canvas
	//
	pCanvas = new Canvas(Canvas_Main, this, 4, 29, 232, 235);
	pCanvas->ulStyle = CANVAS_STYLE_FILL | CANVAS_STYLE_PAINT_NOTIFY;
	pCanvas->ulFillColor = WINDOW_COLOR;
}

//*****************************************************************************
//
// Draw Battery PID
//
//****************************************************************************
void
DebugBattPID::BattPIDDraw(tContext *pContext)
{
	char       pcString[17];
	long       lColumn = 0;
	long       lX;
	long       lY;
	long       lButton = 0;

	static const tColumn sColumns[] =
	{
		{ "#",      13 },
		{ "Kp",     30 },
		{ "Ki",     37 },
		{ "Kd",     37 },
		{ "dt",     37 },
		{ "Err",    37 },
		{ "EN",     27 },
	};

	const unsigned short usColumnsNum = sizeof(sColumns) / sizeof(sColumns[0]);

	static const char *pcPIDNames[] =
	{
		"CV",
		"CC"
	};

	const unsigned short usPIDsNum = sizeof(pcPIDNames) / sizeof(pcPIDNames[0]);

	//*****************************************************************
	//
	// Table name
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrLime);

	lX = (pContext->sClipRegion.sXMax - pContext->sClipRegion.sXMin + 1) / 2;
	lY = 6;

	GrStringDrawCenteredL(pContext, "Battery PIDs:", -1, pContext->sClipRegion.sXMin + lX,
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

	//
	// PID name
	//
	lX += sColumns[lColumn++].sIndent;
	lY  = 33;

	for( int i = 0; i < usPIDsNum; i++ )
	{
		usprintf(pcString, "%s", pcPIDNames[i]);
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 13;
	}

	//
	// PID Kp
	//
	lX += sColumns[lColumn++].sIndent;
	lY  = 33;

	for( int i = 0; i < usPIDsNum; i++ )
	{
		if( !this->pButtons[lButton] )
		{
			//
			// Setup Button
			//
			this->pButtons[lButton] = new RectangularButton(Button_1 + lButton, this, pContext->sClipRegion.sXMin + lX - 18,
					                                                                  pContext->sClipRegion.sYMin + lY - 5, 36, 12);
			this->pButtons[lButton]->ulStyle = RB_STYLE_OUTLINE;
			this->pButtons[lButton]->ulOutlineColor = ClrWhite;
		}
		else
		{
			if( this->pButtons[lButton]->ulStyle & RB_STYLE_PRESSED )
			{
				this->pButtons[lButton]->ulOutlineColor = ClrGray;
			}
			else
			{
				this->pButtons[lButton]->ulOutlineColor = TEXT_COLOR;
			}
		}

		lButton++;

		//
		// todo: 5 is a size of battery pid
		//
		BatteryPropertyStringGet(0, BATTERY_VC_PID_KP + (i * 5), pcString);
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 13;
	}

	//
	// PID Ki
	//
	lX += sColumns[lColumn++].sIndent;
	lY  = 33;

	for( int i = 0; i < usPIDsNum; i++ )
	{
		if( !this->pButtons[lButton] )
		{
			//
			// Setup Button
			//
			this->pButtons[lButton] = new RectangularButton(Button_1 + lButton, this, pContext->sClipRegion.sXMin + lX - 18,
					                                                                  pContext->sClipRegion.sYMin + lY - 5, 36, 12);
			this->pButtons[lButton]->ulStyle = RB_STYLE_OUTLINE;
			this->pButtons[lButton]->ulOutlineColor = ClrWhite;
		}
		else
		{
			if( this->pButtons[lButton]->ulStyle & RB_STYLE_PRESSED )
			{
				this->pButtons[lButton]->ulOutlineColor = ClrGray;
			}
			else
			{
				this->pButtons[lButton]->ulOutlineColor = TEXT_COLOR;
			}
		}

		lButton++;

		//
		// todo: 5 is a size of battery pid
		//
		BatteryPropertyStringGet(0, BATTERY_VC_PID_KI + (i * 5), pcString);
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 13;
	}

	//
	// PID Kd
	//
	lX += sColumns[lColumn++].sIndent;
	lY  = 33;

	for( int i = 0; i < usPIDsNum; i++ )
	{
		if( !this->pButtons[lButton] )
		{
			//
			// Setup Button
			//
			this->pButtons[lButton] = new RectangularButton(Button_1 + lButton, this, pContext->sClipRegion.sXMin + lX - 18,
					                                                                  pContext->sClipRegion.sYMin + lY - 5, 36, 12);
			this->pButtons[lButton]->ulStyle = RB_STYLE_OUTLINE;
			this->pButtons[lButton]->ulOutlineColor = ClrWhite;
		}
		else
		{
			if( this->pButtons[lButton]->ulStyle & RB_STYLE_PRESSED )
			{
				this->pButtons[lButton]->ulOutlineColor = ClrGray;
			}
			else
			{
				this->pButtons[lButton]->ulOutlineColor = TEXT_COLOR;
			}
		}

		lButton++;

		//
		// todo: 5 is a size of battery pid
		//
		BatteryPropertyStringGet(0, BATTERY_VC_PID_KD + (i * 5), pcString);
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 13;
	}

	//
	// PID dt
	//
	lX += sColumns[lColumn++].sIndent;
	lY  = 33;

	for( int i = 0; i < usPIDsNum; i++ )
	{
		if( !this->pButtons[lButton] )
		{
			//
			// Setup Button
			//
			this->pButtons[lButton] = new RectangularButton(Button_1 + lButton, this, pContext->sClipRegion.sXMin + lX - 18,
					                                                                  pContext->sClipRegion.sYMin + lY - 5, 36, 12);
			this->pButtons[lButton]->ulStyle = RB_STYLE_OUTLINE;
			this->pButtons[lButton]->ulOutlineColor = ClrWhite;
		}
		else
		{
			if( this->pButtons[lButton]->ulStyle & RB_STYLE_PRESSED )
			{
				this->pButtons[lButton]->ulOutlineColor = ClrGray;
			}
			else
			{
				this->pButtons[lButton]->ulOutlineColor = TEXT_COLOR;
			}
		}

		lButton++;

		BatteryPropertyStringGet(0, BATTERY_PID_DT, pcString);
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 13;
	}

	//
	// PID Err
	//
	lX += sColumns[lColumn++].sIndent;
	lY  = 33;

	for( int i = 0; i < usPIDsNum; i++ )
	{
		//
		// todo: 5 is a size of battery pid
		//
		BatteryPropertyStringGet(0, BATTERY_VC_PID_ERROR + (i * 5), pcString);
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 13;
	}

	//
	// PID Enable
	//
	lX += sColumns[lColumn++].sIndent;
	lY  = 33;

	for( int i = 0; i < usPIDsNum; i++ )
	{
		if( !this->pButtons[lButton] )
		{
			//
			// Setup Button
			//
			this->pButtons[lButton] = new RectangularButton(Button_1 + lButton, this, pContext->sClipRegion.sXMin + lX - 8,
					                                                                  pContext->sClipRegion.sYMin + lY - 5, 16, 12);
			this->pButtons[lButton]->ulStyle = RB_STYLE_OUTLINE;
			this->pButtons[lButton]->ulOutlineColor = ClrWhite;
		}
		else
		{
			if( this->pButtons[lButton]->ulStyle & RB_STYLE_PRESSED )
			{
				this->pButtons[lButton]->ulOutlineColor = ClrGray;
			}
			else
			{
				this->pButtons[lButton]->ulOutlineColor = TEXT_COLOR;
			}
		}

		lButton++;

		//
		// todo: 5 is a size of battery pid
		//
		BatteryPropertyStringGet(0, BATTERY_VC_ENABLE + (i * 5), pcString);
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 13;
	}
}


//*****************************************************************************
//
// Draw Battery params
//
//****************************************************************************
void
DebugBattPID::BattParamDraw(tContext *pContext)
{
	char       pcString[17];
	long       lColumn = 0;
	long       lX;
	long       lY;
//	long       lButton = 0;

	static const tColumn sColumns[] =
	{
		{ "#",              40 },
		{ "Value Set",      40 },
		{ "Value Measured", 75 },
	};

	const unsigned short usColumnsNum = sizeof(sColumns) / sizeof(sColumns[0]);

	static const char *pcValueNames[] =
	{
		"U",
		"I",
	};

	const unsigned short usValuesNum = sizeof(pcValueNames) / sizeof(pcValueNames[0]);

	//*****************************************************************
	//
	// Table name
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrLime);

	lX = (pContext->sClipRegion.sXMax - pContext->sClipRegion.sXMin + 1) / 2;
	lY = 63;

	GrStringDrawCenteredL(pContext, "Battery Params:", -1, pContext->sClipRegion.sXMin + lX,
			                                               pContext->sClipRegion.sYMin + lY, 0);

	//*****************************************************************
	//
	// Titles
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrYellow);

	lX = 0;
	lY = 78;

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
	// Value name
	//
	lX += sColumns[lColumn++].sIndent;
	lY  = 90;

	for( int i = 0; i < usValuesNum; i++ )
	{
		usprintf(pcString, "%s", pcValueNames[i]);
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 13;
	}

	//
	// Value set
	//
	lX += sColumns[lColumn++].sIndent;
	lY  = 90;

	for( int i = 0; i < usValuesNum; i++ )
	{
//		if( !this->pButtons[lButton] )
//		{
//			//
//			// Setup Button
//			//
//			this->pButtons[lButton] = new RectangularButton(Button_1 + lButton, this, pContext->sClipRegion.sXMin + lX - 18,
//					                                                                  pContext->sClipRegion.sYMin + lY - 5, 36, 12);
//			this->pButtons[lButton]->ulStyle = RB_STYLE_OUTLINE;
//			this->pButtons[lButton]->ulOutlineColor = ClrWhite;
//		}
//		else
//		{
//			if( this->pButtons[lButton]->ulStyle & RB_STYLE_PRESSED )
//			{
//				this->pButtons[lButton]->ulOutlineColor = ClrGray;
//			}
//			else
//			{
//				this->pButtons[lButton]->ulOutlineColor = TEXT_COLOR;
//			}
//		}
//
//		lButton++;

		switch( i )
		{
		case 0:
			BatteryPropertyStringGet(0, BATTERY_VOLTAGE_SETUP, pcString);
			break;

		case 1:
			BatteryPropertyStringGet(0, BATTERY_CURRENT_LIMIT, pcString);
			break;
		}

		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 13;
	}

	//
	// Value measured
	//
	lX += sColumns[lColumn++].sIndent;
	lY  = 90;

	for( int i = 0; i < usValuesNum; i++ )
	{
		switch( i )
		{
		case 0:
			BatteryPropertyStringGet(0, BATTERY_VOLTAGE, pcString);
			break;

		case 1:
			BatteryPropertyStringGet(0, BATTERY_CURRENT, pcString);
			break;

		default:
			pcString[0] = '\0';
			break;
		}

		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY, 0);
		lY += 13;
	}
}


//*****************************************************************************
//
// Draw Battery status
//
//****************************************************************************
void
DebugBattPID::BattStatusDraw(tContext *pContext)
{

	char       pcString[17];
	long       lColumn = 0;
	long       lX;
	long       lY;
	long       lY_table;

	static const tColumn sColumns[] =
	{
		{ "Properties:",   80},
		{ "Status",        100 },
	};

	const unsigned short usColumnsNum = sizeof(sColumns) / sizeof(sColumns[0]);

	static const char *pcValueNames[] =
	{
		"Capacity used",
		"Capacity full",
		"Charge level",
		"Status",
		"Flags"
	};

	const unsigned short usValuesNum = sizeof(pcValueNames) / sizeof(pcValueNames[0]);

	//*****************************************************************
	//
	// Table name
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrLime);

	lX = (pContext->sClipRegion.sXMax - pContext->sClipRegion.sXMin + 1) / 2;
	lY = 120;

	GrStringDrawCenteredL(pContext, "Battery Status:", -1, pContext->sClipRegion.sXMin + lX,
			                                               pContext->sClipRegion.sYMin + lY, 0);

	//*****************************************************************
	//
	// Titles
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrYellow);

	lX = 0;
	lY += 15;

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
	// Value name
	//
	lX += sColumns[lColumn++].sIndent;
	lY_table = lY + 12;

	for( int i = 0; i < usValuesNum; i++ )
	{
		usprintf(pcString, "%s", pcValueNames[i]);
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY_table, 0);
		lY_table += 13;
	}

	//
	// Value set
	//
	lX += sColumns[lColumn++].sIndent;
	lY_table = lY + 12;

	for( int i = 0; i < usValuesNum; i++ )
	{

		switch( i )
		{
		case 0:
			BatteryPropertyStringGet(0, BATTERY_FULL_CAPACITY, pcString);
			break;

		case 1:
			BatteryPropertyStringGet(0, BATTERY_USED_CAPACITY, pcString);
			break;

		case 2:
			BatteryPropertyStringGet(0, BATTERY_LEVEL, pcString);
			break;

		case 3:
			BatteryPropertyStringGet(0, BATTERY_STATUS, pcString);
			break;

		case 4:
			BatteryPropertyStringGet(0, BATTERY_FLAGS, pcString);
			break;
		}

		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
				                                      pContext->sClipRegion.sYMin + lY_table, 0);
		lY_table += 13;
	}
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugBattPID::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                  unsigned long ulParam2)
{
	tRectangle sPosition;
	tContext   sCtx;
	long       lValue;

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
			this->BattPIDDraw(&sCtx);
			this->BattParamDraw(&sCtx);
			this->BattStatusDraw(&sCtx);
			break;

		default:
			return 0;
		}

    	break;

	case WIDGET_NOTIFY_BTN_UNPRESSED:
		switch( ulParam1 )
		{
		case Button_1:
			lValue = BatteryPropertyGet(0, BATTERY_VC_PID_KP);
			(new DialogNumericEditor(Dialog_Numeric_Editor_1, this, 80, 70))->Init(lValue, 0);
			break;

		case Button_2:
			lValue = BatteryPropertyGet(0, BATTERY_CC_PID_KP);
			(new DialogNumericEditor(Dialog_Numeric_Editor_2, this, 80, 70))->Init(lValue, 0);
			break;

		case Button_3:
			lValue = BatteryPropertyGet(0, BATTERY_VC_PID_KI);
			(new DialogNumericEditor(Dialog_Numeric_Editor_3, this, 80, 70))->Init(lValue, 0);
			break;

		case Button_4:
			lValue = BatteryPropertyGet(0, BATTERY_CC_PID_KI);
			(new DialogNumericEditor(Dialog_Numeric_Editor_4, this, 80, 70))->Init(lValue, 0);
			break;

		case Button_5:
			lValue = BatteryPropertyGet(0, BATTERY_VC_PID_KD);
			(new DialogNumericEditor(Dialog_Numeric_Editor_5, this, 80, 70))->Init(lValue, 0);
			break;

		case Button_6:
			lValue = BatteryPropertyGet(0, BATTERY_CC_PID_KD);
			(new DialogNumericEditor(Dialog_Numeric_Editor_6, this, 80, 70))->Init(lValue, 0);
			break;

		case Button_7:
			lValue = BatteryPropertyGet(0, BATTERY_PID_DT);
			(new DialogNumericEditor(Dialog_Numeric_Editor_7, this, 80, 70))->Init(lValue, 3);
			break;

		case Button_8:
			lValue = BatteryPropertyGet(0, BATTERY_PID_DT);
			(new DialogNumericEditor(Dialog_Numeric_Editor_8, this, 80, 70))->Init(lValue, 3);
			break;

		case Button_9:
			lValue  = BatteryPropertyGet(0, BATTERY_VC_ENABLE);
			lValue ^= 1;
			BatteryPropertySet(0, BATTERY_VC_ENABLE, lValue);
			break;

		case Button_10:
			lValue = BatteryPropertyGet(0, BATTERY_CC_ENABLE);
			lValue ^= 1;
			BatteryPropertySet(0, BATTERY_CC_ENABLE, lValue);
			break;
		}

		break;

	case WIDGET_NOTIFY_APPLY:
		switch( ulParam1 )
		{
		case Dialog_Numeric_Editor_1:
			BatteryPropertySet(0, BATTERY_VC_PID_KP, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
			break;

		case Dialog_Numeric_Editor_2:
			BatteryPropertySet(0, BATTERY_CC_PID_KP, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
			break;
		case Dialog_Numeric_Editor_3:
			BatteryPropertySet(0, BATTERY_VC_PID_KI, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
			break;

		case Dialog_Numeric_Editor_4:
			BatteryPropertySet(0, BATTERY_CC_PID_KI, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
			break;

		case Dialog_Numeric_Editor_5:
			BatteryPropertySet(0, BATTERY_VC_PID_KD, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
			break;

		case Dialog_Numeric_Editor_6:
			BatteryPropertySet(0, BATTERY_CC_PID_KD, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
			break;

		case Dialog_Numeric_Editor_7:
			BatteryPropertySet(0, BATTERY_PID_DT, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
			break;

		case Dialog_Numeric_Editor_8:
			BatteryPropertySet(0, BATTERY_PID_DT, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
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
		case Dialog_Numeric_Editor_6:
		case Dialog_Numeric_Editor_7:
		case Dialog_Numeric_Editor_8:
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
DebugBattPID::~DebugBattPID(void)
{
	delete[] this->pButtons;
}
