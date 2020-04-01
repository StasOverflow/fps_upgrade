/*
 * window_info_horizontal.h
 *
 *  Created on: 15 нояб. 2018 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_WINDOW_INFO_HORIZONTAL_H_
#define INTERFACE_WINDOWS_WINDOW_INFO_HORIZONTAL_H_

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
class WindowInfo : public Widget
{
public:
    WindowInfo(unsigned char ucID, Widget *pParent);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    ~WindowInfo(void);

private:
    unsigned char ucServiceButtonCnt;
    unsigned char secret_button_pressed_count;

    unsigned long ulServiceButtonTimer;
    unsigned long secret_button_timer;

    unsigned char show_message;

    Canvas          *Secret_Message;
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_WINDOWS_WINDOW_INFO_HORIZONTAL_H_ */
