/*
 * color_palette.cpp
 *
 *  Created on: 18 июл. 2019 г.
 *      Author: V.Striukov
 */


#include "interface.h"

#include "color_palette.h"
#include "string.h"


#define CANVAS_WIDTH                        320
#define CANVAS_HEIGHT                       100


typedef struct  {
    uint32_t     hex;
    char         name[16];
} color_t;


typedef enum {
    Red,
    Green,
    Blue,
} clr;

typedef enum {
    Plus = 0,
    Minus = 1,
} sign_t;


typedef struct {
    clr         current;
    sign_t      sign;
} clr_loop_t;


clr_loop_t color_loop[] = {
    {
        .current = Blue,
        .sign = Plus,
    },
    {
        .current = Red,
        .sign = Minus,
    },
    {
        .current = Green,
        .sign = Plus,
    },
    {
        .current = Blue,
        .sign = Minus,
    },
    {
        .current = Red,
        .sign = Plus,
    },
    {
        .current = Green,
        .sign = Minus,
    },
};

static const  uint8_t loop_size = (sizeof(color_loop)/sizeof(color_loop[0]));

color_t color_pairs[] = {
    {
        0xff0000,
        "Red",
    },
    {
        0xff00ff,
        "Magenta",
    },
    {
        0x0000ff,
        "Blue",
    },
    {
        0x00ffff,
        "Cyan"
    },
    {
        0x00ff00,
        "Green"
    },
    {
        0xffff00,
        "Yellow"
    },
    {
        0xff0000,
        "Red"
    },
};



//*****************************************************************************
//
// The enum of all widget ID's used in this window
//
//*****************************************************************************
enum
{
    Canvas_Main,

    Widget_Max = 0xFF,
};
//*****************************************************************************
//
// Construcor of widget
//
//*****************************************************************************
ColorPalette::ColorPalette(unsigned char ucID, Widget *pParent,
                           long x_coord, long y_coord)
          :Widget(ucID, pParent, x_coord, y_coord, CANVAS_WIDTH, CANVAS_HEIGHT)
{
    Canvas            *pCanvas;

    /*
     *  Main Canvas
     */
    pCanvas = new Canvas(Canvas_Main, this, 0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);
    pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;
}

/******************************************************************************
 *
 *  Rerender color palette
 *
 ******************************************************************************/
void
ColorPalette::pallete_draw( tContext & sCtx, palette_type_t type, short x_coord, short y_coord )
{

}

uint32_t
ColorPalette::inverted_color_get( uint32_t color )
{
    return 0xffffff - color;
}


