
#include "psu_property.h"

#if ( PSU_COUNT > 0 )

static const char *pcPSUStatusMessage[] =
{
	"Unknown",
	"Malfunc.",
	"Out Fault",
	"Inp Fault",
	"Undercool",
	"Overheat",
	"Offline",
	"Online"
};

#define PSU_STATUS_MESSAGES_COUNT            ( sizeof(pcPSUStatusMessage) / sizeof(pcPSUStatusMessage[0]) )


static const char *pcPSUGroupStatusMessage[] =
{
	"Unknown",
	"Fault",
	"Warning",
	"Offline",
	"Online"
};

#define PSU_GROUP_STATUS_MESSAGES_COUNT    ( sizeof(pcPSUGroupStatusMessage) / sizeof(pcPSUGroupStatusMessage[0]) )


const static tPropertyRange sPropertyRanges[] =
{
    {
        PSU_GROUP_SETUP_VOLTAGE + PSU_GROUP_OFFSET(0),

        {
            PSU_GROUP_1_VOLTAGE_SET_MIN,
            PSU_GROUP_1_VOLTAGE_SET_MAX,
            PSU_GROUP_1_VOLTAGE_SET_INC
        }
    },

    {
        PSU_GROUP_SETUP_VOLTAGE_FAST + PSU_GROUP_OFFSET(0),

        {
            PSU_GROUP_1_VOLTAGE_SET_MIN,
            PSU_GROUP_1_VOLTAGE_SET_MAX,
            PSU_GROUP_1_VOLTAGE_SET_INC
        }
    },

    {
        PSU_GROUP_SETUP_VOLTAGE_HOLD + PSU_GROUP_OFFSET(0),

        {
            PSU_GROUP_1_VOLTAGE_SET_MIN,
            PSU_GROUP_1_VOLTAGE_SET_MAX,
            PSU_GROUP_1_VOLTAGE_SET_INC
        }
    },

    {
        PSU_GROUP_CURRENT_LIMIT + PSU_GROUP_OFFSET(0),

        {
            PSU_GROUP_1_CURRENT_LIMIT_SET_MIN,
            PSU_GROUP_1_CURRENT_LIMIT_SET_MAX,
            PSU_GROUP_1_CURRENT_LIMIT_SET_INC,
        }
    },

#if WITH_TCOMP
	{
		PSU_GROUP_TEMPERATURE_COMPENSATION + PSU_GROUP_OFFSET(0),

		{
			PSU_GROUP_1_TEMPERATURE_COMPENSATION_SET_MIN,
			PSU_GROUP_1_TEMPERATURE_COMPENSATION_SET_MAX,
			PSU_GROUP_1_TEMPERATURE_COMPENSATION_SET_INC
		}
	},
#endif
    {
        PSU_GROUP_SETUP_VOLTAGE + PSU_GROUP_OFFSET(1),

        {
            PSU_GROUP_2_VOLTAGE_SET_MIN,
            PSU_GROUP_2_VOLTAGE_SET_MAX,
            PSU_GROUP_2_VOLTAGE_SET_INC
        }
    },

    {
        PSU_GROUP_SETUP_VOLTAGE_FAST + PSU_GROUP_OFFSET(1),

        {
            PSU_GROUP_2_VOLTAGE_SET_MIN,
            PSU_GROUP_2_VOLTAGE_SET_MAX,
            PSU_GROUP_2_VOLTAGE_SET_INC
        }
    },

    {
        PSU_GROUP_SETUP_VOLTAGE_HOLD + PSU_GROUP_OFFSET(1),

        {
            PSU_GROUP_2_VOLTAGE_SET_MIN,
            PSU_GROUP_2_VOLTAGE_SET_MAX,
            PSU_GROUP_2_VOLTAGE_SET_INC
        }
    },

    {
        PSU_GROUP_CURRENT_LIMIT + PSU_GROUP_OFFSET(1),

        {
            PSU_GROUP_2_CURRENT_LIMIT_SET_MIN,
            PSU_GROUP_2_CURRENT_LIMIT_SET_MAX,
            PSU_GROUP_2_CURRENT_LIMIT_SET_INC,
        }
    },

#if WITH_TCOMP
    {
        PSU_GROUP_TEMPERATURE_COMPENSATION + PSU_GROUP_OFFSET(1),

        {
            PSU_GROUP_2_TEMPERATURE_COMPENSATION_SET_MIN,
            PSU_GROUP_2_TEMPERATURE_COMPENSATION_SET_MAX,
            PSU_GROUP_2_TEMPERATURE_COMPENSATION_SET_INC
        }
    },
#endif
};

#define PROPERTY_RANGES_COUNT           ( sizeof(sPropertyRanges) / sizeof(sPropertyRanges[0]) )


unsigned short
PSUPropertyGet( unsigned short usUnit, unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= PSU_START &&
		usProperty  < PSU_END &&
		usUnit      < PSU_COUNT )
	{
		usValue = InterCommDataGet(PSU_OFFSET(usUnit) + usProperty);
	}

	return usValue;
}


