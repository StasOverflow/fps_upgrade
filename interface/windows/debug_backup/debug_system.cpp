
#include "../debug_backup/debug_system.h"

#include "interface.h"
#include "dialogs/window_confirm.h"
#include "dialogs/dialog_numeric_editor.h"
#include "dialogs/dialog_adjust.h"
#include "dialogs/dialog_adjust_double.h"
#include "dialogs/dialog_confirmation.h"
#include "property/ups_property.h"
#include "property/debug_property.h"
#include "property/net_property.h"

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define TABLE_CELL_TEXT_COLOR                         0x005c7b99
#define WINDOW_COLOR                         0x00000000
#define TEXT_COLOR                           0x00F0F0F0
#define ALARM_WINDOW_COLOR                   0x004F7099
//*****************************************************************************
//
// Column description structure
//
//*****************************************************************************
static unsigned long ip_modified;

#if ALLOW_MAC_SETTING == 1
unsigned char        mac_modified[6];
#endif

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
	Dialog_Adjust,
	Dialog_Adjustments_Reset,
	Dialog_Calibrations_Reset,
	Dialog_Factory_Reset,

	Ip_Numeric_Editor_Begin,
    Dialog_Numeric_Editor_Ip_1 = Ip_Numeric_Editor_Begin,
    Dialog_Numeric_Editor_Ip_2,
    Dialog_Numeric_Editor_Ip_3,
    Dialog_Numeric_Editor_Ip_4,
    Dialog_Ip_Apply,

#if ALLOW_MAC_SETTING == 1
    Mac_Numeric_Editor_Begin,
    Dialog_Adjust_Mac_1 = Mac_Numeric_Editor_Begin,
    Dialog_Adjust_Mac_2,
    Dialog_Adjust_Mac_3,
    Dialog_Adjust_Mac_4,
    Dialog_Adjust_Mac_5,
    Dialog_Adjust_Mac_6,
    Dialog_Mac_Apply,
#endif

	Button_Begin,
	Button_Model = Button_Begin,
	Button_Serial_1,
	Button_Serial_2,

	Button_Ip_Apply,

	Button_Ip_Begin,
    Button_Ip_1 = Button_Ip_Begin,
    Button_Ip_2,
    Button_Ip_3,
    Button_Ip_4,

    Button_Ip_Undo,

#if ALLOW_MAC_SETTING == 1
    Button_Mac_Apply,

    Button_Mac_Begin,
    Button_Mac_1 = Button_Mac_Begin,
    Button_Mac_2,
    Button_Mac_3,
    Button_Mac_4,
    Button_Mac_5,
    Button_Mac_6,

#endif
#if ALLOW_MAC_SETTING == 1
    Button_Mac_Undo,
#endif
	Button_Adjustments_Reset,
	Button_Calibrations_Reset,
	Button_Factory_Reset,

	Button_End,

	Widget_Max = 0xFF
};

