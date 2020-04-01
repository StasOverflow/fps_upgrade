/*
 * confirm_window.cpp
 *
 *  Created on: 1 нояб. 2018 г.
 *      Author: V.Striukov
 */


#include "interface.h"

#include "window_confirm.h"

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define WINDOW_COLOR  				0x007DBABA
#define BUTTON_COLOR                0x00447099
#define TEXT_COLOR					0x00F0F0F0

//*****************************************************************************
//
// The string buffer length
//
//*****************************************************************************
#define TEXT_LENGTH_MAX             ( 16 )

//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
	Canvas_Main,
	Canvas_Border,
	Canvas_Text,

	Button_Begin,
	Button_Apply = Button_Begin,
	Button_Cancel,
	Button_End,

	Widget_Max = 0xFF
};

enum
{
	Warning,
	Alarm
};

static const unsigned short usBtnNum = Button_End - Button_Begin;


void
WindowConfirm::ConfirmTextRender(tContext *sCtx)
{
	long          lX;
	long          lY;
	Canvas	      *pCanvas;
	char          cTitle[15];


	pCanvas = new Canvas(Canvas_Border, this, 1, 1, 196, 156);


	//
	// Text canvas border
	//
	pCanvas->ulStyle = CANVAS_STYLE_RISING_EDGE;


	GrContextFontSet(sCtx, g_pFontWDoslike15pt);

	lX = (sCtx->sClipRegion.sXMax - sCtx->sClipRegion.sXMin + 1) / 2;
	lY = 17;

    if( this->alarmType == Alarm )
    {
        GrContextForegroundSet(sCtx, ClrRed);
        pCanvas->ulFillColor = ClrRed;
        usprintf(cTitle, "ALERT");
    }
    else
    if( this->alarmType == Warning )
    {
        GrContextForegroundSet(sCtx, ClrYellow);
        pCanvas->ulFillColor = ClrYellow;
        usprintf(cTitle, "WARNING");
    }

	GrStringDrawCenteredL(sCtx, cTitle, -1, sCtx->sClipRegion.sXMin + lX,
			                                            sCtx->sClipRegion.sYMin + lY, 0);


	GrLineDrawH(sCtx,  sCtx->sClipRegion.sXMin + 4,
			           sCtx->sClipRegion.sXMin + 195,
			           sCtx->sClipRegion.sYMin + 30);
	GrLineDrawH(sCtx,  sCtx->sClipRegion.sXMin + 4,
			           sCtx->sClipRegion.sXMin + 195,
			           sCtx->sClipRegion.sYMin + 31);

    GrContextForegroundSet(sCtx, TEXT_COLOR);

	lY += 27;
	GrStringDrawCenteredL(sCtx, "You are about to", -1, sCtx->sClipRegion.sXMin + lX,
			                                            sCtx->sClipRegion.sYMin + lY, 0);

	lY += 18;

	GrStringDrawCenteredL(sCtx, this->pcOutputAction, -1, sCtx->sClipRegion.sXMin + lX,
			                                              sCtx->sClipRegion.sYMin + lY, 0);
	lY += 23;

	GrStringDrawCenteredL(sCtx, this->pcOutputWarning, -1, sCtx->sClipRegion.sXMin + lX,
			                                              sCtx->sClipRegion.sYMin + lY, 0);


}


//*****************************************************************************
//
// Constructor
//
//*****************************************************************************

