/*
 * debug_rtos.h
 *
 *  Created on: 14 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_DEBUG_DEBUG_RTOS_H_
#define INTERFACE_WINDOWS_DEBUG_DEBUG_RTOS_H_
#include "horizontal_debug_tab.h"


/******************************************************************************
 *
 * Class describes this window
 *
 ******************************************************************************/
class DebugRtosTab : public HorizontalDebugTabBase
{
public:
    DebugRtosTab(unsigned char ucID, Widget *pParent);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    void inner_content_render(tContext *pContext);

    ~DebugRtosTab(void);

private:
    void rtos_debug_info_render( unsigned short index, unsigned short id,
                                 unsigned short sX, unsigned short sY);

    RectangularButton       **pButtons;
    unsigned short          buttons_created;
};

/******************************************************************************
 *
 * Mark the end of the C bindings section for C++ compilers.
 *
 ******************************************************************************/
#ifdef __cplusplus
}
#endif


#endif /* INTERFACE_WINDOWS_DEBUG_DEBUG_RTOS_H_ */
