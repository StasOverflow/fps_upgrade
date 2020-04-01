/*
 * color_palette.h
 *
 *  Created on: 18 июл. 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_DEBUG_TEST_COLOR_PALETTE_H_
#define INTERFACE_DEBUG_TEST_COLOR_PALETTE_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include "stdio.h"

//*****************************************************************************
//
// Class describes this window
//
//*****************************************************************************
typedef enum {
    Direct_Type,
    Reversed_Type,
    True_Type,
} palette_type_t;


class ColorPalette : public Widget
{
public:
    ColorPalette(unsigned char ucID, Widget *pParent,
                 long x_coord, long y_coord);

    void inner_context_render(tContext & sCtx);

    void pallete_draw(tContext & sCtx, palette_type_t type, short x_coord, short y_coord);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    /*
     * TODO:
     * void columns_append( short column_quantity );
     * void rows_append( short row_quantity );
     */

    void row_set_name( short row_id, char * name );
    void col_set_name( short col_id, char * name );

    void col_set_width( short col_id, short width );

    void cell_set_value( short row, short col, char *value );
    void cell_set_value( short row, short col, long value );

    void col_largest_word_set( short col_id, char * word );

    ~ColorPalette(void);


private:

    uint32_t    inverted_color_get( uint32_t color );

};
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif /* INTERFACE_DEBUG_TEST_COLOR_PALETTE_H_ */
