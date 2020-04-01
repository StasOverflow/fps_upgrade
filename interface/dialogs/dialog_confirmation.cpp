
#include "interface.h"
#include "dialogs/dialog_confirmation.h"

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define WINDOW_COLOR                0x00285a89
#define BUTTON_LIGHT_COLOR          0x005c7b99
#define BUTTON_COLOR                WINDOW_COLOR
#define TEXT_COLOR                  0x00F0F0F0

//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
	Canvas_Title,
	Canvas_Main,
	Canvas_Message,

	Button_No,
    Button_Yes,

	Widget_Max = 0xFF
};

//*****************************************************************************
//
// Constructor
//
//*****************************************************************************
DialogConfirmation::DialogConfirmation(unsigned char ucID, Widget *pParent, long lX, long lY)
                   :Widget(ucID, pParent, lX, lY, 200, 120)
{
	RectangularButton *pButton;
	Canvas            *pCanvas;

	//
	// Main canvas
	//
	pCanvas = new Canvas(Canvas_Main, this, 0, 0, 200, 120);
	pCanvas->ulStyle = CANVAS_STYLE_FILL | CANVAS_STYLE_FALLING_EDGE;
	pCanvas->ulFillColor = WINDOW_COLOR;

	//
	// Title
	//
	pCanvas = new Canvas(Canvas_Title, this, 0, 0, 200, 26);
	pCanvas->ulStyle = CANVAS_STYLE_TEXT | CANVAS_STYLE_PAINT_NOTIFY;
	pCanvas->ulTextColor = TEXT_COLOR;
	pCanvas->pFont = g_pFontWDoslike15pt;
	pCanvas->pcText = (char *)"Confirm your action";

	//
	// Text canvas
	//
	pCanvas = new Canvas(Canvas_Message, this, 10, 35, 180, 26);
	pCanvas->ulStyle = CANVAS_STYLE_TEXT;
	pCanvas->ulTextColor = TEXT_COLOR;
	pCanvas->pFont = g_pFontWDoslike15pt;
	pCanvas->pcText = (char *)"Are you sure?";

	//
	// Button NO
	//
    pButton = new RectangularButton(Button_No, this,  10, 70, 80, 40);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = BUTTON_COLOR;
    pButton->ulPressFillColor = BUTTON_COLOR;
    pButton->ulTextColor = TEXT_COLOR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)"NO";

	//
	// Button Yes
	//
    pButton  = new RectangularButton(Button_Yes, this, 110, 70, 80, 40);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_OUTLINE      |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = BUTTON_COLOR;
    pButton->ulPressFillColor = BUTTON_COLOR;
//    pButton->ulOutlineColor = BUTTON_LIGHT_COLOR;
    pButton->ulOutlineColor = ClrWhite;
    pButton->ulTextColor = TEXT_COLOR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText  = (char *)"YES";
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DialogConfirmation::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                        unsigned long ulParam2)
{
	tContext   sCtx;
	tRectangle sPosition;
	long       lColor;

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

    case WIDGET_NOTIFY_DESTROY:
    	break;

    case WIDGET_NOTIFY_PAINT:
    	((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&sCtx);

        GrContextClipRegionSet(&sCtx, &sPosition);

    	switch( ulParam1 )
    	{
    	case Canvas_Title:
    		lColor = WINDOW_COLOR;
    		ColorValueAdjust(&lColor, -30);
    		GrContextForegroundSet(&sCtx, lColor);
    		GrLineDrawH(&sCtx, sCtx.sClipRegion.sXMin + 4,
    				           sCtx.sClipRegion.sXMin + 195,
    				           sCtx.sClipRegion.sYMin + 24);

    		lColor = WINDOW_COLOR;
    		ColorValueAdjust(&lColor, 30);
    		GrContextForegroundSet(&sCtx, lColor);
    		GrLineDrawH(&sCtx, sCtx.sClipRegion.sXMin + 4,
    				           sCtx.sClipRegion.sXMin + 195,
    				           sCtx.sClipRegion.sYMin + 25);

    		break;

    	default:
    		return 0;
    	}

    	break;

	case WIDGET_NOTIFY_UPDATE:
		break;

	case WIDGET_NOTIFY_BTN_UNPRESSED:
		switch( ulParam1 )
		{
		case Button_Yes:
			Widget::ParentNotify(this, WIDGET_NOTIFY_APPLY);
			break;

		case Button_No:
			Widget::ParentNotify(this, WIDGET_NOTIFY_CANCEL);
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
// Destructor
//
//*****************************************************************************
DialogConfirmation::~DialogConfirmation( void )
{

}
