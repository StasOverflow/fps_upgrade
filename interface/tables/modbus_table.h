/*
 * modbus_table.h
 *
 *  Created on: 15 янв. 2020 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_TABLES_MODBUS_TABLE_H_
#define INTERFACE_TABLES_MODBUS_TABLE_H_


/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif
#include "windows/window_cell_setup_popup.h"

enum {
    Next,
    Prev,
};


/****
 *  Class, that contains methods and props of a current widget
 */
class ModbusTable : public Widget
{
public:
    ModbusTable(uint8_t ucID, Widget *pParent, uint16_t coord_x, uint16_t coord_y);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    void page_scroll( uint8_t direction );
    uint8_t     at_left_limit( void );
    uint8_t     at_right_limit( void );

    ~ModbusTable(void);

private:
    void        table_content_render( tContext *context );
    int16_t     offset;

    void        popup_handle( void );
    int8_t                       active_cell_id;
    WindowCellSetupPopup        *prop_popup;
    RectangularButton          **buttons;
};

/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif


#endif /* INTERFACE_TABLES_MODBUS_TABLE_H_ */
