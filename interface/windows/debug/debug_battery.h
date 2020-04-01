/*
 * debug_battery.h
 *
 *  Created on: 13 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_DEBUG_DEBUG_BATTERY_H_
#define INTERFACE_WINDOWS_DEBUG_DEBUG_BATTERY_H_


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
class DebugBatteryTab : public HorizontalDebugTabBase
{
public:
    DebugBatteryTab(unsigned char ucID, Widget *pParent);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    void inner_content_render(tContext *pContext);

    ~DebugBatteryTab(void);

private:
    void button_create_or_update_borders( unsigned short index, unsigned short id,
                                          unsigned short sX, unsigned short sY);

    unsigned short battery_status_render( tContext *pContext, unsigned short sX, unsigned short sY);
    unsigned short battery_pids_render( tContext *pContext, unsigned short sX, unsigned short sY);
    unsigned short battery_shunt_render( tContext *pContext, unsigned short sX, unsigned short sY);

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


#endif /* INTERFACE_WINDOWS_DEBUG_DEBUG_BATTERY_H_ */
