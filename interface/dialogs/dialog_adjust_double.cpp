
#include "interface.h"
#include "dialogs/dialog_numeric_editor.h"
#include "dialogs/dialog_adjust_double.h"

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

	Button_Minus,
	Button_Apply,
	Button_Plus,

	Button_Minus_Higher,
	Button_Plus_Higher,

	Button_Cancel,
	Button_Exit,

#if VERY_SPECIAL_DEBUG_BUTTON == 1
	Button_Set,
#endif

	Widget_Max = 0xFF,
};


typedef struct {
    unsigned char lower;
    unsigned char higher;
} boundaries;

boundaries lim[BoundaryCount];


//*****************************************************************************
//
// Constructor
//
//*****************************************************************************
DialogAdjustDouble::DialogAdjustDouble(unsigned char ucID, Widget *pParent,
                                       long lX, long lY, unsigned short usValue,
                                       bit_shift_t digit_max_size,
                                       boundary_t bounds)
                   :Widget(ucID, pParent, lX, lY, 98, 200)
{
	RectangularButton *pButton;
	Canvas            *pCanvas;

	long              lButtonX;
	long              lButtonY;

	long			  buttoHeight;
    unsigned short    usAndValue;

	const static char *pButtonNames[] =
	{
		"<",  "OK", ">", "<", ">", "C", "X",
#if VERY_SPECIAL_DEBUG_BUTTON == 1
		"set",
#endif
	};


	lim[Alphabetical].lower  = 'A';
	lim[Alphabetical].higher = 'Z';

    lim[Hexadecimal].lower  = 0;
    lim[Hexadecimal].higher = 0xf;

    lim[Decimal].lower  = 0;
    lim[Decimal].higher = 9;

    lim[Octal].lower  = 1;
    lim[Octal].higher = 7;

    lim[Binary].lower  = 0;
    lim[Binary].higher = 1;

    //
    // Set initial values
    //

    this->pcText = new char[TEXT_LENGTH_MAX + 1];

    this->xDigitMaxSize   = digit_max_size;
    this->xBounds         = bounds;

    usAndValue            = (1 << this->xDigitMaxSize) - 1;
    this->usValueInitial  = usValue;
    this->usValue         = this->usValueInitial;
    this->ValueModify(this->usValue, 0);
    this->ucValueHigher   = (this->usValue >> this->xDigitMaxSize) & usAndValue;
    this->ucValueLower    = this->usValue & usAndValue;

    this->DisplayableValuesUpdate();
    //
	// Main canvas
	//
	pCanvas = new Canvas(Canvas_Main, this, 0, 0, 98,
#if VERY_SPECIAL_DEBUG_BUTTON == 1
	        200);
#else
	        87);
#endif
	pCanvas->ulStyle = CANVAS_STYLE_FILL
	                 | CANVAS_STYLE_FALLING_EDGE
	                 | CANVAS_STYLE_PAINT_NOTIFY;
	pCanvas->ulFillColor = WINDOW_COLOR;

	//
	// Text canvas border
    //
	pCanvas = new Canvas(Canvas_Border, this, 2, 2, 94, 19);
	pCanvas->ulStyle = CANVAS_STYLE_RISING_EDGE;
	pCanvas->ulFillColor = WINDOW_COLOR;

    //
    // Text canvas
    //
    this->pxCanvasText = new Canvas(Canvas_Text, this, 4, 4, 90, 15);
    this->pxCanvasText->ulStyle = CANVAS_STYLE_FILL
                                | CANVAS_STYLE_TEXT;
    this->pxCanvasText->ulFillColor = DISP_COLOR;
    this->pxCanvasText->ulTextColor = ClrBlack;
    this->pxCanvasText->pFont = g_pFontWDoslike15pt;
	//
	// Buttons
	//
	lButtonX = 4;
	lButtonY = 23;

	for( int i = Button_Minus, iButton = 0; i <= Button_Plus_Higher; i++, iButton++ )
	{
		if( (iButton) && (!(iButton % 3)) )
		{
			lButtonX  = 4;
			lButtonY += 20;
		}
		buttoHeight = 20;
		if( iButton == 1 )
		{
			buttoHeight += 20;
		}
		pButton = new RectangularButton(i, this,  lButtonX, lButtonY, 30, buttoHeight);
		pButton->ulStyle = RB_STYLE_FILL         |
						   RB_STYLE_FALLING_EDGE |
						   RB_STYLE_TEXT;

		if( (i == Button_Minus)  || (i == Button_Plus)
		 || (i == Button_Minus_Higher) || (i == Button_Plus_Higher) )
		{
			pButton->ulStyle |= RB_STYLE_AUTO_REPEAT;
			pButton->usAutoRepeatDelay = 700;
			pButton->usAutoRepeatRate = 100;
		}

		pButton->ulFillColor = BUTTON_COLOR;
		pButton->ulPressFillColor = BUTTON_COLOR;
		pButton->ulTextColor = TEXT_COLOR;
		pButton->pFont = g_pFontWDoslike15pt;
		pButton->pcText = (char *)pButtonNames[iButton];

		if( iButton == 3 )
		{
			lButtonX += 30;	//add some space between second line's buttons
		}
		lButtonX += 30;
	}

	//
	// Button Cancel
	//
	pButton = new RectangularButton(Button_Cancel, this,  4, lButtonY + 20, 45, buttoHeight);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = BUTTON_COLOR;
    pButton->ulPressFillColor = BUTTON_COLOR;
    pButton->ulTextColor = TEXT_COLOR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)pButtonNames[Button_Cancel - 3];

    //
    //Button Exit
    //
    pButton = new RectangularButton(Button_Exit, this,  4 + 45, lButtonY + 20, 45, buttoHeight);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = BUTTON_COLOR;
    pButton->ulPressFillColor = BUTTON_COLOR;
    pButton->ulTextColor = TEXT_COLOR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)pButtonNames[Button_Exit - 3];