static const unsigned short usBtnNum = Button_End - Button_Begin;
//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
DebugSystem::DebugSystem(unsigned char ucID, Widget *pParent)
            :Widget(ucID, pParent, 0, 0, 240, 320)
{
	Canvas            *pCanvas;
	unsigned char     btn = 0;

    ip_modified = net_ip_address_get();

#if ALLOW_MAC_SETTING == 1
    memcpy(mac_modified, net_mac_address_get(), 6);
#endif
    memcpy(this->mac_modified, net_mac_address_get(), 6);

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


	this->pButtons = new RectangularButton * [usBtnNum];

	//
	// Button Model
	//
	this->pButtons[btn] = new RectangularButton(Button_Model, this, 188, 38, 39, 16);
	this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE;
	this->pButtons[btn]->ulOutlineColor = ClrWhite;

	//
	// Button Serial 1
	//
	this->pButtons[++btn] = new RectangularButton(Button_Serial_1, this, 174, 68, 20, 16);
	this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE;
	this->pButtons[btn]->ulOutlineColor = ClrWhite;

	//
	// Button Serial 2
	//
	this->pButtons[++btn] = new RectangularButton(Button_Serial_2, this, 193, 68, 33, 16);
	this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE;
	this->pButtons[btn]->ulOutlineColor = ClrWhite;

	//
	// Button Ip 1..4
	//
	for( short i = 0; i < 4; i++ )
	{
	    this->pButtons[++btn] = new RectangularButton(Button_Ip_1 + i, this, 77 + i*34, 178, 25, 16);
	    this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE;
	}

	//
	// Button Ip Undo
	//
    this->pButtons[++btn] = new RectangularButton(Button_Ip_Undo, this, 210, 178, 20, 16);
    this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE
                                 | RB_STYLE_TEXT;
    this->pButtons[btn]->ulTextColor = TEXT_COLOR;
    this->pButtons[btn]->pFont = g_pFontWTahoma13pt;
    this->pButtons[btn]->pcText = (char *) "C";

    //
    // Button Ip Apply
    //
    this->pButtons[++btn] = new RectangularButton(Button_Ip_Apply, this, 10, 178, 30, 16);
    this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE;

#if ALLOW_MAC_SETTING == 1

    //
    // Button Mac 1..6
    //
    for( short i = 0; i < 6; i++ )
    {
        this->pButtons[++btn] = new RectangularButton(Button_Mac_1 + i, this, 45 + i*28, 198, 19, 16);
        this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE;
    }

    //
    // Button Mac Undo
    //
    this->pButtons[++btn] = new RectangularButton(Button_Mac_Undo, this, 210, 198, 20, 16);
    this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE
                                 | RB_STYLE_TEXT;
    this->pButtons[btn]->ulTextColor = TEXT_COLOR;
    this->pButtons[btn]->pFont = g_pFontWTahoma13pt;
    this->pButtons[btn]->pcText = (char *) "C";

    //
    // Button Mac Apply
    //
    this->pButtons[++btn] = new RectangularButton(Button_Mac_Apply, this, 10, 198, 30, 16);
    this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE;
#endif

	//
	// Button Adjustments reset
	//
	this->pButtons[++btn] = new RectangularButton(Button_Adjustments_Reset, this, 5, 231, 75, 30);
	this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE
								 | RB_STYLE_FALLING_EDGE
								 | RB_STYLE_TEXT;
	this->pButtons[btn]->ulOutlineColor = ClrLime;
	this->pButtons[btn]->ulTextColor = TEXT_COLOR;
	this->pButtons[btn]->pFont = g_pFontWTahoma12pt;
	this->pButtons[btn]->pcText = (char *)"Adjustments";

	//
	// Button Calibrations reset
	//
	this->pButtons[++btn] = new RectangularButton(Button_Calibrations_Reset, this, 81, 231, 76, 30);
	this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE
			 	 	 	 	 	 | RB_STYLE_FALLING_EDGE
								 | RB_STYLE_TEXT;
	this->pButtons[btn]->ulOutlineColor = ClrLime;
	this->pButtons[btn]->ulTextColor = TEXT_COLOR;
	this->pButtons[btn]->pFont = g_pFontWTahoma12pt;
	this->pButtons[btn]->pcText = (char *)"Calibrations";

	//
	// Button Factory reset
	//
	this->pButtons[++btn] = new RectangularButton(Button_Factory_Reset, this, 158, 231, 75, 30);
	this->pButtons[btn]->ulStyle = RB_STYLE_OUTLINE
								 | RB_STYLE_FALLING_EDGE
								 | RB_STYLE_TEXT;
	this->pButtons[btn]->ulOutlineColor = ClrLime;
	this->pButtons[btn]->ulTextColor = TEXT_COLOR;
	this->pButtons[btn]->pFont = g_pFontWTahoma12pt;
	this->pButtons[btn]->pcText = (char *)"Factory";
}
//*****************************************************************************
//
// Draw Debug register table
//
//*****************************************************************************
void
DebugSystem::DebugDebugRender(tContext *pContext)
{
	char       pcString[17];
	long       lColumn = 0;
	long       lX;
	long       lY = 0;
	long       lY_table;

	static const tColumn sColumns[] =
	{
		{ "Regs:",      35 },
		{ "Vals",       55 },
	};

	const unsigned short usColumnsNum = sizeof(sColumns) / sizeof(sColumns[0]);

	const unsigned short usValuesNum = DEBUG_REG_8 -  DEBUG_REG_1 + 1;


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
	lY_table = lY + 15;

	for( int i = 0; i < usValuesNum; i++ )
	{
		usprintf(pcString, "Reg_%d", i+1);
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
													  pContext->sClipRegion.sYMin + lY_table, 0);
		lY_table += 13;
	}

	//
	// Value set
	//
	lX += sColumns[lColumn++].sIndent;
	lY_table = lY + 15;

	for( int i = 0; i < usValuesNum; i++ )
	{

		uint16_t value = DebugPropertyGet(DEBUG_REG_1 + i);

		usprintf(pcString, "%02d", value);
		GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
													  pContext->sClipRegion.sYMin + lY_table, 0);
		lY_table += 13;
	}

	lColumn = 0;
	lY_table = 90;

    lX = sColumns[0].sIndent + 110;

    GrContextForegroundSet(pContext, ClrYellow);
    usprintf(pcString, "Sends:");
    GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
                                                  pContext->sClipRegion.sYMin + lY_table, 0);


    GrContextForegroundSet(pContext, ClrWhite);
    lX += sColumns[1].sIndent;
    usprintf(pcString, "%d", inter_comm_conn_instances_get());
    GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
                                                  pContext->sClipRegion.sYMin + lY_table, 0);

    lX = sColumns[0].sIndent + 110;
    lY_table += 15;

    GrContextForegroundSet(pContext, ClrYellow);
    usprintf(pcString, "Errors:");
    GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
                                                  pContext->sClipRegion.sYMin + lY_table, 0);

    GrContextForegroundSet(pContext, ClrWhite);
    lX += sColumns[1].sIndent;

    usprintf(pcString, "%d", inter_comm_conn_errs_get());
    GrStringDrawCenteredL(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
                                                  pContext->sClipRegion.sYMin + lY_table, 0);

}
//*****************************************************************************
//
// Render Net ccfg stuff
//
//****************************************************************************
void
DebugSystem::DebugNetRender(tContext *pContext)
{
    short sX;
    short sY;
    short sXIndent;
    short sYIndent;
    char pcString[20];
    unsigned long ip;
    unsigned short ip_section;
#if ALLOW_MAC_SETTING == 1
    unsigned char * net_mac;
#endif

    sX = pContext->sClipRegion.sXMin + 10;
    sY = pContext->sClipRegion.sYMin + 150;

    sXIndent = sX;
    sYIndent = sY;

    GrContextForegroundSet(pContext, ClrYellow);

    usprintf(pcString, "    IP:");
    GrStringDraw(pContext, pcString, -1, sXIndent, sYIndent, 0);

    sYIndent += 20;

#if ALLOW_MAC_SETTING == 1
    usprintf(pcString, "MAC:");

    GrStringDraw(pContext, pcString, -1, sXIndent, sYIndent, 0);
#endif

    GrContextForegroundSet(pContext, ClrWhite);

    sXIndent = sX + 67;
    sYIndent = sY;

    ip = ip_modified;

    for( short i = 0; i < 4; i++ )
    {
        if( i )
        {
            sXIndent += 9;
        }
        ip_section = ( ip >> ((3 - i) * 8) ) & 0xFF;
        if( ip_section < 10 )
        {
            usprintf(pcString, "%5d", ip_section);
        }
        else
        if( ip_section < 100 )
        {
            usprintf(pcString, "%4d", ip_section);
        }
        else
        {
            usprintf(pcString, "%3d", ip_section);
        }
        GrStringDraw(pContext, pcString, -1, sXIndent, sYIndent, 0);
        if( i < 3 )
        {
            usprintf(pcString, ".");
            sXIndent += 25;
            GrStringDraw(pContext, pcString, -1, sXIndent, sYIndent, 0);
        }
    }

#if ALLOW_MAC_SETTING == 1
    sXIndent = sX + 35;
    sYIndent = sY + 20;

    net_mac = mac_modified;

    for( short i = 0; i < 6; i++ )
    {
        if( i )
        {
            sXIndent += 9;
        }
        usprintf(pcString, "%02x", net_mac[i]);
        GrStringDraw(pContext, pcString, -1, sXIndent, sYIndent, 0);
        if( i < 5 )
        {
            usprintf(pcString, ":");
            sXIndent += 19;
            GrStringDraw(pContext, pcString, -1, sXIndent, sYIndent, 0);
        }
    }
#endif
}

