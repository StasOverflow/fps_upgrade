/*
 * debug_debug_horizontal.h
 *
 *  Created on: 22 нояб. 2018 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_DEBUG_DEBUG_DEBUG_HORIZONTAL_H_
#define INTERFACE_WINDOWS_DEBUG_DEBUG_DEBUG_HORIZONTAL_H_

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
class DebugDebugHorizontal : public Widget
{
public:
    DebugDebugHorizontal(unsigned char ucID, Widget *pParent);

    void StructRender(tContext *pContext);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    ~DebugDebugHorizontal(void);

private:
    RectangularButton **pButtons;
    short             sRowMin;
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_WINDOWS_DEBUG_DEBUG_DEBUG_HORIZONTAL_H_ */
