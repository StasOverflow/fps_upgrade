/*
 * main_table.cpp
 *
 *  Created on: 8 мая 2019 г.
 *      Author: V.Striukov
 */

#include "interface.h"

#include "main_table.h"


#define DEF_COL_WIDTH                       90
#define APPROX_PIXEL_PER_CHAR               8
#define CANVAS_WIDTH                        305
#define CANVAS_HEIGHT                       150
#define INTER_ROW_INDENT                    20

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
MainTable::MainTable(unsigned char ucID, Widget *pParent,
                     short rows, short cols)
          :Widget(ucID, pParent, 4, 15, CANVAS_WIDTH, CANVAS_HEIGHT)
{
    Canvas            *pCanvas;

    this->rows = rows;
    this->cols = cols;

    this->cell_array = new char**[rows];

    for( int i = 0; i < rows; i++ )
    {
        this->cell_array[i] = new char*[cols];

        for( int j = 0; j < cols; j ++ )
        {
            this->cell_array[i][j] = new char[20]();
        }
    }

    this->col_width = new unsigned long[cols];

    for( int i = 0; i < cols; i++ )
    {
        this->col_width[i] = DEF_COL_WIDTH;
    }

    /*
     *  Main Canvas
     */
    pCanvas = new Canvas(Canvas_Main, this, 4, 15, CANVAS_WIDTH, CANVAS_HEIGHT);
    pCanvas->ulStyle = CANVAS_STYLE_PAINT_NOTIFY;
}
/******************************************************************************
 *
 *  Table manipulating interfaces
 *
 ******************************************************************************/
void
MainTable::row_set_name( short row_id, char * name )
{
    this->cell_set_value(row_id, 0, name);
}


void
MainTable::col_set_name( short col_id, char * name )
{
    this->cell_set_value(0, col_id, name);
}


void
MainTable::col_largest_word_set( short col_id, char * word )
{
    unsigned long len;

    /*
     * Calculate cell max len
     */
    len = strlen(word) * APPROX_PIXEL_PER_CHAR;
    if( len > this->col_width[col_id])
    {
        this->col_set_width(col_id, len);
    }
}


void
MainTable::col_set_width( short col_id, short width )
{
    this->col_width[col_id] = width;
}


void
MainTable::cell_set_value( short row, short col, long value )
{
    char        word[20];


    usprintf(word, "%d", value);
    this->cell_set_value(row, col, word);
}


void
MainTable::cell_set_value( short row, short col, char *value )
{
    col_largest_word_set(col, value);

    usprintf(this->cell_array[row][col], value);
}
/******************************************************************************
 *
 *  Rerender table and its values
 *
 ******************************************************************************/
void
MainTable::TableContentsRender( tContext *pContext )
{
    short             sX;
    short             sY;
    short             initial_x_indent;
    short             additional_width;

    GrContextForegroundSet(pContext, ClrBlack);
    GrContextFontSet(pContext, g_pFontWDoslike15pt);

    additional_width = 0;
    for( int i = 0; i < this->cols; i++ )
    {
        additional_width += this->col_width[i];
    }

    initial_x_indent = (CANVAS_WIDTH - additional_width) / 2; // + (this->col_width[0] / 2);

    this->horizontal_line_draw(pContext, additional_width,
            ((( CANVAS_HEIGHT - this->rows * INTER_ROW_INDENT ) / 2 ) +
            INTER_ROW_INDENT*2 ) + (INTER_ROW_INDENT / 4)) ;

    for( int row = 0; row < this->rows; row++ )
    {
        sX = initial_x_indent;

        for( int col = 0; col < this->cols; col++ )
        {
            if( col )
            {
                sX += (this->col_width[col - 1] / 2);
                vertical_line_draw(pContext, (this->rows * INTER_ROW_INDENT) + (INTER_ROW_INDENT / 2),
                        pContext->sClipRegion.sXMin + sX);
            }

            sX += this->col_width[col] / 2;


            sY = ( ( CANVAS_HEIGHT - this->rows * INTER_ROW_INDENT ) / 2 ) +
                 INTER_ROW_INDENT * row + ( row ? INTER_ROW_INDENT/2 : 0 ) + 5;

            GrStringDrawCenteredL(pContext, this->cell_array[row][col], -1,
                                            pContext->sClipRegion.sXMin + sX,
                                            pContext->sClipRegion.sYMin + sY, 0);
        }
    }
}
/******************************************************************************
 *
 *  Private helper funcs
 *
 ******************************************************************************/
void
MainTable::horizontal_line_draw( tContext *pContext, short width, short y_coordinate )
{
    unsigned long  lColor;
    unsigned long  start_point;

    start_point = (CANVAS_WIDTH - width - 50) / 2;

    lColor = ClrBlack;
    GrContextForegroundSet(pContext, lColor);

    GrLineDrawH(pContext, start_point, start_point + width + 75,
            y_coordinate);
    GrLineDrawH(pContext, start_point, start_point + width + 75,
            y_coordinate + 1);
}


void
MainTable::vertical_line_draw( tContext *pContext, short height, short x_coordinate )
{
    unsigned long  lColor;
    unsigned long  start_point;

    start_point = 30 + ( (CANVAS_HEIGHT - height) / 2 );

    lColor = ClrBlack;
    GrContextForegroundSet(pContext, lColor);

    GrLineDrawV(pContext, x_coordinate,
                          start_point,
                          start_point + height);

    GrLineDrawV(pContext, x_coordinate + 1,
                          start_point,
                          start_point + height);
}
//*****************************************************************************
//
// This function handles the messages for this widget.
//
//*****************************************************************************
long
MainTable::MessageProc(unsigned long ulMsg, unsigned long ulParam1,
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

        GrContextForegroundSet(&sCtx, ClrBlack);
        GrContextBackgroundSet(&sCtx, ClrBlack);

        switch( ulParam1 )
        {
        case Canvas_Main:
            this->TableContentsRender(&sCtx);
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
MainTable::~MainTable(void)
{
    for( int i = 0; i < this->rows; i++ )
    {
        for( int j = 0; j < this->cols; j ++ )
        {
            delete[] this->cell_array[i][j];
        }
        delete[] this->cell_array[i];
    }
    delete[] this->cell_array;

    delete[] this->col_width;
}
