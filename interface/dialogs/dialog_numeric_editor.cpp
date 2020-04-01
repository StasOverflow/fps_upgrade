
#include "interface.h"
#include "dialogs/dialog_numeric_editor.h"

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
	Button_1,
	Button_2,
	Button_3,
	Button_4,
	Button_5,
	Button_6,
	Button_7,
	Button_8,
	Button_9,
	Button_Clear,
	Button_0,
	Button_Cancel,

	Widget_Max = 0xFF
};

//*****************************************************************************
//
// Constructor
//
//*****************************************************************************
DialogNumericEditor::DialogNumericEditor(unsigned char ucID, Widget *pParent, long lX, long lY)
                   :Widget(ucID, pParent, lX, lY, 98, 127)
{
	RectangularButton *pButton;
	Canvas            *pCanvas;
	long              lButtonX;
	long              lButtonY;

	const static char *pButtonNames[] =
	{
		"<",  "OK", ">",
		"1",  "2",  "3",
		"4",  "5",  "6",
		"7",  "8",  "9",
		"C",  "0",  "X",
	};

	//
	// Set initial values
	//
	this->lValue = 0;
	this->ucDotPos = 0;
	this->ucAnyNumberPressed = 0;
	this->pcText = new char[TEXT_LENGTH_MAX + 1];
	this->pcText[0] = '\0';

	//
	// Main canvas
	//
	pCanvas = new Canvas(Canvas_Main, this, 0, 0, 98, 127);
	pCanvas->ulStyle = CANVAS_STYLE_FILL | CANVAS_STYLE_FALLING_EDGE;
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
	pCanvas = new Canvas(Canvas_Text, this, 4, 4, 90, 15);
	pCanvas->ulStyle = CANVAS_STYLE_FILL | CANVAS_STYLE_TEXT;
	pCanvas->ulFillColor = DISP_COLOR;
	pCanvas->ulTextColor = ClrBlack;
	pCanvas->pFont = g_pFontWDoslike15pt;
	pCanvas->pcText = this->pcText;

	//
	// Buttons
	//
	lButtonX = 4;
	lButtonY = 23;

	for( int i = Button_Minus, iButton = 0; i <= Button_Cancel; i++, iButton++ )
	{
		if( (iButton) && (!(iButton % 3)) )
		{
			lButtonX  = 4;
			lButtonY += 20;
		}

		pButton = new RectangularButton(i, this,  lButtonX, lButtonY, 30, 20);
		pButton->ulStyle = RB_STYLE_FILL         |
						   RB_STYLE_FALLING_EDGE |
						   RB_STYLE_TEXT;

		if( (i == Button_Minus) || (i == Button_Plus) )
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

		lButtonX += 30;
	}
}

//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
DialogNumericEditor::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
		                        unsigned long ulParam2)
{
	char pcFmt[10];

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

    case WIDGET_MSG_UPDATE:
    	if( this->lValue >= 0 )
    	{
    		usprintf(pcFmt, "%%0%dd", this->ucDotPos + 1);
    	}
    	else
    	{
    		usprintf(pcFmt, "%%0%dd", this->ucDotPos + 1 + 1);
    	}

		usprintf(this->pcText, pcFmt, this->lValue);

		string_insert(this->pcText, (char *)".", strlen(this->pcText) - this->ucDotPos);

    	break;

    case WIDGET_NOTIFY_BTN_UNPRESSED:
    	switch( ulParam1 )
    	{
		case Button_Apply:
			Widget::ParentNotify(this, WIDGET_NOTIFY_APPLY);
			break;

		case Button_1:
		case Button_2:
		case Button_3:
		case Button_4:
		case Button_5:
		case Button_6:
		case Button_7:
		case Button_8:
		case Button_9:
			if( !ucAnyNumberPressed )
			{
				this->lValue = ulParam1 - Button_1 + 1;

				ucAnyNumberPressed = 1;
			}
			else
			{
				this->lValue *= 10;

				if( this->lValue >= 0 )
				{
					this->lValue += ulParam1 - Button_1 + 1;
				}
				else
				{
					this->lValue -= ulParam1 - Button_1 + 1;
				}
			}

			break;

		case Button_0:
			if( !ucAnyNumberPressed )
			{
				this->lValue = 0;

				ucAnyNumberPressed = 1;
			}
			else
			{
				this->lValue *= 10;
			}

			break;

		case Button_Clear:
			this->lValue = 0;
			break;

		case Button_Cancel:
			Widget::ParentNotify(this, WIDGET_NOTIFY_CANCEL);
			break;
    	}

    	break;

	case WIDGET_NOTIFY_BTN_PRESSED:
		switch( ulParam1 )
		{
		case Button_Minus:
			this->lValue--;
			break;

		case Button_Plus:
			this->lValue++;
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
// Constructor
//
//*****************************************************************************
void
DialogNumericEditor::Init( long lValue, unsigned char ucDotPos )
{
	this->lValue = lValue;
	this->ucDotPos = ucDotPos;
}

//*****************************************************************************
//
// Destructor
//
//*****************************************************************************
DialogNumericEditor::~DialogNumericEditor( void )
{
	delete[] pcText;
}
