/*
 * window_error_message.cpp
 *
 *  Created on: 7 нояб. 2018 г.
 *      Author: V.Striukov
 */
#include "FreeRTOS.h"
#include "task.h"


#include "interface.h"

#include "property/input_property.h"
#include "property/battery_property.h"
#include "property/inverter_property.h"
#include "property/dc_output_property.h"
#include "property/ac_output_property.h"
#include "property/ups_property.h"

#include "window_error_message.h"

#define PICTURE_COUNT                (  2 )
#define FRAMES_PER_SECOND            (  1 )
#define ANIMATION_FPS                ( 1000 / FRAMES_PER_SECOND )


//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************


typedef struct {
	const unsigned char * frame[2];
} t_icon;

static t_icon icon;
//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define WINDOW_COLOR                0x005c7b99
#define BUTTON_COLOR                0x00447099
#define DISP_COLOR                  0x00bcc7aa
#define TEXT_COLOR                  0x00F0F0F0
//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
	Canvas_Main,
	Window_Title,

	Canvas_Error_Message_Box,

	Button_Got_It,

	Widget_Max = 0xFF,
};

//*****************************************************************************
//
// Construcor of this window
//
//*****************************************************************************
WindowErrorMessage::WindowErrorMessage(unsigned char ucID, Widget *pParent)
						:Widget(ucID, pParent, 0, 0, 240, 320)
{
	Canvas            *pCanvas;
	WindowTitle       *pTitle;
	RectangularButton *pButton;


    //
    // Mapping icon frames
    //
	icon.frame[0] = g_pucIconConnectionError_1_Large;
	icon.frame[1] = g_pucIconConnectionError_2_Large;


    //
    // Main Canvas (used similar to window_main.cpp)
    //
	pCanvas = new Canvas(Canvas_Main, this, 0,  0, 240, 320);
	pCanvas->ulStyle = CANVAS_STYLE_FILL | CANVAS_STYLE_FALLING_EDGE;
	pCanvas->ulFillColor = WINDOW_COLOR;


	//
	// Title Canvas (used similar to window_main.cpp)
	//
	pTitle = new WindowTitle(Window_Title, this);
	pTitle->TitleSet((char *)"UPS 48V-75A-30A");

    //
    // Error Message Canvas
    //
	pCanvas = new Canvas(Canvas_Error_Message_Box, this, 2, 27, 236, 239);
	pCanvas->ulStyle = CANVAS_STYLE_FILL
					| CANVAS_STYLE_OUTLINE
					| CANVAS_STYLE_RISING_EDGE
					| CANVAS_STYLE_PAINT_NOTIFY;
	pCanvas->ulFillColor = ClrBlack;
	pCanvas->ulOutlineColor = ClrRed;

    //
    // The Got It Button
    //
	pButton = new RectangularButton(Button_Got_It, this, 20, 272, 200, 40);
	pButton->ulStyle = RB_STYLE_FILL         |
			           RB_STYLE_FALLING_EDGE |
			           RB_STYLE_TEXT;
	pButton->ulFillColor = BUTTON_COLOR;
	pButton->ulPressFillColor = BUTTON_COLOR;
	pButton->ulOutlineColor = ClrBlack;
	pButton->ulTextColor = TEXT_COLOR;
	pButton->pFont = g_pFontWDoslike15pt;
	pButton->pcText = (char *)"Ok, Got It";
}


void
WindowErrorMessage::MessageBoxRender(tContext *sCtx)
{
	long          lX;
	long          lY;
	char          cTitle[40];
	TickType_t    xCurrentTime;
	static short         sCurrentPic = 0;
	static TickType_t  xAnimationTimestamp;

    //
    // Animation logic goes here
    //
    xCurrentTime = xTaskGetTickCount();
    if( xCurrentTime - xAnimationTimestamp >= ( ANIMATION_FPS ) )
    {
        xAnimationTimestamp = xCurrentTime;

        if( ++sCurrentPic >= PICTURE_COUNT )
        {
            sCurrentPic = 0;
        }
    }


    //
    // Header Text
    //
	GrContextFontSet(sCtx, g_pFontWTimes25pt);

	usprintf(cTitle, "!! ATTENTION !!");
	GrContextForegroundSet(sCtx, ClrLime);

	lX = (sCtx->sClipRegion.sXMax - sCtx->sClipRegion.sXMin + 1) / 2;
	lY = 27;

	GrStringDrawCenteredL(sCtx, cTitle, -1, sCtx->sClipRegion.sXMin + lX,
			                                sCtx->sClipRegion.sYMin + lY, 0);


    //
    // Animated Picture
    //

    lY += 53;

	//**************************************************************************
	// As grlib uses a bit weird logic of calculating background transparent
	// color, current version of code requires additional calculations
	// (workaround) to get set background color to transparent
	//
	// value of the following would be different for each given picture,
	// and for now can only be get by guessing , so don't copy or use again
	// the codeline given below
	//**************************************************************************

	short sTransparencyColor = sCurrentPic + 2;

	//**************************************************************************
	// End of workaround
	//**************************************************************************

	GrTransparentImageDraw(sCtx, icon.frame[sCurrentPic],
	                       sCtx->sClipRegion.sXMin + lX - 35,
	                       sCtx->sClipRegion.sYMin + lY,
	                       sTransparencyColor);

    //
    // The picture followup text
    //

	GrContextFontSet(sCtx, g_pFontWDoslike15pt);

	usprintf(cTitle, "Connection LOST");
	lY += 100;

	GrStringDrawCenteredL(sCtx, cTitle, -1, sCtx->sClipRegion.sXMin + lX,
			                                sCtx->sClipRegion.sYMin + lY, 0);

}


long
WindowErrorMessage::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                  unsigned long ulParam2)
{
	tRectangle sPosition;
	tContext   sCtx;

    switch( ulMsg )
    {
    case WIDGET_NOTIFY_PAINT:
    	((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&sCtx);

        GrContextClipRegionSet(&sCtx, &sPosition);

		switch( ulParam1 )
		{
		case Canvas_Error_Message_Box:
			this->MessageBoxRender(&sCtx);
			break;

		default:
			return 0;
		}

    	break;

	case WIDGET_NOTIFY_BTN_UNPRESSED:
		switch( ulParam1 )
		{
		}

		break;

	case WIDGET_NOTIFY_APPLY:
		switch( ulParam1 )
		{
		}

		break;

	case WIDGET_NOTIFY_CANCEL:
		switch( ulParam1 )
		{
		}

		break;

	default:
		return 0;
    }

    return 1;
}


WindowErrorMessage::~WindowErrorMessage(void)
{

}
