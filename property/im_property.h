/*
 * im_property.h
 *
 *  Created on: 11 мая 2019 г.
 *      Author: V.Striukov
 */

#ifndef INSULATION_MONITOR_PROPERTY_H_
#define INSULATION_MONITOR_PROPERTY_H_


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

enum {
    Ins_Mon_Status_Unknown,
    Ins_Mon_Status_Alarm,
    Ins_Mon_Status_Ok,
};

void            im_property_validate( unsigned short usProperty, unsigned short *pusValue );

unsigned short  im_property_get( unsigned long prop );
void            im_property_string_get( unsigned long prop, char *string );
void            im_property_set( unsigned short property, unsigned short value );

void            im_poperty_init( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif /* INSULATION_MONITOR_PROPERTY_H_ */
