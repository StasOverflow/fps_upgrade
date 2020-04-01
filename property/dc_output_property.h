
#ifndef DC_OUTPUT_PROPERTY_H_
#define DC_OUTPUT_PROPERTY_H_

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

enum
{
	OUTPUT_DC_STATUS_UNKNOWN,
	OUTPUT_DC_STATUS_FAULT,
	OUTPUT_DC_STATUS_OFF,
	OUTPUT_DC_STATUS_OK
};


unsigned short DCOutputPropertyGet( unsigned short usChannel, unsigned short usProperty );
void           DCOutputPropertyStringGet( unsigned short usChannel, unsigned short usProperty, char *pcString );
void           DCOutputPropertyValidate( unsigned short usChannel, unsigned short usProperty, unsigned short *pusValue );
void           DCOutputPropertySet( unsigned short usChannel, unsigned short usProperty, unsigned short usValue );
unsigned short DCOutputPropertyStepGet( unsigned short usChannel, unsigned short usProperty );
void           DCOutputPropertyInit( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* DC_OUTPUT_PROPERTY_H_ */
