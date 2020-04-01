/*
 * window_setup_modbus.h
 *
 *  Created on: 16 нояб. 2018 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_WINDOW_SETUP_MODBUS_H_
#define INTERFACE_WINDOWS_WINDOW_SETUP_MODBUS_H_

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
class WindowSetupModbus : public Widget
{
public:
    WindowSetupModbus(unsigned char ucID, Widget *pParent);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    ~WindowSetupModbus(void);


private:
    unsigned short usBaudrate;
    unsigned short usParity;
    unsigned short usSlaveAddress;
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_WINDOWS_WINDOW_SETUP_MODBUS_H_ */