#if VERY_SPECIAL_DEBUG_BUTTON == 1
    //
    //Button Set
    //
    pButton = new RectangularButton(Button_Set, this,  4, lButtonY + 40, 90, buttoHeight);
    pButton->ulStyle = RB_STYLE_FILL         |
                       RB_STYLE_FALLING_EDGE |
                       RB_STYLE_TEXT;
    pButton->ulFillColor = BUTTON_COLOR;
    pButton->ulPressFillColor = BUTTON_COLOR;
    pButton->ulTextColor = TEXT_COLOR;
    pButton->pFont = g_pFontWDoslike15pt;
    pButton->pcText = (char *)pButtonNames[Button_Set - 3];
#endif
}


//*****************************************************************************
//
// Very Special Debug Function to Output current state of given values
//
//*****************************************************************************
#if VERY_SPECIAL_DEBUG_BUTTON == 1
void
DialogAdjustDouble::DebugInfoRender(tContext *pContext)
{
    char                pcString[17];
    char                ucTextFormat[5];
    unsigned short      usBtm;
    unsigned short      usTop;

    ucTextFormat[0] = '%';
    if( this->xBounds == Alphabetical )
    {
        ucTextFormat[1] = 'c';
    }
    else
    if( this->xBounds == Hexadecimal )
    {
        ucTextFormat[1] = 'x';
    }
    else
    if( this->xBounds >= Decimal )
    {
        ucTextFormat[1] = 'd';
    }
    GrContextFontSet(pContext, g_pFontWTahoma13pt);

    GrContextForegroundSet(pContext, ClrWhite);

    usprintf(pcString, "high:");
    usprintf(pcString + strlen(pcString), ucTextFormat, this->ucValueHigher);
    GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + 10,
                                         pContext->sClipRegion.sYMin + 110, 0);

    usprintf(pcString, "low:");
    usprintf(pcString + strlen(pcString), ucTextFormat, this->ucValueLower);
    GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + 55,
                                         pContext->sClipRegion.sYMin + 110, 0);

    usTop = (lim[this->xBounds].higher << this->xDigitMaxSize) | lim[this->xBounds].higher;
    usBtm = (lim[this->xBounds].lower << this->xDigitMaxSize) | lim[this->xBounds].lower;

    usprintf(pcString, "Top: ");
    usprintf(pcString + strlen(pcString), ucTextFormat, usTop);
    GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + 10,
                                         pContext->sClipRegion.sYMin + 130, 0);

    usprintf(pcString, "Btm: ");
    usprintf(pcString + strlen(pcString), ucTextFormat, usBtm);
    GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + 55,
                                         pContext->sClipRegion.sYMin + 130, 0);


    usprintf(pcString, "total:");
    if( this->xBounds == Alphabetical )
    {
        usprintf(pcString + strlen(pcString), ucTextFormat, this->ucValueHigher);
        usprintf(pcString + strlen(pcString), ucTextFormat, this->ucValueLower);
    }
    else
    {
        usprintf(pcString + strlen(pcString), ucTextFormat, this->usValue);
    }
    GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + 25,
                                         pContext->sClipRegion.sYMin + 150, 0);


    usprintf(pcString, "initial:");
    if( this->xBounds == Alphabetical )
    {
        usprintf(pcString + strlen(pcString), ucTextFormat, this->usValue >> this->xBounds);
        usprintf(pcString + strlen(pcString), ucTextFormat, this->usValue & ((1 << this->xBounds) - 1));
    }
    else
    {
        usprintf(pcString + strlen(pcString), ucTextFormat, this->usValueInitial);
    }
    GrStringDraw(pContext, pcString, -1, pContext->sClipRegion.sXMin + 25,
                                         pContext->sClipRegion.sYMin + 170, 0);

}
#endif
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DialogAdjustDouble::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                        unsigned long ulParam2)
{
    tRectangle     sPosition;
    tContext       sCtx;

    switch( ulMsg )
    {
    case WIDGET_NOTIFY_PAINT:
        ((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&sCtx);

        GrContextClipRegionSet(&sCtx, &sPosition);
        switch( ulParam1 )
        {
        case Canvas_Main:
            this->DisplayableValuesUpdate();
            this->pxCanvasText->pcText = this->pcText;
#if VERY_SPECIAL_DEBUG_BUTTON == 1
            this->DebugInfoRender(&sCtx);
#endif
            break;
        }
        break;

    case WIDGET_MSG_PTR_DOWN:
    case WIDGET_MSG_PTR_MOVE:
    case WIDGET_MSG_PTR_UP:
    	//
    	// Haldle all pointer messages to prevent
    	// pressing buttons outside this window
    	//
    	break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
    	switch( ulParam1 )
    	{
		case Button_Apply:
			Widget::ParentNotify(this, WIDGET_NOTIFY_APPLY);
			break;

        case Button_Cancel:
            //make value as initial
            this->ucValueHigher = this->usValue >> this->xDigitMaxSize;
            this->ucValueLower  = this->usValue & ((1 << this->xDigitMaxSize) - 1);
            break;

        case Button_Exit:
            //call destrutor
            this->Destroy();
            break;

#if VERY_SPECIAL_DEBUG_BUTTON == 1
        case Button_Set:
            this->usValue  = this->ucValueHigher << this->xDigitMaxSize;
            this->usValue |= this->ucValueLower & ((1 << this->xDigitMaxSize) - 1);
            break;
#endif
    	}

    	break;

	case WIDGET_NOTIFY_BTN_PRESSED:
		switch( ulParam1 )
		{
		case Button_Minus:
		    this->ValueModify(this->ucValueHigher, '-');

			break;

		case Button_Plus:
            this->ValueModify(this->ucValueHigher, '+');

			break;

		case Button_Minus_Higher:
            this->ValueModify(this->ucValueLower, '-');

			break;

		case Button_Plus_Higher:
            this->ValueModify(this->ucValueLower, '+');

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
// Private function to render operational values in a correct format
//
//*****************************************************************************
void
DialogAdjustDouble::DisplayableValuesUpdate(void)
{
    if( this->xBounds == Alphabetical )
    {
        usprintf(this->pcText, "%c%c", this->ucValueHigher, this->ucValueLower);
    }
    else
    if( this->xBounds == Hexadecimal )
    {
        usprintf(this->pcText, "%x%x", this->ucValueHigher, this->ucValueLower);
    }
    else
    if( this->xBounds >= Decimal )
    {
        usprintf(this->pcText, "%d%d", this->ucValueHigher, this->ucValueLower);
    }
}
//*****************************************************************************
//
// Private function to update given value (either lower or higher)
//
//*****************************************************************************
void
DialogAdjustDouble::ValueModify(unsigned long & ulrValue, char cSign)
{
    unsigned long  ulValue = ulrValue;
    unsigned long  usCalc;


    if( (cSign == '0') || (cSign == 0) )
    {
        //
        // we us this case only on initialization to modify given Whole
        // value to match our boundaries
        //

        usCalc = (lim[this->xBounds].lower << this->xDigitMaxSize) | lim[this->xBounds].lower;

        if( ulValue < usCalc  )
        {
            ulValue = 0;
        }

        usCalc = (lim[this->xBounds].higher << this->xDigitMaxSize) | lim[this->xBounds].higher;

        if( ulValue > usCalc )
        {
            ulValue = usCalc;
        }
    }
    else
    {
        if( cSign == '-')
        {
            if( ulValue <= lim[this->xBounds].lower )
            {
                ulValue = lim[this->xBounds].lower;
            }
            else
            {
                ulValue--;
            }
        }
        else
        if( cSign == '+')
        {
            if( ulValue >= lim[this->xBounds].higher )
            {
                ulValue = lim[this->xBounds].higher;
            }
            else
            {
                ulValue++;
            }
        }
    }

    ulrValue = ulValue;
}
//*****************************************************************************
//
// Getter
//
//*****************************************************************************
short
DialogAdjustDouble::ValueGet(val_t value)
{
    unsigned short usReturnValue;

    switch (value)
    {
    case Whole:
        this->usValue  = ucValueHigher << this->xDigitMaxSize;
        this->usValue |= (ucValueLower & ((1 << this->xDigitMaxSize) - 1));
        usReturnValue = this->usValue;
        break;
    case Higher:
        usReturnValue = this->ucValueLower;
        break;
    case Lower:
        usReturnValue = this->ucValueHigher;
        break;
    default:
        usReturnValue = this->usValue;
        break;
    }
    return usReturnValue;
}
//*****************************************************************************
//
// Destructor
//
//*****************************************************************************
DialogAdjustDouble::~DialogAdjustDouble( void )
{

}
