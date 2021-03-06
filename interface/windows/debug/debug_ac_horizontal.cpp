#include "interface.h"
#include "dialogs/dialog_numeric_editor.h"

#include "property/input_property.h"

#if ( INPUT_PHASE_COUNT > 0 )

#include "debug_ac_horizontal.h"

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

	Dialog_Numeric_Editor_1,
	Dialog_Numeric_Editor_2,
	Dialog_Numeric_Editor_3,
	Dialog_Numeric_Editor_4,
	Dialog_Numeric_Editor_5,
	Dialog_Numeric_Editor_6,

	Button_1,
	Button_2,
	Button_3,
	Button_4,
	Button_5,
	Button_6,

	Widget_Max = 0xFF
};

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugACHorizontal::DebugACHorizontal(unsigned char ucID, Widget *pParent)
        :Widget(ucID, pParent, 2, 2, 315, 200)
{
	Canvas              *pCanvas;

	//
	// Create pointers to the buttons
	//
	this->pButtons = new RectangularButton *[INPUT_PHASE_COUNT];

	//
	// Clear pionters
	//
	for(int i = 0; i < INPUT_PHASE_COUNT; i++ )
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
	pCanvas = new Canvas(Canvas_Main, this, 2, 2, 316, 200);
	pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY
	                 | CANVAS_STYLE_FILL;
	pCanvas->ulFillColor = ClrBrown;
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugACHorizontal::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                unsigned long ulParam2)
{
	tRectangle    sPosition;
	tContext      sCtx;
	char          pcString[17];
	long          lColumn = 0;
	long          lX;
	long          lY;
	long          lValue;

	static const tColumn sColumns[] =
	{
		{ "#",      25 },
		{ "Uac",    30 },

		{ "#",      50 },
		{ "Iac",    30 },
		{ "Uadc",   30 },
		{ "Coeff",  35 },
	};

	const unsigned short usColumnsNum = sizeof(sColumns) / sizeof(sColumns[0]);

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
			//*****************************************************************
			//
			// Table name
			//
			//*****************************************************************
			GrContextForegroundSet(&sCtx, ClrLime);

			lY = 6;

			lX = 45;

			GrStringDrawCenteredL(&sCtx, "AC Voltage:", -1, sCtx.sClipRegion.sXMin + lX,
					                                        sCtx.sClipRegion.sYMin + lY, 0);

			lX += 110;

			GrStringDrawCenteredL(&sCtx, "AC Current:", -1, sCtx.sClipRegion.sXMin + lX,
					                                        sCtx.sClipRegion.sYMin + lY, 0);

			//*****************************************************************
			//
			// Titles
			//
			//*****************************************************************
			GrContextForegroundSet(&sCtx, ClrYellow);

			lX = 0;
			lY = 21;

			for(int i = 0; i < usColumnsNum; i++)
			{
				lX += sColumns[i].sIndent;

				GrStringDrawCenteredL(&sCtx, sColumns[i].pcName, -1, sCtx.sClipRegion.sXMin + lX,
												                     sCtx.sClipRegion.sYMin + lY, 0);
			}

			//*****************************************************************
			//
			// Values
			//
			//*****************************************************************
			GrContextForegroundSet(&sCtx, ClrWhite);

			lX = 0;

			//
			// ###
			//
			lX += sColumns[lColumn++].sIndent;
			lY = 33;

			for(int i = 0; i < INPUT_PHASE_COUNT; i++)
			{
				usprintf(pcString, "%d", i + 1);
				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
												           sCtx.sClipRegion.sYMin + lY, 0);
				lY += 12;
			}

			//
			// Voltage
			//
			lX += sColumns[lColumn++].sIndent;
			lY  = 33;

			for(int i = 0; i < INPUT_PHASE_COUNT; i++)
			{
				InputPhasePropertyStringGet(i, INPUT_VOLTAGE, pcString);
				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
												           sCtx.sClipRegion.sYMin + lY, 0);
				lY += 12;
			}

			//
			// ###
			//
			lX += sColumns[lColumn++].sIndent;
			lY = 33;

			for(int i = 0; i < INPUT_PHASE_COUNT; i++)
			{
				usprintf(pcString, "%d", i + 1);
				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
												           sCtx.sClipRegion.sYMin + lY, 0);
				lY += 12;
			}

			//
			// Current
			//
			lX += sColumns[lColumn++].sIndent;
			lY  = 33;

			for(int i = 0; i < INPUT_PHASE_COUNT; i++)
			{
				InputPhasePropertyStringGet(i, INPUT_CURRENT, pcString);
				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
												           sCtx.sClipRegion.sYMin + lY, 0);
				lY += 12;
			}

			//
			// ADCV
			//
			lX += sColumns[lColumn++].sIndent;
			lY  = 33;

			for(int i = 0; i < INPUT_PHASE_COUNT; i++)
			{
				InputPhasePropertyStringGet(i, INPUT_CURRENT_ADCV, pcString);
				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
												           sCtx.sClipRegion.sYMin + lY, 0);
				lY += 12;
			}

			//
			// Coeff
			//
			lX += sColumns[lColumn++].sIndent;
			lY  = 33;

			for(int i = 0; i < INPUT_PHASE_COUNT; i++)
			{
				if( !this->pButtons[i] )
				{
					//
					// Setup Button
					//
					this->pButtons[i] = new RectangularButton(Button_1 + i, this, sCtx.sClipRegion.sXMin + lX - 18,
							                                                      sCtx.sClipRegion.sYMin + lY - 5, 36, 12);
					this->pButtons[i]->ulStyle = RB_STYLE_OUTLINE;
					this->pButtons[i]->ulOutlineColor = ClrWhite;
				}
				else
				{
					if( this->pButtons[i]->ulStyle & RB_STYLE_PRESSED )
					{
						this->pButtons[i]->ulOutlineColor = ClrGray;
					}
					else
					{
						this->pButtons[i]->ulOutlineColor = TEXT_COLOR;
					}
				}

				InputPhasePropertyStringGet(i, INPUT_CURRENT_CALIBRATION_COEFF, pcString);
				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
												           sCtx.sClipRegion.sYMin + lY, 0);
				lY += 12;
			}

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
			(new DialogNumericEditor(Dialog_Numeric_Editor_1, this, 80, 70))->Init(lValue, 0);
			break;

		case Button_2:
			lValue = InputPhasePropertyGet(1, INPUT_CURRENT_CALIBRATION_COEFF);
			(new DialogNumericEditor(Dialog_Numeric_Editor_2, this, 80, 70))->Init(lValue, 0);
			break;

		case Button_3:
			lValue = InputPhasePropertyGet(2, INPUT_CURRENT_CALIBRATION_COEFF);
			(new DialogNumericEditor(Dialog_Numeric_Editor_3, this, 80, 70))->Init(lValue, 0);
			break;

		case Button_4:
			lValue = InputPhasePropertyGet(3, INPUT_CURRENT_CALIBRATION_COEFF);
			(new DialogNumericEditor(Dialog_Numeric_Editor_4, this, 80, 70))->Init(lValue, 0);
			break;

		case Button_5:
			lValue = InputPhasePropertyGet(4, INPUT_CURRENT_CALIBRATION_COEFF);
			(new DialogNumericEditor(Dialog_Numeric_Editor_5, this, 80, 70))->Init(lValue, 0);
			break;

		case Button_6:
			lValue = InputPhasePropertyGet(5, INPUT_CURRENT_CALIBRATION_COEFF);
			(new DialogNumericEditor(Dialog_Numeric_Editor_6, this, 80, 70))->Init(lValue, 0);
			break;
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

		case Dialog_Numeric_Editor_4:
			InputPhasePropertySet(3, INPUT_CURRENT_CALIBRATION_COEFF, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
			break;

		case Dialog_Numeric_Editor_5:
			InputPhasePropertySet(4, INPUT_CURRENT_CALIBRATION_COEFF, ((DialogNumericEditor *)ulParam2)->ValueGet());
			((DialogNumericEditor *)ulParam2)->Destroy();
			break;

		case Dialog_Numeric_Editor_6:
			InputPhasePropertySet(5, INPUT_CURRENT_CALIBRATION_COEFF, ((DialogNumericEditor *)ulParam2)->ValueGet());
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
DebugACHorizontal::~DebugACHorizontal(void)
{
	delete[] this->pButtons;
}

#endif
