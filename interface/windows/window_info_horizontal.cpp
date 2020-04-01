
#include "interface.h"

#include "property/ups_property.h"

#include "window_info_horizontal.h"

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
// The string buffer length
//
//*****************************************************************************
#define TEXT_LENGTH_MAX             ( 17 )

//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
	Canvas_Main,
	Canvas_Header,
	Canvas_Border,

	Canvas_SWVersion,
	Canvas_Model,
	Canvas_Serial,

	Button_Back,
	Button_Service,

	Management_Unit_Canvas,

    Button_Secret_MU_Number_Reveal,

	Widget_Max = 0xFF
};

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
WindowInfo::WindowInfo(unsigned char ucID, Widget *pParent)
               :Widget(ucID, pParent, 0, 0, 320, 240)
{
	RectangularButton   *pButton;
	Canvas              *pCanvas;
	long                lY;

	const static char *pNames[] =
	{
		"SW Version:",
		"Model:",
		"Serial No:",
	};

	//
	// Set initial values
	//
	this->ucServiceButtonCnt = 0;
    this->secret_button_pressed_count = 0;

	/*
	 * Main Canvas
	 */
    pCanvas = new Canvas(Canvas_Main, this, 0, 0, 320, 240);
    pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;


    /*
     * Header Canvas
     */
    pCanvas = new Canvas(Canvas_Header, this, 0, 2, 320, 25);
    pCanvas->ulStyle = CANVAS_STYLE_TEXT;
    pCanvas->ulTextColor = TEXT_COLOR;
    pCanvas->pFont = g_pFontWTimes22pt;
    pCanvas->pcText = (char *)"Setup / Info";

	lY = 80;

	for( int i = Canvas_SWVersion, iCanvas = 0; i <= Canvas_Serial; i++, iCanvas++)
	{
		//
		// Name canvas
		//
		pCanvas = new Canvas(Widget_Max, this, 50, lY, 113, 15);
		pCanvas->ulStyle = CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_RIGHT;
		pCanvas->ulTextColor = TEXT_COLOR;
		pCanvas->pFont = g_pFontWDoslike15pt;
		pCanvas->pcText = (char *)pNames[iCanvas];

		//
		// Value canvas
		//
		pCanvas = new Canvas(i, this, 125+50, lY, 113, 15);
		pCanvas->ulStyle = CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_LEFT;
		pCanvas->ulTextColor = TEXT_COLOR;
		pCanvas->pFont = g_pFontWDoslike15pt;
		pCanvas->pcText = new char[TEXT_LENGTH_MAX + 1];
		pCanvas->pcText[0] = '\0';

		lY += 20;
	}

	//
	// Button Back
	//
	pButton = new RectangularButton(Button_Back, this, BUTTON_BACK_POSITION);
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
	// Button Service
	//
	pButton = new RectangularButton(Button_Service, this, 0, 0, 50, 50);

    pButton = new RectangularButton(Button_Secret_MU_Number_Reveal, this, 135, 190, 50, 50);

    this->Secret_Message = new Canvas(Management_Unit_Canvas, this, 260, 0, 60, 25);
    this->Secret_Message->ulStyle = CANVAS_STYLE_TEXT;
    this->Secret_Message->ulTextColor = ClrLime;
    this->Secret_Message->pFont = g_pFontWTahoma13pt;
    this->Secret_Message->pcText = (char *)"";
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
WindowInfo::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                unsigned long ulParam2)
{
    tRectangle          sPosition;
    tContext            sCtx;

	Canvas *pCanvas;

	pCanvas = (Canvas *)ulParam2;


    if( (GrTimerGet() - this->secret_button_timer) >= 2000 )
    {
        this->Secret_Message->pcText = (char *)"";
        this->Secret_Message->Draw();
    }

    switch( ulMsg )
    {
    case WIDGET_NOTIFY_PAINT:
        ((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&sCtx);
        GrContextClipRegionSet(&sCtx, &sPosition);

        switch( ulParam1 )
        {
        case Canvas_Main:
            GrImageDraw(&sCtx, g_pucimg_info, 0, 0);
            break;
        }

        break;

    case WIDGET_NOTIFY_DESTROY:
		switch( ulParam1 )
		{
		case Canvas_SWVersion:
		case Canvas_Model:
		case Canvas_Serial:
			delete[] pCanvas->pcText;
			break;

		default:
			return 0;
		}

    	break;

    case WIDGET_NOTIFY_UPDATE:
    	switch(ulParam1)
    	{
		case Canvas_SWVersion:
    		strcpy(pCanvas->pcText, SOFTWARE_VERSION);
			break;
		case Canvas_Model:
			if( UPSPropertyGet(SYSTEM_MODEL_MAJOR) )
			{
				strcpy(pCanvas->pcText, "U");

				UPSPropertyStringGet(SYSTEM_MODEL_MAJOR, pCanvas->pcText + strlen(pCanvas->pcText));

				if( UPSPropertyGet(SYSTEM_MODEL_MINOR) )
				{
					strcat(pCanvas->pcText, ".");
					UPSPropertyStringGet(SYSTEM_MODEL_MINOR, pCanvas->pcText + strlen(pCanvas->pcText));
				}
			}
			else
			{
				strcpy(pCanvas->pcText, "------");
			}

			break;

		case Canvas_Serial:
			if( (UPSPropertyGet(SYSTEM_SERIAL_NUMBER_0) != 0) &&
				(UPSPropertyGet(SYSTEM_SERIAL_NUMBER_1) != 0) &&
				(UPSPropertyGet(SYSTEM_SERIAL_NUMBER_2) != 0) )
			{
				UPSPropertyStringGet(SYSTEM_SERIAL_NUMBER_0, pCanvas->pcText);
                strcat(pCanvas->pcText, " ");
				UPSPropertyStringGet(SYSTEM_SERIAL_NUMBER_1, pCanvas->pcText + strlen(pCanvas->pcText));
				UPSPropertyStringGet(SYSTEM_SERIAL_NUMBER_2, pCanvas->pcText + strlen(pCanvas->pcText));
			}
			else
			{
				strcpy(pCanvas->pcText, "------");
			}

			break;

		default:
			return 0;
    	}

    	break;

	case WIDGET_NOTIFY_BTN_PRESSED:
		switch( ulParam1 )
		{
		case Button_Service:
			if( this->ucServiceButtonCnt > 0 )
			{
				if( (GrTimerGet() - this->ulServiceButtonTimer) >= 500 )
				{
					this->ucServiceButtonCnt = 0;
				}
			}

			this->ulServiceButtonTimer = GrTimerGet();

			if( ++this->ucServiceButtonCnt >= 3 )
			{
				WindowManager::MessagePost(WM_MSG_SHOW, Window_Debug);
			}

			break;

        case Button_Secret_MU_Number_Reveal:

            if( this->secret_button_pressed_count > 0 )
            {
                if( (GrTimerGet() - this->secret_button_timer) >= 1000 )
                {
                    this->secret_button_pressed_count = 0;
                }
            }

            this->secret_button_timer = GrTimerGet();

            if( ++this->secret_button_pressed_count >= 6 )
            {
                this->Secret_Message->pcText = (char *) MODULE_NAME;
                this->Secret_Message->Draw();
            }

            break;
		}

		break;

	case WIDGET_NOTIFY_BTN_UNPRESSED:
		switch( ulParam1 )
		{
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
WindowInfo::~WindowInfo(void)
{
    this->Secret_Message->Destroy();
}
