
#ifndef __LCD_H
#define __LCD_H

#include "grlib/grlib.h"

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


enum
{
	DISPLAY_ORIENTATION_LANDSCAPE,
	DISPLAY_ORIENTATION_LANDSCAPE_FLIP,
	DISPLAY_ORIENTATION_PORTRAIT,
	DISPLAY_ORIENTATION_PORTRAIT_FLIP
};

uint16_t    lcd_refresh_rate_get( void );
uint16_t    lcd_fps_get( void );
void        LCD_Init(void);

void tim_val_plus( uint8_t value );
void tim_val_minus( uint8_t value );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif /* __LCD_H */

