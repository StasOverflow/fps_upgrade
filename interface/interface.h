
#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <string.h>
#include "config.h"
#include "ustdlib.h"
#include "string_utils.h"

#include "grlib/grlib.h"
#include "grlib/widgets/widget.h"
#include "grlib/widgets/canvas.h"
#include "grlib/widgets/rectangularbutton.h"
#include "grlib/widgets/circularbutton.h"
#include "debug_test/color_palette.h"

#include "images/images.h"

#include "window_manager.h"


#define GENERAL_BUTTON_WIDTH            63
#define GENERALBUTTON_HEIGHT            47


#define BUTTON_BACK_POSITION            2, 190, GENERAL_BUTTON_WIDTH, GENERALBUTTON_HEIGHT
#define BUTTON_SETUP_POSITION           254, 190, GENERAL_BUTTON_WIDTH, GENERALBUTTON_HEIGHT

#define BUTTON_MINUS_POSITION           238, 198, 40, 40
#define BUTTON_PLUS_POSITION            278, 198, 40, 40

#define DEBUG_TAB_BORDER_POS            2, 27, 316, 170
#define DEBUG_TAB_MAIN_POS              4, 29, 312, 166


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

void InterfaceTask( void *pvParameters );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INTERFACE_H_ */