void
ColorPalette::inner_context_render( tContext & sCtx )
{
    uint16_t        width;
    int16_t         gap_x;
    int16_t         gap_y;
    char            string[16];
    int32_t         inter_val[3];
    int             val = 0;
    int16_t         *loop_gaps;
    uint32_t        *colors;

    inter_val[Red] = 255;
    inter_val[Green] = 0;
    inter_val[Blue] = 0;

    width = 30;

    gap_x = sCtx.sClipRegion.sXMin - 5;
    gap_y = sCtx.sClipRegion.sYMin;

    loop_gaps = new int16_t[loop_size + 3];
    colors = new uint32_t[loop_size + 3];

    for( int loop_id = 0; loop_id < loop_size; loop_id++ )
    {
        for( int i = 0; i <= 255; i += 6 )
        {
            if( color_loop[loop_id].sign == Minus )
            {
                val = 255 - i;
            }
            else
            {
                val = i;
            }

            inter_val[color_loop[loop_id].current] = val;

            colors[loop_id] = (inter_val[Red] << 16 | inter_val[Green] << 8 | inter_val[Blue]);

            GrContextForegroundSet(&sCtx, colors[loop_id]);
            GrLineDrawV(&sCtx, gap_x, gap_y, gap_y + width);

            GrContextForegroundSet(&sCtx, this->inverted_color_get(colors[loop_id]));
            GrLineDrawV(&sCtx, gap_x, gap_y + width + 1, gap_y + width*2 + 1);

            gap_x++;
        }

        loop_gaps[loop_id] = gap_x;
    }

    for( int i = 0; i <= 255; i+= 7 )
    {
        inter_val[Green] = i;
        inter_val[Blue] = i;

        colors[loop_size] = (inter_val[Red] << 16 | inter_val[Green] << 8 | inter_val[Blue]);
        GrContextForegroundSet(&sCtx, colors[loop_size]);
        GrLineDrawV(&sCtx, gap_x, gap_y, gap_y + width);

        colors[loop_size] = (inter_val[Red] << 16 | inter_val[Green] << 8 | inter_val[Blue]);
        GrContextForegroundSet(&sCtx, this->inverted_color_get(colors[loop_size]));
        GrLineDrawV(&sCtx, gap_x++, gap_y + width + 1, gap_y + width * 2 + 1);
    }

    loop_gaps[loop_size] = gap_x;

    for( int i = 255; i >= 0; i-= 7 )
    {
        inter_val[Red] = i;
        inter_val[Green] = i;
        inter_val[Blue] = i;

        colors[loop_size + 1] = (inter_val[Red] << 16 | inter_val[Green] << 8 | inter_val[Blue]);

        GrContextForegroundSet(&sCtx, colors[loop_size + 1]);
        GrLineDrawV(&sCtx, gap_x, gap_y, gap_y + width);


        GrContextForegroundSet(&sCtx, this->inverted_color_get(colors[loop_size + 1]));
        GrLineDrawV(&sCtx, gap_x++, gap_y + width + 1, gap_y + width * 2 + 1);

    }

    loop_gaps[loop_size + 1] = gap_x;

    for( int i = 0; i <= 128; i+= 7 )
    {
        GrContextForegroundSet(&sCtx, ((i << 16) | (i << 8) | (i)) );
        GrLineDrawV(&sCtx, gap_x, gap_y, gap_y + width);

        GrContextForegroundSet(&sCtx, this->inverted_color_get((i << 16) | (i << 8) | (i)) );
        GrLineDrawV(&sCtx, gap_x++, gap_y + width + 1, gap_y + width * 2 + 1);
    }

    gap_x = sCtx.sClipRegion.sXMin - 5;

    for( int loop_id = 0; loop_id < loop_size + 2; loop_id++ )
    {
        if( loop_id )
        {
            /*
             *  Direct names
             */
            if( loop_id != loop_size + 1 )
            {
                usprintf(string, color_pairs[loop_id].name);
            }
            else
            {
                usprintf(string, "White");
            }
            GrContextFontSet(&sCtx, g_pFontWTahoma13pt);
            GrContextForegroundSet(&sCtx, this->inverted_color_get(colors[loop_id - 1]));

            GrStringDrawCenteredL(&sCtx, string, -1, loop_gaps[loop_id - 1], gap_y + width / 2, 0);

            /*
             *  Inverted names
             */
            if( loop_id != loop_size + 1 )
            {
                usprintf(string, color_pairs[loop_id < 3 ? loop_size - 3 + loop_id : loop_id - 3].name);
            }
            else
            {
                usprintf(string, "Black");
            }

            GrContextFontSet(&sCtx, g_pFontWTahoma13pt);
            GrContextForegroundSet(&sCtx, colors[loop_id - 1]);

            GrStringDrawCenteredL(&sCtx, string, -1, loop_gaps[loop_id - 1], gap_y + width + width / 2 + 1, 0);
        }
    }

    delete[] loop_gaps;
    delete[] colors;

}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
ColorPalette::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                        unsigned long ulParam2)
{
    tRectangle      sPosition;
    tContext        sCtx;


    switch( ulMsg )
    {
    case WIDGET_MSG_UPDATE:
        /*
         * Notify parent
         */
        Widget::ParentNotify(this, WIDGET_NOTIFY_UPDATE);

        break;

    case WIDGET_NOTIFY_PAINT:
        ((Widget *)ulParam2)->AbsolutePositionGet(&sPosition);

        GrContextInit(&sCtx);
        GrContextClipRegionSet(&sCtx, &sPosition);

        switch( ulParam1 )
        {
        case Canvas_Main:
            this->inner_context_render(sCtx);
            break;

        default:
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
ColorPalette::~ColorPalette(void)
{

}



