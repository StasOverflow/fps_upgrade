
#ifndef BATTERY_PROPERTY_H_
#define BATTERY_PROPERTY_H_

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


#define BATF_CONNECTED              ( 1 << 0 )
#define BATF_DISCHARGE              ( 1 << 1 )
// ...
#define BATF_OVERHEAT               ( 1 << 4 )


enum BatteryStatusEnum
{
	BATTERY_STATUS_UNKNOWN,
	BATTERY_STATUS_OVERHEAT,
	BATTERY_STATUS_DEPLETED,
	BATTERY_STATUS_DISCONNECTED,
	BATTERY_STATUS_DISCHARGE,
	BATTERY_STATUS_HOLD,
	BATTERY_STATUS_FAST
};

enum BatteryLevelEnum
{
	BATTERY_LEVEL_UNKNOWN,
	BATTERY_LEVEL_DEPLETED,
	BATTERY_LEVEL_LOW,
	BATTERY_LEVEL_MEDIUM,
	BATTERY_LEVEL_HIGH,
	BATTERY_LEVEL_TOO_HIGH
};


unsigned short BatteryPropertyGet( unsigned short usBatt, unsigned short usProperty );
void           BatteryPropertyStringGet( unsigned short usBatt, unsigned short usProperty, char *pcString );

void           BatteryPropertyValidate( unsigned short usBatt, unsigned short usProperty, unsigned short *pusValue );
unsigned char  BatteryPropertyIsValid( unsigned short usBatt, unsigned short usProperty, unsigned short usValue );
void           BatteryPropertySet( unsigned short usBatt, unsigned short usProperty, unsigned short usValue );
unsigned short BatteryPropertyStepGet( unsigned short usBatt, unsigned short usProperty );

void           BatteryPropertyInit( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* BATTERY_PROPERTY_H_ */
