/*
 * im_property.cpp
 *
 *  Created on: 11 мая 2019 г.
 *      Author: V.Striukov
 */


#include "ustdlib.h"
#include "im_property.h"

#include "../comm/inter_comm.h"

const static tPropertyRange sPropertyRanges[] =
{
    {
        IM_ALARM_RESISTANCE_SET,

        {
            ALARM_RESISTANCE_SET_MIN,
            ALARM_RESISTANCE_SET_MAX,
            ALARM_RESISTANCE_SET_INC,
        }
    },

    {
        IM_ALARM_TIME_SET,

        {
            1,
            10,
            1
        }
    },

    {
        IM_ALARM_FORWARD_ENABLE_SET,

        {
            0,
            1,
            1
        }
    },
};

#define PROPERTY_RANGES_COUNT             ( sizeof(sPropertyRanges) / sizeof(sPropertyRanges[0]) )

#define ALARM_RESISTANCE_SET_MIN        5
#define ALARM_RESISTANCE_SET_MAX        50
#define ALARM_RESISTANCE_SET_INC        5

#define ALARM_TIME_SET_MIN              1
#define ALARM_TIME_SET_MAX              10
#define ALARM_TIME_SET_INC              1
//
// ***************************************
//


const static
status_lookup_t pInsulationMonitorStatus[] =
{
    {
        Ins_Mon_Status_Unknown,
        "Unknown",
    },
    {
        Ins_Mon_Status_Alarm,
        "Alarm",
    },
    {
        Ins_Mon_Status_Ok,
        "Ok",
    }
};


#define INSMON_STATUS_LENGTH    ( sizeof(pInsulationMonitorStatus) / sizeof(pInsulationMonitorStatus[0]) )


const char *ForwardingStatus[] =
{
    "Off",
    "On"
};



unsigned short
im_property_get( unsigned long property )
{
    unsigned short usValue = 0;


    if( ( property >= IM_PROPERTY_START &&
          property <= IM_PROPERTY_END )
        ||
        ( property >= U712036_PROPERTY_START &&
          property <= U712036_PROPERTY_END ) )
    {
        usValue = InterCommDataGet(property);
    }

    return usValue;
}


void
im_property_string_get( unsigned long property, char *string )
{
    unsigned short  value = 0;

    value = im_property_get(property);

    switch( property )
    {
    case U712036_SUPPLY_VOLTAGE:
    case U712036_LEAK_VOLTAGE_1:
    case U712036_LEAK_VOLTAGE_2:
        /*
         * Voltage 123.4 V
         */
        if( value > 9999 )
            usprintf(string, "^^^");
        else
            usprintf(string, "%3d.%1d", value/10, value%10);
        break;

    case IM_LEAK_RESISTANCE_1:
    case IM_LEAK_RESISTANCE_2:
        //
        // Resistance 123.45 kOhm
        //
        if( value == 0xFFFF )
            usprintf(string, "%s", ">500");
        else
            usprintf(string, "%3d.%02d   ", value/100, value%100);
        break;

    case IM_RDIV_RESISTANCE:
        usprintf(string, "%3d.%1d", value/10, value%10);
        break;

    case IM_STATUS:
        if( value < INSMON_STATUS_LENGTH )
        {
            usprintf(string, "%s", pInsulationMonitorStatus[value].status);
        }
        else
        {
            usprintf(string, "!Err!, 0x%02X", value);
        }
        break;

    case IM_ALARM_RESISTANCE_SET:
        usprintf(string, "%d", value/100);
        break;

    case IM_ALARM_TIME_SET:
        usprintf(string, "%d", value);
        break;

    case IM_ALARM_FORWARD_ENABLE_SET:
        usprintf(string, "%s", ForwardingStatus[value]);
        break;

    case U712036_COMM_ERRORS:
        usprintf(string, "%d", value);
        break;

    case U712036_COMM_STATUS:
        usprintf(string, "%s", (value) ? "OK" : "FAULT");
        break;

    default:
        usprintf(string, "Error");
    }
}


void
im_property_validate( unsigned short usProperty, unsigned short *pusValue )
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
im_property_set( unsigned short property, unsigned short value )
{
    im_property_validate(property, &value);
    InterCommDataSet(property, value);
}


void
im_property_init( void )
{

}

