/*
 * debug_psu.h
 *
 *  Created on: 11 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_DEBUG_DEBUG_PSU_H_
#define INTERFACE_WINDOWS_DEBUG_DEBUG_PSU_H_


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
class DebugPSUTab : public HorizontalDebugTabBase
{
public:
    DebugPSUTab(unsigned char ucID, Widget *pParent);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    void inner_content_render(tContext *pContext);

    ~DebugPSUTab(void);

private:
    RectangularButton       **pButtons;
    unsigned short          psu_group_enable[PSU_GROUP_COUNT];
};

/******************************************************************************
 *
 * Mark the end of the C bindings section for C++ compilers.
 *
 ******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_WINDOWS_DEBUG_DEBUG_PSU_H_ */
