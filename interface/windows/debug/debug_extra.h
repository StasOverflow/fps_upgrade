/*
 * debug_cfg.h
 *
 *  Created on: 11 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_DEBUG_DEBUG_EXTRA_H_
#define INTERFACE_WINDOWS_DEBUG_DEBUG_EXTRA_H_

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
class DebugExtraTab : public HorizontalDebugTabBase
{
public:
    DebugExtraTab(unsigned char ucID, Widget *pParent);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    void inner_content_render(tContext *pContext);

    unsigned short extra_debug_info_render( tContext *pContext, unsigned short sX, unsigned short sY );

    ~DebugExtraTab(void);

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


#endif /* INTERFACE_WINDOWS_DEBUG_DEBUG_EXTRA_H_ */
