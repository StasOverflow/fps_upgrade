/*
 * inverter_property.h
 *
 *  Created on: 21.07.2014
 *      Author: V.Dudnik
 */

#ifndef INVERTER_PROPERTY_H_
#define INVERTER_PROPERTY_H_

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

#define INVERTER_FLAG_OVERHEAT            ( 1 <<  0 )
#define INVERTER_FLAG_ON_OFF              ( 1 <<  1 )
#define INVERTER_FLAG_OVERLOAD_SIGNAL     ( 1 <<  2 )
#define INVERTER_FLAG_MASTER              ( 1 <<  3 )
#define INVERTER_FLAG_AUXV_LOW            ( 1 <<  4 )
#define INVERTER_FLAG_SYNC_EXTERNAL       ( 1 <<  5 )
#define INVERTER_FLAG_POWEROK             ( 1 <<  6 )
#define INVERTER_FLAG_OFFLINE             ( 1 <<  7 )
#define INVERTER_FLAG_SYNC_SIGOK          ( 1 <<  8 )
#define INVERTER_FLAG_SYNC_OK             ( 1 <<  9 )
#define INVERTER_FLAG_MALFUNCTION         ( 1 << 10 )
#define INVERTER_FLAG_CURRENT_LIM         ( 1 << 11 )
#define INVERTER_FLAG_TEMPSENSOR_BROKEN   ( 1 << 12 )
#define INVERTER_FLAG_TEMPSENSOR_SHORTEN  ( 1 << 13 )
#define INVERTER_FLAG_OVERLOAD            ( 1 << 14 )
#define INVERTER_FLAG_STUB_BIT_15         ( 1 << 15 )


enum InverterStatusEnum
{
	INVERTER_STATUS_UNKNOWN,
	INVERTER_STATUS_FAULT,
	INVERTER_STATUS_TEMP_SENSOR_SHORTEN,
	INVERTER_STATUS_TEMP_SENSOR_BROKEN,
	INVERTER_STATUS_CURRENT_LIMITED,
	INVERTER_STATUS_AUX_VOLTAGE_LOW,
	INVERTER_STATUS_350V_LOW,
	INVERTER_STATUS_OVERLOAD,
	INVERTER_STATUS_OVERHEAT,
	INVERTER_STATUS_EXT_OFF,
	INVERTER_STATUS_OFFLINE,
	INVERTER_STATUS_OK
};


enum eInverterGroupStatusEnum
{
	INVERTER_GROUP_STATUS_UNKNOWN,
	INVERTER_GROUP_STATUS_FAULT,
	INVERTER_GROUP_STATUS_NOT_POWERED,
	INVERTER_GROUP_STATUS_READY,
	INVERTER_GROUP_STATUS_MODERATE,
	INVERTER_GROUP_STATUS_OK
};


unsigned short InverterPropertyGet( unsigned short usUnit, unsigned short usProperty );
void           InverterPropertyStringGet( unsigned short usUnit, unsigned short usProperty, char *pcString );

unsigned short InverterGroupPropertyGet( unsigned short usGroup, unsigned short usProperty );
void           InverterGroupPropertyStringGet( unsigned short usGroup, unsigned short usProperty, char *pcString );

void           InverterPropertyInit( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* INVERTER_PROPERTY_H_ */