WindowConfirm::WindowConfirm(unsigned char ucID, Widget *pParent, long lX, long lY)
                   :Widget(ucID, pParent, lX, lY, 200, 160)
{
	Canvas				*pCanvas;
	short				btn = 0;


	this->pcOutputWarning = new char[TEXT_LENGTH_MAX + 1];
	this->pcOutputWarning[0] = '\0';


	this->pcOutputAction = new char[TEXT_LENGTH_MAX + 1];
	this->pcOutputAction[0] = '\0';


	//
	// Main canvas
	//
	pCanvas = new Canvas(Canvas_Main, this, 0, 0, 200, 160);
	pCanvas->ulStyle = CANVAS_STYLE_FILL
					 | CANVAS_STYLE_FALLING_EDGE
					 | CANVAS_STYLE_PAINT_NOTIFY;
	pCanvas->ulFillColor = BUTTON_COLOR;


	this->pButtons = new RectangularButton * [usBtnNum];

	this->pButtons[btn] = new RectangularButton(Button_Cancel, this, 10, 110, 85, 40);


	this->pButtons[btn]->ulStyle = RB_STYLE_FILL
								 | RB_STYLE_OUTLINE
								 | RB_STYLE_FALLING_EDGE
								 | RB_STYLE_TEXT;
    this->pButtons[btn]->ulOutlineColor = BUTTON_COLOR;
	this->pButtons[btn]->ulFillColor = BUTTON_COLOR;
	this->pButtons[btn]->ulPressFillColor = BUTTON_COLOR;
	this->pButtons[btn]->ulTextColor = TEXT_COLOR;
	this->pButtons[btn]->pFont = g_pFontWDoslike15pt;
    this->pButtons[btn]->pcText = (char *)"< Cancel";



	this->pButtons[btn] = new RectangularButton(Button_Apply, this, 105, 110, 85, 40);
	this->pButtons[btn]->ulStyle = RB_STYLE_FILL
								 | RB_STYLE_OUTLINE
								 | RB_STYLE_FALLING_EDGE
								 | RB_STYLE_TEXT;
    this->pButtons[btn]->ulOutlineColor = BUTTON_COLOR;
	this->pButtons[btn]->ulFillColor = BUTTON_COLOR;
	this->pButtons[btn]->ulPressFillColor = BUTTON_COLOR;
	this->pButtons[btn]->ulTextColor = TEXT_COLOR;
	this->pButtons[btn]->pFont = g_pFontWDoslike15pt;
    this->pButtons[btn++]->pcText = (char *)"Apply";

}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
WindowConfirm::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                        unsigned long ulParam2)
{
	tContext   sCtx;
	tRectangle sPosition;


    switch( ulMsg )
    {
    case WIDGET_MSG_PTR_DOWN:
    case WIDGET_MSG_PTR_MOVE:
    case WIDGET_MSG_PTR_UP:
    	//
    	// Haldle all pointer messages to prevent
    	// pressing buttons outside this window
    	//
    	break;

    case WIDGET_NOTIFY_PAINT:
    	((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&sCtx);

        GrContextClipRegionSet(&sCtx, &sPosition);

    	switch( ulParam1 )
    	{
    	case Canvas_Main:
    		this->ConfirmTextRender(&sCtx);

			for( short i = 0; i < usBtnNum; i++ )
			{
			    /* Only Apply button should be colorized */
				if( this->pButtons[0]->ulStyle & RB_STYLE_PRESSED )
				{
				    if( this->alarmType == Alarm )
				    {
	                    this->pButtons[0]->ulOutlineColor = ClrOrangeRed;
                    }
				    else
                    if( this->alarmType == Warning )
				    {
                        this->pButtons[0]->ulOutlineColor = ClrYellow;
				    }
				}
				else
				{
					this->pButtons[0]->ulOutlineColor = BUTTON_COLOR;
				}
			}
    		break;

    	default:
    		return 0;
    	}

    case WIDGET_MSG_UPDATE:

    	break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
    	switch( ulParam1 )
    	{
		case Button_Apply:
			Widget::ParentNotify(this, WIDGET_NOTIFY_APPLY);
            this->Destroy();
			break;

		case Button_Cancel:
			Widget::ParentNotify(this, WIDGET_NOTIFY_CANCEL);
			this->Destroy();
			break;
    	}

    	break;

	case WIDGET_NOTIFY_BTN_PRESSED:
		switch( ulParam1 )
		{

		default:
			return 0;
		}

		break;

	default:
		return 0;
    }

    return 1;
}

void
WindowConfirm::Init( bool critical, char * pcTextAction, char * pcTextWarning )
{
	if( critical )
	{
		this->alarmType = Alarm;
	}
	else
	{
		this->alarmType = Warning;
	}

	usprintf(this->pcOutputWarning, "%s", pcTextWarning);
	usprintf(this->pcOutputAction, "%s", pcTextAction);
}
//*****************************************************************************
//
// Destructor
//
//*****************************************************************************
WindowConfirm::~WindowConfirm( void )
{

}