//*****************************************************************************
//
// Render System serials and stuff
//
//****************************************************************************

void
DebugSystem::DebugSystemRender(tContext *pContext)
{
	long           lX = 0;
	long           lY = 0;
	char           pcString[17];

	//*****************************************************************
	//
	// Model Title
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrLime);

	lX += 130;
	lY += 10;

	GrStringDraw(pContext, "Model:", -1, pContext->sClipRegion.sXMin + lX,
										 pContext->sClipRegion.sYMin + lY, 0);

	//*****************************************************************
	//
	// Model Value
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrWhite);

	lX += 42;

	strcpy(pcString, "U   ");
	UPSPropertyStringGet(SYSTEM_MODEL_MAJOR, pcString + strlen(pcString));
	strcat(pcString, ".");
	UPSPropertyStringGet(SYSTEM_MODEL_MINOR, pcString + strlen(pcString));

	GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
										 pContext->sClipRegion.sYMin + lY, 0);

	//*****************************************************************
	//
	// Serial Number Title
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrLime);

	lX = 135;
	lY += 30;

	GrStringDraw(pContext, "SN:", -1, pContext->sClipRegion.sXMin + lX,
									  pContext->sClipRegion.sYMin + lY, 0);

	//*****************************************************************
	//
	// Serial Number Value
	//
	//*****************************************************************
	GrContextForegroundSet(pContext, ClrWhite);

	lX += 38;

	UPSPropertyStringGet(SYSTEM_SERIAL_NUMBER_0, pcString);
