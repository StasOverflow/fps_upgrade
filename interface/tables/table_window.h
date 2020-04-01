/*
 * table_window.h
 *
 *  Created on: 10 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_TABLES_TABLE_WINDOW_H_
#define INTERFACE_TABLES_TABLE_WINDOW_H_

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

#include "tables/main_table.h"

//*****************************************************************************
//
// Class describes this window
//
//*****************************************************************************
class TableWindow : public Widget
{
public:
    TableWindow(unsigned char ucID, Widget *pParent, short rows, short cols);

    void TableContentsUpdate(tContext *pContext);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1, unsigned long ulParam2);

    ~TableWindow(void);

protected:
    Canvas              *Header;
    MainTable           *Table;
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif




#endif /* INTERFACE_TABLES_TABLE_WINDOW_H_ */
