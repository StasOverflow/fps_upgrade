#include "ups_property.h"


static const char *pcUPSStatusMessage[] =
{
	"Unknown",
	"Alarm",
	"Ok"
};


static const char *pcVMStatusMessage[] =
{
    "Alarm",
    "Ok"
};



#define VM_STATUS_MESSAGES_COUNT                 ( sizeof(pcVMStatusMessage) / sizeof(pcVMStatusMessage[0]) )



#define UPS_STATUS_MESSAGES_COUNT                ( sizeof(pcUPSStatusMessage) / sizeof(pcUPSStatusMessage[0]) )


const static tPropertyRange sPropertyRanges[] =
{
    {
        SYSTEM_VM_VOLTAGE_MIN_THRESHOLD,

        {
            VM_VOLTAGE_MIN_THRESHOLD_SET_MIN,
            VM_VOLTAGE_MIN_THRESHOLD_SET_MAX,
            VM_VOLTAGE_MIN_THRESHOLD_SET_INC
        }
    },

    {
        SYSTEM_VM_VOLTAGE_MAX_THRESHOLD,

        {
            VM_VOLTAGE_MAX_THRESHOLD_SET_MIN,
            VM_VOLTAGE_MAX_THRESHOLD_SET_MAX,
            VM_VOLTAGE_MAX_THRESHOLD_SET_INC
        }
    },
	{
		SYSTEM_SERIAL_NUMBER_0,

		{
			0x4141, // AA
			0x5A5A, // ZZ
			1
		}
	},

	{
		SYSTEM_SERIAL_NUMBER_1,

		{
			0x3030, // 00
			0x3939, // 99
			1
		}
	},

	{
		SYSTEM_SERIAL_NUMBER_2,

		{
			0x3030, // 00
			0x3939, // 99
			1
		}
	},

	{
		SYSTEM_MODEL_MAJOR,

		{
			0,
			999,
			1
		}
	},

	{
		SYSTEM_MODEL_MINOR,

		{
			0,
			99,
			1
		}
	},
};

#define PROPERTY_RANGES_COUNT           ( sizeof(sPropertyRanges) / sizeof(sPropertyRanges[0]) )


unsigned short
UPSPropertyGet( unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= SYSTEM_START &&
		usProperty  < SYSTEM_END )
	{
		usValue = InterCommDataGet(usProperty);
	}

	return usValue;
}


void
UPSPropertyStringGet( unsigned short usProperty, char *pcString )
{
	unsigned short  usValue = UPSPropertyGet(usProperty);

	switch( usProperty )
	{
	case SYSTEM_TEMPERATURE_INTERNAL:
	case SYSTEM_TEMPERATURE_EXTERNAL:
		if( (short)usValue < -80 || (short)usValue > 120 )
			usprintf(pcString, "%s", "-");
		else
			usprintf(pcString, "%d", (short)usValue);
		break;
	case SYSTEM_STATUS:
	    usValue &= 0xff;

		usprintf(pcString, "%s", pcUPSStatusMessage[(usValue < UPS_STATUS_MESSAGES_COUNT) ? usValue : 0]);
		break;

    case SYSTEM_VM_VOLTAGE_MIN_THRESHOLD:
    case SYSTEM_VM_VOLTAGE_MAX_THRESHOLD:
        usprintf(pcString, "%d", usValue/10);
        break;
    case SYSTEM_VM_STATUS:
        usprintf(pcString, "%s", pcVMStatusMessage[(usValue < VM_STATUS_MESSAGES_COUNT) ? usValue : 0]);
        break;

	case SYSTEM_SERIAL_NUMBER_0:
		if( usValue == 0 )
		{
			usValue = ('Q' << 8) | 'A';
		}

		usprintf(pcString, "%c%c", usValue >> 8, usValue & 0xFF);

		break;

	case SYSTEM_SERIAL_NUMBER_1:
	case SYSTEM_SERIAL_NUMBER_2:
		if( usValue == 0 )
		{
			usValue = ('0' << 8) | '0';
		}

		usprintf(pcString, "%c%c", usValue >> 8, usValue & 0xFF);

		break;

	case SYSTEM_MODEL_MAJOR:
		usprintf(pcString, "%03d", usValue);
		break;

	case SYSTEM_MODEL_MINOR:
		usprintf(pcString, "%02d", usValue);
		break;

	default:
		usprintf(pcString, "Error");
		break;
	}
}


void
UPSPropertyStringGetFromValue( unsigned short usProperty, unsigned short usValue, char *pcString )
{
	switch( usProperty )
	{
	case SYSTEM_MODEL_MAJOR:
		usprintf(pcString, "%d", usValue);
		break;

	case SYSTEM_MODEL_MINOR:
		usprintf(pcString, "%02d", usValue);
		break;

	default:
		usprintf(pcString, "Error");
		break;
	}
}


void
UPSPropertyValidate( unsigned short usProperty, unsigned short *pusValue )
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
UPSPropertySet( unsigned short usProperty, unsigned short usValue )
{
	UPSPropertyValidate(usProperty, &usValue);
	InterCommDataSet(usProperty, usValue);
}


unsigned short
UPSPropertyStepGet( unsigned short usProperty )
{
	unsigned short usStep = 0;
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
		usStep = sPropertyRanges[usPropertyIndex].sPropertyRange.usStep;
	}

	return usStep;
}

void
UPSPropertyInit( void )
{

}
