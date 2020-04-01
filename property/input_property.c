#include "input_property.h"

#if ( INPUT_PHASE_COUNT > 0 )

static const char *pcInputCommonStatusMessage[] =
{
	"Unknown",
	"OFF",
	"Out Of Range",
	"Phase seq. wrong",
	"Syncing...",
	"Ready",
	"Ok"
};

#define INPUT_COMMON_STATUS_MESSAGES_COUNT       ( sizeof(pcInputCommonStatusMessage) / sizeof(pcInputCommonStatusMessage[0]) )

const static status_lookup_t pInputStatusMessage[] =
{
    {
        Input_Status_Fault,
        "Fault",
    },
    {
        Input_Status_Ok,
        "Ok",
    },
};

#define INPUT_STATUS_MESSAGES_COUNT       ( sizeof(pInputStatusMessage) / sizeof(pInputStatusMessage[0]) )

const static tPropertyRange sPropertyRanges[] =
{
	{
		INPUT_CURRENT_CALIBRATION_COEFF + INPUT_PHASE_OFFSET(0),

		{
			0,
			65534,
			1
		}
	},

	{
		INPUT_CURRENT_CALIBRATION_COEFF + INPUT_PHASE_OFFSET(1),

		{
			0,
			65534,
			1
		}
	},

	{
		INPUT_CURRENT_CALIBRATION_COEFF + INPUT_PHASE_OFFSET(2),

		{
			0,
			65534,
			1
		}
	},

	{
		INPUT_CURRENT_CALIBRATION_COEFF + INPUT_PHASE_OFFSET(3),

		{
			0,
			65534,
			1
		}
	},

	{
		INPUT_CURRENT_CALIBRATION_COEFF + INPUT_PHASE_OFFSET(4),

		{
			0,
			65534,
			1
		}
	},

	{
		INPUT_CURRENT_CALIBRATION_COEFF + INPUT_PHASE_OFFSET(5),

		{
			0,
			65534,
			1
		}
	},
};

#define PROPERTY_RANGES_COUNT             ( sizeof(sPropertyRanges) / sizeof(sPropertyRanges[0]) )

unsigned short
InputPhasePropertyGet( unsigned short usPhase, unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= INPUT_PHASE_START &&
		usProperty  < INPUT_PHASE_END &&
		usPhase     < INPUT_PHASE_COUNT )
	{
		usValue = InterCommDataGet(INPUT_PHASE_OFFSET(usPhase) + usProperty);
	}

	return usValue;
}


void
InputPhasePropertyStringGet( unsigned short usPhase, unsigned short usProperty, char *pcString )
{
	unsigned short  usValue = InputPhasePropertyGet(usPhase, usProperty);

	switch( usProperty )
	{
	case INPUT_VOLTAGE:
		usprintf(pcString, "%d", usValue);
		break;
	case INPUT_CURRENT:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case INPUT_POWER:
		usValue /= 10;
		usprintf(pcString, "%d.%02d", usValue/100, usValue%100);
		break;
	case INPUT_CURRENT_ADCV:
		usprintf(pcString, "%d.%02d", usValue/100, usValue%100);
		break;
	case INPUT_CURRENT_CALIBRATION_COEFF:
		usprintf(pcString, "%d", usValue);
		break;
    case INPUT_STATUS:
        if( usValue < INPUT_STATUS_MESSAGES_COUNT )
        {
            usprintf(pcString, "%s", pInputStatusMessage[usValue].status);
        }
        else
        {
            usprintf(pcString, "0x%02X", usValue);
        }
        break;
	default:
		usprintf(pcString, "Error,%d", usProperty);
		break;
	}
}


unsigned short
InputFeederPropertyGet( unsigned short usFeeder, unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= INPUT_FEEDER_START &&
		usProperty  < INPUT_FEEDER_END &&
		usFeeder    < INPUT_FEEDER_COUNT )
	{
		usValue = InterCommDataGet(INPUT_FEEDER_OFFSET(usFeeder) + usProperty);
	}

	return usValue;
}


void
InputFeederPropertyStringGet( unsigned short usFeeder, unsigned short usProperty, char *pcString )
{
	unsigned short  usValue = InputFeederPropertyGet(usFeeder, usProperty);

	switch( usProperty )
	{
	case INPUT_FEEDER_STATUS:
		usprintf(pcString, "%s", pcInputCommonStatusMessage[(usValue < INPUT_COMMON_STATUS_MESSAGES_COUNT) ? usValue : 0]);
		break;
	default:
		usprintf(pcString, "Err");
		break;
	}
}

void
InputPhasePropertyValidate( unsigned short usPhase, unsigned short usProperty, unsigned short *pusValue )
{
	unsigned short usPropertyIndex;

	for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
	{
		if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + INPUT_PHASE_OFFSET(usPhase) )
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
InputPhasePropertySet( unsigned short usPhase, unsigned short usProperty, unsigned short usValue )
{
	InputPhasePropertyValidate(usPhase, usProperty, &usValue);
	InterCommDataSet(INPUT_PHASE_OFFSET(usPhase) + usProperty, usValue);
}


unsigned short
InputPhasePropertyStepGet( unsigned short usPhase, unsigned short usProperty )
{
	unsigned short usStep = 0;
	unsigned short usPropertyIndex;

	if( usPhase < INPUT_PHASE_COUNT )
	{
		for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
		{
			if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + INPUT_PHASE_OFFSET(usPhase) )
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


void
InputPropertyInit( void )
{

}

#endif /* INPUT_PHASE_COUNT > 0 */
