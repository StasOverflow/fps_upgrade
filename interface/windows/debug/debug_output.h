/*
 * debug_output.h
 *
 *  Created on: 10 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_DEBUG_DEBUG_OUTPUT_H_
#define INTERFACE_WINDOWS_DEBUG_DEBUG_OUTPUT_H_


/******************************************************************************
 *
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 *
 ******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include "horizontal_debug_tab.h"

/******************************************************************************
 *
 * Class describes this window
 *
 ******************************************************************************/
class DebugOutputTab : public HorizontalDebugTabBase
{
public:
    DebugOutputTab(unsigned char ucID, Widget *pParent);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    void inner_content_render(tContext *pContext);

    ~DebugOutputTab(void);

private:
    RectangularButton       **pButtons;
};

/******************************************************************************
 *
 * Mark the end of the C bindings section for C++ compilers.
 *
 ******************************************************************************/
#ifdef __cplusplus
}
#endif


#endif /* INTERFACE_WINDOWS_DEBUG_DEBUG_OUTPUT_H_ */
