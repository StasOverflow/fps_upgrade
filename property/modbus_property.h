/*
 * modbus_property.h
 *
 *  Created on: 21.07.2014
 *      Author: V.Dudnik
 */

#ifndef MODBUS_PROPERTY_H_
#define MODBUS_PROPERTY_H_

#include "property.h"

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


unsigned short ModbusPropertyValueGet( unsigned short usProperty );
void           ModbusPropertyStringGet( unsigned short usProperty, char *pcString );
void           ModbusPropertyStringGetFromValue( unsigned short usProperty, unsigned short usValue, char *pcString );

void           ModbusPropertyValidate( unsigned short usProperty, unsigned short *pusValue );

void           ModbusPropertySet( unsigned short usProperty, unsigned short usValue );

void           ModbusPropertyInit( void );


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* MODBUS_PROPERTY_H_ */
