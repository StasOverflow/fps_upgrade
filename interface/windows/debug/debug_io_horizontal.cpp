
#include "interface.h"
#include "property/debug_property.h"

#include "debug_io_horizontal.h"

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
// Row description structure
//
//*****************************************************************************
typedef struct
{
	const char *pcName;
	      short sIndent;

} tRow;

//*****************************************************************************
//
// I/O pin description structure
//
//*****************************************************************************
typedef struct
{
	unsigned short usPortReg;
	unsigned short usPin;

} tIOPin;


//*****************************************************************************
//
// Array of all I/O pins
//
//*****************************************************************************
const static
tIOPin psIOPins[] =
{
	{ DEBUG_PORTF, 1 << 6 }, // Ib+
	{ DEBUG_PORTF, 1 << 5 }, // Ib-
	{ DEBUG_PORTF, 1 << 4 }, // T_INT
	{ DEBUG_PORTF, 1 << 3 }, // CTC
	{ DEBUG_PORTF, 1 << 2 }, // IA
	{ DEBUG_PORTF, 1 << 1 }, // IB
	{ DEBUG_PORTF, 1 << 0 }, // IC
	{ DEBUG_PORTE, 1 << 2 }, // DX2
	{ DEBUG_PORTE, 1 << 3 }, // FAN
	{ DEBUG_PORTE, 1 << 4 }, // ON
	{ DEBUG_PORTE, 1 << 6 }, // P0
	{ DEBUG_PORTE, 1 << 7 }, // P1
	{ DEBUG_PORTB, 1 << 0 }, // GRP1
	{ DEBUG_PORTB, 1 << 1 }, // GRP2
	{ DEBUG_PORTB, 1 << 4 }, // TEMP_DATA
	{ DEBUG_PORTB, 1 << 5 }, // TEMP_CLK
	{ DEBUG_PORTA, 1 << 0 }, // APS_DATA
	{ DEBUG_PORTA, 1 << 1 }, // APS_CLK
	{ DEBUG_PORTB, 1 << 7 }, // P3
	{ DEBUG_PORTG, 1 << 4 }, // BEEPER
	{ DEBUG_PORTE, 1 << 1 }, // TX2
	{ DEBUG_PORTE, 1 << 0 }, // RX2
	{ DEBUG_PORTA, 1 << 4 }, // P6
	{ DEBUG_PORTG, 1 << 2 }, // P5
	{ DEBUG_PORTC, 1 << 7 }, // DIN
	{ DEBUG_PORTC, 1 << 6 }, // DCLK
	{ DEBUG_PORTC, 1 << 5 }, // DO6
	{ DEBUG_PORTC, 1 << 4 }, // DO5
	{ DEBUG_PORTC, 1 << 3 }, // DO4
	{ DEBUG_PORTC, 1 << 2 }, // DO3
	{ DEBUG_PORTC, 1 << 1 }, // DO2
	{ DEBUG_PORTC, 1 << 0 }, // DO1
	{ DEBUG_PORTG, 1 << 1 }, // K2
	{ DEBUG_PORTG, 1 << 0 }, // K1
	{ DEBUG_PORTD, 1 << 7 }, // FF1
	{ DEBUG_PORTD, 1 << 6 }, // FF2
	{ DEBUG_PORTD, 1 << 4 }, // DX1
	{ DEBUG_PORTD, 1 << 3 }, // TX1
	{ DEBUG_PORTD, 1 << 2 }, // RX1
};

#define DEBUG_IO_PINS_NUM           ( sizeof(psIOPins) / sizeof(psIOPins[0]) )