void
PSUPropertyStringGet( unsigned short usUnit, unsigned short usProperty, char *pcString )
{
	unsigned short  usValue = PSUPropertyGet(usUnit, usProperty);

	switch( usProperty )
	{
	case PSU_SETUP_VOLTAGE:
		usprintf(pcString, "%d.%02d", usValue/100, usValue%100);
		break;

	case PSU_OUTPUT_VOLTAGE:
		if( usUnit >= PSU_GROUP_2_START_INDEX &&
			usUnit <= PSU_GROUP_2_END_INDEX	)
		{
            usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		}
		else
		{
            usprintf(pcString, "%d", usValue);
		}

		break;
	case PSU_OUTPUT_CURRENT:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case PSU_STATUS:
	    if( usValue < PSU_STATUS_MESSAGES_COUNT )
	    {
	        usprintf(pcString, "%s", pcPSUStatusMessage[usValue]);
	    }
	    else
	    {
	        usprintf(pcString, "0x%02X", usValue);
	    }
		break;
	case PSU_TEMPERATURE:
		if( (short)usValue < -60 )
			usprintf(pcString, "%s", "-");
		else
			usprintf(pcString, "%d", (short)usValue);
		break;
	case PSU_COMM_ERROR_COUNTER:
		usprintf(pcString, "%d", usValue);
		break;
	default:
		usprintf(pcString, "Error");
		break;
	}
}


unsigned short
PSUGroupPropertyGet( unsigned short usGroup, unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= PSU_GROUP_START &&
		usProperty  < PSU_GROUP_END &&
		usGroup     < PSU_GROUP_COUNT )
	{
		usValue = InterCommDataGet(PSU_GROUP_OFFSET(usGroup) + usProperty);
	}

	return usValue;
}


void
PSUGroupPropertyStringGet( short unsigned usGroup, short unsigned usProperty, char *pcString )
{
	unsigned short  usValue = PSUGroupPropertyGet(usGroup, usProperty);

	switch( usProperty )
	{
	case PSU_GROUP_OUTPUT_VOLTAGE:
	    if( usGroup == 1 )
	    {
	        usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
	    }
	    else
	    {
	        usprintf(pcString, "%d", usValue);
	    }
		break;
	case PSU_GROUP_OUTPUT_CURRENT:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case PSU_GROUP_STATUS:
        if( usValue < PSU_STATUS_MESSAGES_COUNT )
        {
            usprintf(pcString, "%s", pcPSUGroupStatusMessage[usValue]);
        }
        else
        {
            usprintf(pcString, "0x%02X", usValue);
        }
		break;
	case PSU_GROUP_SETUP_VOLTAGE:
	case PSU_GROUP_SETUP_VOLTAGE_FAST:
//	case PSU_GROUP_SETUP_VOLTAGE_HOLD:

		if( usGroup == 0 )
		{
			usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		}
		else
		{
            usValue /= 10;
			usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		}

		break;
	case PSU_GROUP_ENABLE_SET:
	    usprintf(pcString, usValue ? "On" : "Off");
	    break;
	case PSU_GROUP_CURRENT_LIMIT:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case PSU_GROUP_TEMPERATURE_COMPENSATION:
	    if( usValue >= 1000 )
	    {
	        usprintf(pcString, "%d.%1d", usValue/1000, usValue%1000);
	    }
	    else
	    {
            usprintf(pcString, "0.%1d",  usValue/100);
	    }
		break;
	default:
		usprintf(pcString, "Error");
		break;
	}
}


tBoolean
PSUGroupPropertyIsValid( unsigned short usGroup, unsigned short usProperty, unsigned short usValue )
{
	tBoolean isValid = true;
	unsigned short usPropertyIndex;

	for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
	{
		if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + PSU_GROUP_OFFSET(usGroup) )
		{
			if( usPropertyIndex < PROPERTY_RANGES_COUNT )
			{
				if( ((short)usValue < sPropertyRanges[usPropertyIndex].sPropertyRange.lMin) ||
					((short)usValue > sPropertyRanges[usPropertyIndex].sPropertyRange.lMax) )
				{
					isValid = false;
				}
			}

			break;
		}
	}

	return isValid;
}


void
PSUGroupPropertySet( unsigned short usGroup, unsigned short usProperty, unsigned short usValue )
{
	unsigned short usPropertyIndex;


	for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
	{
		if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + PSU_GROUP_OFFSET(usGroup) )
		{
			break;
		}
	}

	if( usPropertyIndex < PROPERTY_RANGES_COUNT )
	{
		if( (short)usValue >= sPropertyRanges[usPropertyIndex].sPropertyRange.lMin )
		{
			if( (short)usValue <= sPropertyRanges[usPropertyIndex].sPropertyRange.lMax )
			{
				usValue -= usValue % sPropertyRanges[usPropertyIndex].sPropertyRange.usStep;
			}
			else
			{
				usValue = sPropertyRanges[usPropertyIndex].sPropertyRange.lMax;
			}
		}
		else
		{
			usValue = sPropertyRanges[usPropertyIndex].sPropertyRange.lMin;
		}
	}

	InterCommDataSet(PSU_GROUP_OFFSET(usGroup) + usProperty, usValue);
}


unsigned short
PSUGroupPropertyStepGet( unsigned short usGroup, unsigned short usProperty )
{
	unsigned short usStep = 0;
	unsigned short usPropertyIndex;

	if( usGroup < PSU_GROUP_COUNT )
	{
		for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
		{
			if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + PSU_GROUP_OFFSET(usGroup) )
			{
				break;
			}
		}

		if( usPropertyIndex < PROPERTY_RANGES_COUNT )
		{
			usStep = sPropertyRanges[usPropertyIndex].sPropertyRange.usStep;
		}
	}

	return usStep;
}


void PSUPropertyInit( void )
{

}

#endif /* PSU_COUNT > 0 */
