/*
 * heater_property.cpp
 *
 *  Created on: 20 мая 2019 г.
 *      Author: V.Striukov
 */


#include "ustdlib.h"
#include "heater_property.h"

#include "../comm/inter_comm.h"

const static tPropertyRange sPropertyRanges[] =
{
    {
        HEATER_TEMPERATURE_ON_SET,

        {
            HEATER_ON_LEVEL_SET_MIN,
            HEATER_ON_LEVEL_SET_MAX,
            HEATER_ON_LEVEL_SET_INC,
        }
    },
};

#define PROPERTY_RANGES_COUNT             ( sizeof(sPropertyRanges) / sizeof(sPropertyRanges[0]) )
//
// ***************************************
//



unsigned short
heater_property_get( unsigned long property )
{
    unsigned short usValue = 0;

    usValue = InterCommDataGet(property);

    return usValue;
}


void
heater_property_string_get( unsigned long property, char *string )
{
    unsigned short  value = 0;

    value = heater_property_get(property);

    switch( property )
    {
    case HEATER_TEMPERATURE_ON_SET:
        usprintf(string, "%d", value);
        break;

    default:
        usprintf(string, "Error");
    }
}


void
heater_property_validate( unsigned short usProperty, unsigned short *pusValue )
{
    unsigned short usPropertyIndex;

    for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
    {
        if( sPropertyRanges[usPropertyIndex].usProperty == usProperty )
        {
            break;
        }
    }

    if( usPropertyIndex < PROPERTY_RANGES_COUNT )
    {
        if( (short)*pusValue >= sPropertyRanges[usPropertyIndex].sPropertyRange.lMin )
        {
            if( (short)*pusValue <= sPropertyRanges[usPropertyIndex].sPropertyRange.lMax )
            {
                *pusValue -= *pusValue % sPropertyRanges[usPropertyIndex].sPropertyRange.usStep;
            }
            else
            {
                *pusValue = sPropertyRanges[usPropertyIndex].sPropertyRange.lMax;
            }
        }
        else
        {
            *pusValue = sPropertyRanges[usPropertyIndex].sPropertyRange.lMin;
        }
    }
}


void
heater_property_set( unsigned short property, unsigned short value )
{
    heater_property_validate(property, &value);
    InterCommDataSet(property, value);
}


void
heater_property_init( void )
{

}

