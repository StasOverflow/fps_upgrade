/*
 * debug_system.h
 *
 *  Created on: 13 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_DEBUG_DEBUG_SYSTEM_H_
#define INTERFACE_WINDOWS_DEBUG_DEBUG_SYSTEM_H_

#include "horizontal_debug_tab.h"

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

/******************************************************************************
 *
 * Class describes this window
 *
 ******************************************************************************/
class DebugSystemTab : public HorizontalDebugTabBase
{
public:
    DebugSystemTab(unsigned char ucID, Widget *pParent);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    void inner_content_render(tContext *pContext);

    ~DebugSystemTab(void);

private:
    unsigned short system_acqmb_render( tContext *pContext, unsigned short sX, unsigned short sY );
    unsigned short system_aps_render( tContext *pContext, unsigned short sX, unsigned short sY );

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

#endif /* INTERFACE_WINDOWS_DEBUG_DEBUG_SYSTEM_H_ */
