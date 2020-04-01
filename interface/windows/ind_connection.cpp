
#include "interface.h"
#include "FreeRTOS.h"
#include "task.h"

#include "property/input_property.h"
#include "property/battery_property.h"
#include "property/inverter_property.h"
#include "property/dc_output_property.h"
#include "property/ac_output_property.h"
#include "property/ups_property.h"

//*****************************************************************************
//
// The colors used in this window
//
//*****************************************************************************
#define TABLE_CELL_TEXT_COLOR                0x005c7b99
#define WINDOW_COLOR                0x00000000
#define TEXT_COLOR                  0x00F0F0F0

#define BUTTONS_COUNT               ( 10 )
#define PICTURE_COUNT               (  2 )
#define FRAMES_PER_SECOND           (  1 )
#define ANIMATION_FPS               ( 1000 / FRAMES_PER_SECOND )

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


typedef struct {
    const unsigned char * frame[2];
} t_icon;

//static t_icon icon;

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
IndicatorConnection::IndicatorConnection(unsigned char ucID, Widget *pParent)
               :Widget(ucID, pParent, 0, 0, 1, 1)
{
	Canvas	      *pCanvas;

	//
	// Main canvas
	//
	pCanvas = new Canvas(Canvas_Main, this, 0, 0, 240, 25);
	pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;

//
//    icon.frame[0] = g_pucIconConnectionError_1;
//    icon.frame[1] = g_pucIconConnectionError_3;
}


void
IndicatorConnection::IndicatorAnimationRender(tContext *sCtx)
{
    TickType_t              xCurrentTime;
//    static short            sCurrentPic = 0;
//    long                    lValue = 0;
    static unsigned char    ucRender = false;
    static TickType_t       xAnimationTimestamp;
    //    char                    cTitle[4];

    xCurrentTime = xTaskGetTickCount();
    if( xCurrentTime - xAnimationTimestamp >= ( 200 ) )
    {
        xAnimationTimestamp = xCurrentTime;

        if( inter_comm_conn_present() )
        {

            ucRender = true;
        }
        else
        {
            ucRender = false;
        }
    }

//    usprintf(cTitle, "%d", (short) lValue);
//
//    GrContextFontSet(sCtx, g_pFontWCourier21pt);
//    GrContextForegroundSet(sCtx, ClrLime);
//
//    GrStringDrawCenteredL(sCtx, cTitle, -1,
//                          sCtx->sClipRegion.sXMin + 80,
//                          sCtx->sClipRegion.sYMin + 10, 0);

	if( ucRender )
	{
//        GrTransparentImageDraw(sCtx, icon.frame[sCurrentPic],
//                               sCtx->sClipRegion.sXMin + 213,
//                               sCtx->sClipRegion.sYMin + 3, 0);
	}
}


long
IndicatorConnection::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
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
		case Canvas_Main:
			this->IndicatorAnimationRender(&sCtx);
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

IndicatorConnection::~IndicatorConnection(void)
{

}
