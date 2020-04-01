/*
 * heater_property.h
 *
 *  Created on: 20 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef PROPERTY_HEATER_PROPERTY_H_
#define PROPERTY_HEATER_PROPERTY_H_

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

#include "property.h"

void            heater_property_validate( unsigned short usProperty, unsigned short *pusValue );

unsigned short  heater_property_get( unsigned long prop );
void            heater_property_string_get( unsigned long prop, char *string );
void            heater_property_set( unsigned short property, unsigned short value );

void            heater_poperty_init( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* PROPERTY_HEATER_PROPERTY_H_ */
