/*
 * horizontal_debug_tab.h
 *
 *  Created on: 10 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_DEBUG_HORIZONTAL_DEBUG_TAB_H_
#define INTERFACE_WINDOWS_DEBUG_HORIZONTAL_DEBUG_TAB_H_

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

#include "interface.h"
/******************************************************************************
 *
 * Position defines
 *
 ******************************************************************************/
#define NUM_ED_CENTRAL_COORDS                       110, 45
//*****************************************************************************
//
// Class describes this window
//
//*****************************************************************************
class HorizontalDebugTabBase : public Widget
{
public:
    HorizontalDebugTabBase(unsigned char ucID, Widget *pParent);

    virtual void inner_content_render(tContext *pContext) = 0;

    ~HorizontalDebugTabBase(void);

private:
};

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_WINDOWS_DEBUG_HORIZONTAL_DEBUG_TAB_H_ */
