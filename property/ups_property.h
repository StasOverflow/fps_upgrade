
#ifndef UPS_PROPERTY_H_
#define UPS_PROPERTY_H_

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

enum UPSStatus
{
	UPS_STATUS_UNKNOWN,
	UPS_STATUS_ALARM,
	UPS_STATUS_OK
};


enum {
    Volt_Mon_Status_Alarm,
    Volt_Mon_Status_Ok,
};

unsigned short UPSPropertyGet( unsigned short usProperty );
void           UPSPropertyStringGet( unsigned short usProperty, char *pcString );
void           UPSPropertyStringGetFromValue( unsigned short usProperty, unsigned short usValue, char *pcString );

void           UPSPropertyValidate( unsigned short usProperty, unsigned short *pusValue );

void           UPSPropertySet( unsigned short usProperty, unsigned short usValue );

unsigned short UPSPropertyStepGet( unsigned short usProperty );

void           UPSPropertyInit( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* UPS_PROPERTY_H_ */
