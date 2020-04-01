
#include "../debug_backup/debug_inverters.h"

#include "interface.h"
#include "property/inverter_property.h"
#include "property/ups_property.h"

#if ( INVERTER_COUNT > 0 )

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define BORDER_COLOR                0x005c7b99
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
DebugInverters::DebugInverters(unsigned char ucID, Widget *pParent)
               :Widget(ucID, pParent, 0, 0, 240, 320)
{
	Canvas              *pCanvas;

	//
	// Border canvas
	//
	pCanvas = new Canvas(Canvas_Border, this, 2, 27, 236, 239);
	pCanvas->ulStyle = CANVAS_STYLE_RISING_EDGE;
	pCanvas->ulFillColor = BORDER_COLOR;

	//
	// Main canvas
	//
	pCanvas = new Canvas(Canvas_Main, this, 4, 29, 232, 235);
	pCanvas->ulStyle = CANVAS_STYLE_FILL | CANVAS_STYLE_PAINT_NOTIFY;
	pCanvas->ulFillColor = WINDOW_COLOR;
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugInverters::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                unsigned long ulParam2)
{
	tRectangle    sPosition;
	tContext      sCtx;
	char          pcString[17];
	long          lFlags;
	long          lRow = 0;
	long          lX;
	long          lY;

	static const tRow sRows[] =
	{
		{ "Unit",       15 },
		{ "Uin",        12 },
		{ "Uout",       12 },
		{ "Iout",       12 },
		{ "Tconv",      12 },
		{ "Tint",       12 },
		{ "Enable",     12 },
		{ "Offline",    12 },
		{ "M/S/E",      12 },
		{ "SyncOK",     12 },
		{ "PhSync",     12 },
		{ "Errors",     12 },
	};

	const unsigned short usRowsNum = sizeof(sRows) / sizeof(sRows[0]);

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

			GrStringDrawCenteredL(&sCtx, "Inverters:", -1, sCtx.sClipRegion.sXMin + lX,
					                                       sCtx.sClipRegion.sYMin + lY, 0);

			//*****************************************************************
			//
			// Titles
			//
			//*****************************************************************
			GrContextForegroundSet(&sCtx, ClrYellow);

			lX = 5;
			lY = 0;

			for(int i = 0; i < usRowsNum; i++)
			{
				lY += sRows[i].sIndent;

				GrStringDraw(&sCtx, sRows[i].pcName, -1, sCtx.sClipRegion.sXMin + lX,
												         sCtx.sClipRegion.sYMin + lY, 0);
			}

			//*****************************************************************
			//
			// Values
			//
			//*****************************************************************

			//
			// Inverter Num
			//
			lX = 65;
			lY = sRows[lRow++].sIndent + 6;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				usprintf(pcString, "%d", i + 1);
				GrContextForegroundSet(&sCtx, ClrYellow);
				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
			}

			//
			// Inverter Input Voltage
			//
			lX  = 65;
			lY += sRows[lRow++].sIndent;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				lFlags = InverterPropertyGet(i, INVERTER_FLAGS);

				if( InverterPropertyGet(i, INVERTER_STATUS) != INVERTER_STATUS_UNKNOWN )
				{
					InverterPropertyStringGet(i, INVERTER_INPUT_VOLTAGE, pcString);
					GrContextForegroundSet(&sCtx, (lFlags & INVERTER_FLAG_POWEROK) ? ClrWhite : ClrRed);
				}
				else
				{
					usprintf(pcString, "-");
					GrContextForegroundSet(&sCtx, ClrWhite);
				}

				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
			}

			//
			// Inverter Output Voltage
			//
			lX  = 65;
			lY += sRows[lRow++].sIndent;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				lFlags = InverterPropertyGet(i, INVERTER_FLAGS);

				if( InverterPropertyGet(i, INVERTER_STATUS) != INVERTER_STATUS_UNKNOWN )
				{
					InverterPropertyStringGet(i, INVERTER_OUTPUT_VOLTAGE, pcString);
					GrContextForegroundSet(&sCtx, (lFlags & INVERTER_FLAG_POWEROK) ? ClrWhite : ClrRed);
				}
				else
				{
					usprintf(pcString, "-");
					GrContextForegroundSet(&sCtx, ClrWhite);
				}

				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
			}

			//
			// Inverter Output Current
			//
			lX  = 65;
			lY += sRows[lRow++].sIndent;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				lFlags = InverterPropertyGet(i, INVERTER_FLAGS);

				if( InverterPropertyGet(i, INVERTER_STATUS) != INVERTER_STATUS_UNKNOWN )
				{
					if( lFlags & INVERTER_FLAG_OVERLOAD )
					{
						InverterPropertyStringGet(i, INVERTER_OVERLOAD_CURRENT, pcString);
					}
					else
					{
						InverterPropertyStringGet(i, INVERTER_OUTPUT_CURRENT, pcString);
					}

					GrContextForegroundSet(&sCtx, (lFlags & INVERTER_FLAG_OVERLOAD) ? ClrRed : ClrWhite);
				}
				else
				{
					usprintf(pcString, "-");
					GrContextForegroundSet(&sCtx, ClrWhite);
				}

				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
			}

			//
			// Inverter Converter Temperature
			//
			lX  = 65;
			lY += sRows[lRow++].sIndent;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				lFlags = InverterPropertyGet(i, INVERTER_FLAGS);

				if( InverterPropertyGet(i, INVERTER_STATUS) != INVERTER_STATUS_UNKNOWN )
				{
					InverterPropertyStringGet(i, INVERTER_TEMPERATURE_CONV, pcString);
					GrContextForegroundSet(&sCtx, (lFlags & INVERTER_FLAG_OVERHEAT) ? ClrRed : ClrWhite);
				}
				else
				{
					usprintf(pcString, "-");
					GrContextForegroundSet(&sCtx, ClrWhite);
				}

				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
			}

			//
			// Inverter Internal Temperature
			//
			lX  = 65;
			lY += sRows[lRow++].sIndent;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				if( InverterPropertyGet(i, INVERTER_STATUS) != INVERTER_STATUS_UNKNOWN )
				{
					InverterPropertyStringGet(i, INVERTER_TEMPERATURE_INT, pcString);
				}
				else
				{
					usprintf(pcString, "-");
				}

				GrContextForegroundSet(&sCtx, ClrWhite);
				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
			}

			//
			// Inverter Enable Flag
			//
			lX  = 65;
			lY += sRows[lRow++].sIndent;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				lFlags = InverterPropertyGet(i, INVERTER_FLAGS);

				if( InverterPropertyGet(i, INVERTER_STATUS) != INVERTER_STATUS_UNKNOWN )
				{
					usprintf(pcString, (lFlags & INVERTER_FLAG_ON_OFF) ? "1" : "0");
					GrContextForegroundSet(&sCtx, (lFlags & INVERTER_FLAG_ON_OFF) ? ClrRed : ClrLime);
				}
				else
				{
					usprintf(pcString, "-");
					GrContextForegroundSet(&sCtx, ClrWhite);
				}

				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
			}

			//
			// Inverter Offline Flag
			//
			lX  = 65;
			lY += sRows[lRow++].sIndent;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				lFlags = InverterPropertyGet(i, INVERTER_FLAGS);

				if( InverterPropertyGet(i, INVERTER_STATUS) != INVERTER_STATUS_UNKNOWN )
				{
					usprintf(pcString, (lFlags & INVERTER_FLAG_OFFLINE) ? "1" : "0");
					GrContextForegroundSet(&sCtx, (lFlags & INVERTER_FLAG_OFFLINE) ? ClrRed : ClrLime);
				}
				else
				{
					usprintf(pcString, "-");
					GrContextForegroundSet(&sCtx, ClrWhite);
				}

				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
			}

			//
			// Inverter Master/Slave/ExtSync Flag
			//
			lX  = 65;
			lY += sRows[lRow++].sIndent;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				lFlags = InverterPropertyGet(i, INVERTER_FLAGS);

				if( InverterPropertyGet(i, INVERTER_STATUS) != INVERTER_STATUS_UNKNOWN )
				{
					usprintf(pcString, "%s%s", (lFlags & INVERTER_FLAG_MASTER) ? "M" : "S",
							                   (lFlags & INVERTER_FLAG_SYNC_EXTERNAL) ? "E" : "");

					GrContextForegroundSet(&sCtx, !(lFlags & (INVERTER_FLAG_MASTER |
							                                 INVERTER_FLAG_SYNC_EXTERNAL)) ? ClrRed : ClrLime);
				}
				else
				{
					usprintf(pcString, "-");
					GrContextForegroundSet(&sCtx, ClrWhite);
				}

				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
			}

			//
			// Inverter SyncOK Flag
			//
			lX  = 65;
			lY += sRows[lRow++].sIndent;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				lFlags = InverterPropertyGet(i, INVERTER_FLAGS);

				if( InverterPropertyGet(i, INVERTER_STATUS) != INVERTER_STATUS_UNKNOWN )
				{
					usprintf(pcString, (lFlags & INVERTER_FLAG_SYNC_SIGOK) ? "1" : "0");
					GrContextForegroundSet(&sCtx, (lFlags & INVERTER_FLAG_SYNC_SIGOK) ? ClrLime : ClrRed);
				}
				else
				{
					usprintf(pcString, "-");
					GrContextForegroundSet(&sCtx, ClrWhite);
				}

				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
			}

			//
			// Inverter PhaseSync Flag
			//
			lX  = 65;
			lY += sRows[lRow++].sIndent;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				lFlags = InverterPropertyGet(i, INVERTER_FLAGS);

				if( InverterPropertyGet(i, INVERTER_STATUS) != INVERTER_STATUS_UNKNOWN )
				{
					usprintf(pcString, (lFlags & INVERTER_FLAG_SYNC_OK) ? "1" : "0");
					GrContextForegroundSet(&sCtx, (lFlags & INVERTER_FLAG_SYNC_OK) ? ClrLime : ClrRed);
				}
				else
				{
					usprintf(pcString, "-");
					GrContextForegroundSet(&sCtx, ClrWhite);
				}

				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
			}

			//
			// Inverter Comm Errors
			//
			lX  = 65;
			lY += sRows[lRow++].sIndent;

			for( int i = 0; i < INVERTER_COUNT; i++ )
			{
				InverterPropertyStringGet(i, INVERTER_COMM_ERROR_COUNTER, pcString);

				GrContextForegroundSet(&sCtx, ClrWhite);
				GrStringDrawCenteredL(&sCtx, pcString, -1, sCtx.sClipRegion.sXMin + lX,
														   sCtx.sClipRegion.sYMin + lY, 0);
				lX += 25;
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
DebugInverters::~DebugInverters(void)
{

}

#endif