//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
	Canvas_Border,
	Canvas_Main,

	Widget_Max = 0xFF
};

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugIOHorizontal::DebugIOHorizontal(unsigned char ucID, Widget *pParent)
        :Widget(ucID, pParent, 0, 0, 320, 240)
{
	Canvas              *pCanvas;

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
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugIOHorizontal::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                unsigned long ulParam2)
{
	tRectangle    sPosition;
	tContext      sCtx;
	char          pcString[17];
	long          lColor;
	long          lX;
	long          lY;
	unsigned long ulPin;
	long          lPinState;

	static const tRow sRows[][10] =
	{
		{
			{ "Ib+",   15 },
			{ "Ib-",   12 },
			{ "T_INT", 12 },
			{ "CTC",   12 },
			{ "IA",    12 },
			{ "IB",    12 },
			{ "IC",    12 },
			{ "DX2",   12 },
			{ "FAN",   12 },
			{ "ON",    12 },
		},

		{
			{ "P0",        15 },
			{ "P1",        12 },
			{ "GRP1",      12 },
			{ "GRP2",      12 },
			{ "TEMP_DATA", 12 },
			{ "TEMP_CLK",  12 },
			{ "APS_DATA",  12 },
			{ "APS_CLK",   12 },
			{ "P3",        12 },
			{ "BEEPER",    12 },
		},

		{
			{ "TX2",  15 },
			{ "RX2",  12 },
			{ "P6",   12 },
			{ "P5",   12 },
			{ "DIN",  12 },
			{ "DCLK", 12 },
			{ "DO6",  12 },
			{ "DO5",  12 },
			{ "DO4",  12 },
			{ "DO3",  12 },
		},

		{
			{ "DO2", 15 },
			{ "DO1", 12 },
			{ "K2",  12 },
			{ "K1",  12 },
			{ "FF1", 12 },
			{ "FF2", 12 },
			{ "DX1", 12 },
			{ "TX1", 12 },
			{ "RX1", 12 },
			{ "",    12 },
		},
	};

	static const unsigned short usColumnIndents[] =
	{
		5, 50, 80, 45
	};

	static const unsigned short usValueIndents[] =
	{
		23, 80, 45, 40
	};

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

			lX = (sCtx.sClipRegion.sXMax - sCtx.sClipRegion.sXMin + 1) / 2;
			lY = 6;

			GrStringDrawCenteredL(&sCtx, "General Purpose I/O (GPIO):", -1, sCtx.sClipRegion.sXMin + lX,
					                                                        sCtx.sClipRegion.sYMin + lY, 0);

			//*****************************************************************
			//
			// Titles
			//
			//*****************************************************************
			GrContextForegroundSet(&sCtx, ClrYellow);

			lX = usColumnIndents[0];
			lY = 0;

			for( int iCol = 0; iCol < 4; iCol++ )
			{
				lX += usColumnIndents[iCol];
				lY  = 0;

				for( int iRow = 0; iRow < 10; iRow++ )
				{
					lY += sRows[iCol][iRow].sIndent;

					GrStringDraw(&sCtx, sRows[iCol][iRow].pcName, -1, sCtx.sClipRegion.sXMin + lX,
															          sCtx.sClipRegion.sYMin + lY, 0);
				}
			}

			//*****************************************************************
			//
			// Values
			//
			//*****************************************************************
			lX = usValueIndents[0];
			lY = 0;

			for( int iCol = 0; iCol < 4; iCol++ )
			{
				lX += usValueIndents[iCol];
				lY  = 0;

				for( int iRow = 0; iRow < 10; iRow++ )
				{
					lY += sRows[iCol][iRow].sIndent;

					ulPin = (iCol * 10) + iRow;

					if( ulPin < DEBUG_IO_PINS_NUM )
					{
						lPinState = DebugPropertyGet(psIOPins[ulPin].usPortReg) & psIOPins[ulPin].usPin;

						lColor = (lPinState) ? ClrRed : ClrDodgerBlue;

						GrContextForegroundSet(&sCtx, lColor);

						usprintf(pcString, "%d", (lPinState) ? 1 : 0);

						GrStringDraw(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														  sCtx.sClipRegion.sYMin + lY, 0);
					}

				}
			}

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
DebugIOHorizontal::~DebugIOHorizontal(void)
{

}
