/*
 * debug_cfg.h
 *
 *  Created on: 11 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INTERFACE_WINDOWS_DEBUG_DEBUG_CFG_H_
#define INTERFACE_WINDOWS_DEBUG_DEBUG_CFG_H_

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

#include "../debug/horizontal_debug_tab.h"

/******************************************************************************
 *
 * Class describes this window
 *
 ******************************************************************************/
class DebugCfgTab : public HorizontalDebugTabBase
{
public:
    DebugCfgTab(unsigned char ucID, Widget *pParent);

    long MessageProc(unsigned long ulMsg, unsigned long ulParam1,
                     unsigned long ulParam2);

    void inner_content_render(tContext *pContext);

    unsigned short system_connection_errors_render( tContext *pContext, unsigned short sX, unsigned short sY );
    unsigned short system_model_serial_render( tContext *pContext, unsigned short sX, unsigned short sY);

    ~DebugCfgTab(void);

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


#endif /* INTERFACE_WINDOWS_DEBUG_DEBUG_CFG_H_ */
