/*
 * main_table.h
 *
 *  Created on: 8 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_TABLES_MAIN_TABLE_H_
#define INTERFACE_TABLES_MAIN_TABLE_H_

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

//*****************************************************************************
//
// Class describes this window
//
//*****************************************************************************
class MainTable : public Widget
{
public:
    MainTable(unsigned char ucID, Widget *pParent,
              short rows, short cols);

    void TableContentsRender(tContext *pContext);

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

    ~MainTable(void);

private:

    void horizontal_line_draw( tContext *pContext, short width, short y_coordinate );
    void vertical_line_draw( tContext *pContext, short height, short x_coordinate );

    char                ***cell_array;

    unsigned long       *col_width;

    unsigned short      inter_cell_x_indent;
    unsigned short      inter_cell_y_indent;
    unsigned short      rows;
    unsigned short      cols;

};
//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_TABLES_MAIN_TABLE_H_ */