//	strcat(pcString, "  ");

	GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
										 pContext->sClipRegion.sYMin + lY, 0);


	lX += 21;

	UPSPropertyStringGet(SYSTEM_SERIAL_NUMBER_1, pcString/* + strlen(pcString)*/);
	UPSPropertyStringGet(SYSTEM_SERIAL_NUMBER_2, pcString + strlen(pcString));

	GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + lX,
										 pContext->sClipRegion.sYMin + lY, 0);


}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DebugSystem::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                unsigned long ulParam2)
{
	tRectangle     sPosition;
	tContext       sCtx;
	long           lValue;
	char           pcString[17];


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
			this->DebugSystemRender(&sCtx);
			this->DebugDebugRender(&sCtx);
			this->DebugNetRender(&sCtx);


			for( uint8_t i = 0; i < usBtnNum; i++ )
			{

				if( this->pButtons[i]->ulStyle & RB_STYLE_PRESSED )
				{
					this->pButtons[i]->ulOutlineColor = ClrGray;
				}
				else
				{
					if( i > 2 )
					{
						this->pButtons[i]->ulOutlineColor = ClrLime;
					}
					else
					{
						this->pButtons[i]->ulOutlineColor = TEXT_COLOR;
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
		case Button_Model:
			lValue  = UPSPropertyGet(SYSTEM_MODEL_MAJOR) * 100;
			lValue += UPSPropertyGet(SYSTEM_MODEL_MINOR);
			(new DialogNumericEditor(Dialog_Numeric_Editor_1, this, 80, 70))->Init(lValue, 2);

			break;

		case Button_Serial_1:
			lValue = UPSPropertyGet(SYSTEM_SERIAL_NUMBER_0);

	        if( lValue == 0 )
	        {
	            lValue = ('Q' << 8) | 'A';
	        }
			new DialogAdjustDouble(Dialog_Adjust, this, 80, 70, lValue, Short, Alphabetical);

			break;

		case Button_Serial_2:
			pcString[0] = UPSPropertyGet(SYSTEM_SERIAL_NUMBER_1) >> 8;
			pcString[1] = UPSPropertyGet(SYSTEM_SERIAL_NUMBER_1) & 0xFF;
			pcString[2] = UPSPropertyGet(SYSTEM_SERIAL_NUMBER_2) >> 8;
			pcString[3] = UPSPropertyGet(SYSTEM_SERIAL_NUMBER_2) & 0xFF;
			pcString[4] = '\0';

            lValue = ustrtoul(pcString, 0, 10);

			(new DialogNumericEditor(Dialog_Numeric_Editor_2, this, 80, 70))->Init(lValue, 0);

			break;

        case Button_Ip_1:
        case Button_Ip_2:
        case Button_Ip_3:
        case Button_Ip_4:
            lValue = ( ip_modified >> ( ( 3 - (ulParam1 - Button_Ip_Begin)) * 8) ) & 0xFF;

            (new DialogNumericEditor((ulParam1 - Button_Ip_Begin + Ip_Numeric_Editor_Begin), this, 80, 70))->Init(lValue, 0);

            break;

        case Button_Ip_Undo:
            ip_modified = net_ip_address_get();
            break;

        case Button_Ip_Apply:
            usprintf(pcString, "%d.%d.%d.%d", (ip_modified >> 24) & 0xff,
                                              (ip_modified >> 16) & 0xff,
                                              (ip_modified >>  8) & 0xff,
                                              (ip_modified      ) & 0xff);
            (new WindowConfirm(Dialog_Ip_Apply, this, 20, 40))->Init(
                                false,
                                (char *)"Change IP address to",
                                pcString);
            break;

#if ALLOW_MAC_SETTING == 1
        case Button_Mac_1:
        case Button_Mac_2:
        case Button_Mac_3:
        case Button_Mac_4:
        case Button_Mac_5:
        case Button_Mac_6:
            lValue = mac_modified[ulParam1 - Button_Mac_Begin];

            new DialogAdjustDouble((ulParam1 - Button_Mac_Begin + Dialog_Adjust_Mac_1), this, 80, 70, lValue, Byte, Hexadecimal);

            break;

        case Button_Mac_Undo:
            memcpy(mac_modified, net_mac_address_get(), 6);
            break;

        case Button_Mac_Apply:
            usprintf(pcString, "%02x.%02x.%02x.%02x.%02x.%02x",
                              mac_modified[0], mac_modified[1],
                              mac_modified[2], mac_modified[3],
                              mac_modified[4], mac_modified[5]);

            (new WindowConfirm(Dialog_Mac_Apply, this, 20, 40))->Init(
                                true,
                                (char *)"Change Mac address to",
                                pcString);
            break;
#endif

		case Button_Adjustments_Reset:

			(new WindowConfirm(Dialog_Adjustments_Reset, this, 20, 40))->Init(
								false,
								(char *)"Reset Adjustments",
								(char *)"Are you Sure?");

			break;

		case Button_Calibrations_Reset:

			(new WindowConfirm(Dialog_Calibrations_Reset, this, 20, 40))->Init(
								true,
								(char *)"Reset Calibrations",
								(char *)"Are you Sure?");

			break;

		case Button_Factory_Reset:

			(new WindowConfirm(Dialog_Factory_Reset, this, 20, 40))->Init(
								true,
								(char *)"Perform a factory reset",
								(char *)"Are you Sure?");
			break;

		}

		break;


	case WIDGET_NOTIFY_APPLY:
		switch( ulParam1 )
		{
		case Dialog_Numeric_Editor_1:
			lValue = ((DialogNumericEditor *)ulParam2)->ValueGet();

			UPSPropertySet(SYSTEM_MODEL_MAJOR, lValue / 100);
			UPSPropertySet(SYSTEM_MODEL_MINOR, lValue % 100);

			((DialogNumericEditor *)ulParam2)->Destroy();

			break;

		case Dialog_Numeric_Editor_2:
			lValue = ((DialogNumericEditor *)ulParam2)->ValueGet();

			usprintf(pcString, "%04d", lValue);

			lValue  = (unsigned long)pcString[0] << 8;
			lValue |= (unsigned long)pcString[1] << 0;

			UPSPropertySet(SYSTEM_SERIAL_NUMBER_1, lValue);

			lValue  = (unsigned long)pcString[2] << 8;
			lValue |= (unsigned long)pcString[3] << 0;

			UPSPropertySet(SYSTEM_SERIAL_NUMBER_2, lValue);

			((DialogNumericEditor *)ulParam2)->Destroy();

			break;

		case Dialog_Adjust:
			lValue = ((DialogAdjustDouble *)ulParam2)->ValueGet(Whole);

			UPSPropertySet(SYSTEM_SERIAL_NUMBER_0, lValue);

			((DialogAdjustDouble *)ulParam2)->Destroy();

        case Dialog_Numeric_Editor_Ip_1:
        case Dialog_Numeric_Editor_Ip_2:
        case Dialog_Numeric_Editor_Ip_3:
        case Dialog_Numeric_Editor_Ip_4:
            lValue = (((DialogNumericEditor *)ulParam2)->ValueGet()) & 0xff;
            ip_modified &= ~(0xFF << ( (3 - (ulParam1 - Ip_Numeric_Editor_Begin)) * 8) );
            ip_modified |= ((lValue << (3 - (ulParam1 - Ip_Numeric_Editor_Begin)) * 8) );

            ((DialogNumericEditor *)ulParam2)->Destroy();
            break;

        case Dialog_Ip_Apply:
            net_ip_address_set(ip_modified);

            ((WindowConfirm *)ulParam2)->Destroy();
            break;

#if ALLOW_MAC_SETTING == 1
        case Dialog_Adjust_Mac_1:
        case Dialog_Adjust_Mac_2:
        case Dialog_Adjust_Mac_3:
        case Dialog_Adjust_Mac_4:
        case Dialog_Adjust_Mac_5:
        case Dialog_Adjust_Mac_6:
            lValue = ((DialogAdjustDouble *)ulParam2)->ValueGet(Whole);

            mac_modified[ulParam1 - Mac_Numeric_Editor_Begin] = lValue & 0xff;
            ((DialogAdjustDouble *)ulParam2)->Destroy();
            break;

        case Dialog_Mac_Apply:
            net_mac_address_set(mac_modified);

            ((WindowConfirm *)ulParam2)->Destroy();
            break;
#endif

		case Dialog_Adjustments_Reset:
			//add adjustments reset here


			((WindowConfirm *)ulParam2)->Destroy();
			break;

		case Dialog_Calibrations_Reset:
			//add calibrations reset here


			((WindowConfirm *)ulParam2)->Destroy();
			break;

		case Dialog_Factory_Reset:
			//add factory reset here


			((WindowConfirm *)ulParam2)->Destroy();
			break;
		}

		break;

	case WIDGET_NOTIFY_CHANGE:
		switch( ulParam1 )
		{
		case Dialog_Adjust:
//		    lValue = ((DialogAdjustDouble *)ulParam2)->ValueGet(Whole);

//			UPSPropertyValidate(SYSTEM_SERIAL_NUMBER_0, &usValue);

//			((DialogAdjustDouble *)ulParam2)->Init(lValue, Short, Alphabetical);

			break;
		}

		break;

	case WIDGET_NOTIFY_CANCEL:
		switch( ulParam1 )
		{
		case Dialog_Numeric_Editor_1:
		case Dialog_Numeric_Editor_2:

		case Dialog_Numeric_Editor_Ip_1:
		case Dialog_Numeric_Editor_Ip_2:
		case Dialog_Numeric_Editor_Ip_3:
		case Dialog_Numeric_Editor_Ip_4:

#if ALLOW_MAC_SETTING == 1
		case Dialog_Adjust_Mac_1:
		case Dialog_Adjust_Mac_2:
		case Dialog_Adjust_Mac_3:
		case Dialog_Adjust_Mac_4:
		case Dialog_Adjust_Mac_5:
		case Dialog_Adjust_Mac_6:
#endif

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
DebugSystem::~DebugSystem(void)
{
	delete[] this->pButtons;
}
