#ifndef PSU_PROPERTY_H_
#define PSU_PROPERTY_H_


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

enum eChargerStatus
{
	PSU_STATUS_UNKNOWN,
	PSU_STATUS_MALFUNCTION,
	PSU_STATUS_OUTPUT_FAULT,
	PSU_STATUS_INPUT_FAULT,
	PSU_STATUS_UNDERCOOL,
	PSU_STATUS_OVERHEAT,
	PSU_STATUS_OFFLINE,
	PSU_STATUS_ONLINE
};


enum eChargerCommonStatus
{
	PSU_GROUP_STATUS_UNKNOWN,
	PSU_GROUP_STATUS_FAULT,
	PSU_GROUP_STATUS_WARNING,
	PSU_GROUP_STATUS_OK
};


unsigned short PSUPropertyGet( unsigned short usUnit, unsigned short usProperty );
void           PSUPropertyStringGet( unsigned short usUnit, unsigned short usProperty, char *pcString );

unsigned short PSUGroupPropertyGet( unsigned short usGroup, unsigned short usProperty );
void           PSUGroupPropertyStringGet( unsigned short usGroup, unsigned short usProperty, char *pcString );

tBoolean       PSUGroupPropertyIsValid( unsigned short usGroup, unsigned short usProperty, unsigned short usValue );
void           PSUGroupPropertySet( unsigned short usGroup, unsigned short usProperty, unsigned short usValue );
unsigned short PSUGroupPropertyStepGet( unsigned short usGroup, unsigned short usProperty );

void           PSUPropertyInit( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* PSU_PROPERTY_H_ */
