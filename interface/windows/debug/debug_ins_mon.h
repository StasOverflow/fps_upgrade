/*
 * ins_mon.h
 *
 *  Created on: 16 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_DEBUG_DEBUG_INS_MON_H_
#define INTERFACE_WINDOWS_DEBUG_DEBUG_INS_MON_H_

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
class DebugInsMonTab : public HorizontalDebugTabBase
{
public:
    DebugInsMonTab(unsigned char ucID, Widget *pParent);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);


    unsigned short  system_im_render( tContext *pContext, unsigned short sX, unsigned short sY );
    unsigned short  system_im_status_render( tContext *pContext, unsigned short sX, unsigned short sY );
    unsigned short  system_im_alarm_render( tContext *pContext, unsigned short sX, unsigned short sY );

    void            inner_content_render(tContext *pContext);

    ~DebugInsMonTab(void);

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

#endif /* INTERFACE_WINDOWS_DEBUG_DEBUG_INS_MON_H_ */
