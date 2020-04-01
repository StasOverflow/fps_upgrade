/*
 * window_setup_modbus.h
 *
 *  Created on: 16 нояб. 2018 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_WINDOW_SETUP_MBUS_PROPS_H_
#define INTERFACE_WINDOWS_WINDOW_SETUP_MBUS_PROPS_H_

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

#include "controls/adj_editor_themed.h"

//*****************************************************************************
//
// Class describes this window
//
//*****************************************************************************
class WindowSetupMbusProps : public Widget
{
public:
    WindowSetupMbusProps(unsigned char ucID, Widget *pParent);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    ~WindowSetupMbusProps(void);


private:
    uint16_t        scroll_mode;
    uint16_t        def_format;

    uint16_t        baud;
    uint16_t        parity;
    uint16_t        stop_bits;
    uint16_t        data_bits;

    uint16_t        slave_id;
    uint16_t        start_addr;
    uint16_t        reg_quan;

    AdjEditor       **editor;
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_WINDOWS_WINDOW_SETUP_MBUS_PROPS_H_ */
