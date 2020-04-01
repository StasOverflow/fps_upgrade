/*
 * debug_property.h
 *
 *  Created on: 21.07.2014
 *      Author: V.Dudnik
 */

#ifndef DEBUG_PROPERTY_H_
#define DEBUG_PROPERTY_H_

#include <property.h>

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

#define ATS_DF_INPUT_OK                          ( 1 << 0 )
#define ATS_DF_INPUT_PHASE_SEQOK                 ( 1 << 1 )
#define ATS_DF_INVERTER_OK                       ( 1 << 2 )
#define ATS_DF_INVERTER_SYNC_OK                  ( 1 << 3 )
#define ATS_DF_OUTPUT_A_CURRENT_OK               ( 1 << 4 )
#define ATS_DF_OUTPUT_B_CURRENT_OK               ( 1 << 5 )
#define ATS_DF_OUTPUT_C_CURRENT_OK               ( 1 << 6 )


enum
{
	ATS_MODE_AUTO,
	ATS_MODE_MANUAL
};


unsigned short DebugPropertyGet( unsigned short usProperty );
void           DebugPropertyStringGet( unsigned short usProperty, char *pcString );

void           DebugPropertyValidate( unsigned short usProperty, long *plValue );
void           DebugPropertySet( unsigned short usProperty, unsigned short usValue );

void           DebugPropertyInit( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* DEBUG_PROPERTY_H_ */
