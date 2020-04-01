/*
 * ac_output_property.h
 *
 *  Created on: 21.07.2014
 *      Author: V.Dudnik
 */

#ifndef OUTPUT_AC_PROPERTY_H_
#define OUTPUT_AC_PROPERTY_H_

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


enum OUTPUT_SOURCE_ENUM
{
	OUTPUT_SOURCE_NONE,
	OUTPUT_SOURCE_NORMAL,
	OUTPUT_SOURCE_BYPASS
};


enum eOutputStatus
{
	OUTPUT_AC_STATUS_UNKNOWN,
	OUTPUT_AC_STATUS_FAULT,
	OUTPUT_AC_STATUS_MODERATE,
	OUTPUT_AC_STATUS_OVERLOAD,
	OUTPUT_AC_STATUS_OFF,
	OUTPUT_AC_STATUS_OK
};


unsigned short ACOutputPhasePropertyGet( unsigned short usPhase, unsigned short usProperty );
void           ACOutputPhasePropertyStringGet( unsigned short usPhase, unsigned short usProperty, char *pcString );

unsigned short ACOutputFeederPropertyGet( unsigned short usFeeder, unsigned short usProperty );
void           ACOutputFeederPropertyStringGet( unsigned short usFeeder, unsigned short usProperty, char *pcString );

void           ACOutputFeederPropertyValidate( unsigned short usFeeder, unsigned short usProperty, unsigned short *pusValue );
void           ACOutputFeederPropertySet( unsigned short usFeeder, unsigned short usProperty, unsigned short usValue );
unsigned short ACOutputFeederPropertyStepGet( unsigned short usFeeder, unsigned short usProperty );

void  ACOutputPropertyInit( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* OUTPUT_AC_PROPERTY_H_ */
