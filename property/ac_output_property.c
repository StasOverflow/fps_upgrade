#include "ac_output_property.h"

#if ( OUTPUT_AC_PHASE_COUNT > 0 )

static const char *pcACOutputFeederStatusMessage[] =
{
	"Unknown",
	"Overload",
	"Fault",
	"Off",
	"Ok"
};

#define OUTPUT_AC_GROUP_STATUS_MESSAGES_COUNT   ( sizeof(pcACOutputFeederStatusMessage) / sizeof(pcACOutputFeederStatusMessage[0]) )


static const char *pcACOutputSourceMessage[] =
{
	"None",
	"Inverters",
	"Line",
};

#define OUTPUT_AC_SOURCE_MESSAGES_COUNT          ( sizeof(pcACOutputSourceMessage) / sizeof(pcACOutputSourceMessage[0]) )


const static tPropertyRange sPropertyRanges[] =
{
	{
		0,

		{
			0,
			0,
			0
		}
	},
};

#define PROPERTY_RANGES_COUNT             ( sizeof(sPropertyRanges) / sizeof(sPropertyRanges[0]) )


unsigned short
ACOutputPhasePropertyGet( unsigned short usPhase, unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= OUTPUT_AC_PHASE_START &&
		usProperty  < OUTPUT_AC_PHASE_END &&
		usPhase     < OUTPUT_AC_PHASE_COUNT )
	{
		usValue = InterCommDataGet(OUTPUT_AC_PHASE_OFFSET(usPhase) + usProperty);
	}

	return usValue;
}


void
ACOutputPhasePropertyStringGet( unsigned short usPhase, unsigned short usProperty, char *pcString )
{
	unsigned short  usValue = ACOutputPhasePropertyGet(usPhase, usProperty);

	switch( usProperty )
	{
	case OUTPUT_AC_VOLTAGE:
		usprintf(pcString, "%d", usValue);
		break;
	case OUTPUT_AC_CURRENT:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case OUTPUT_AC_POWER:
		//
		// Rounding...
		//
		usValue += 50;
		usValue /= 100;
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	default:
		usprintf(pcString, "Error");
		break;
	}
}


unsigned short
ACOutputFeederPropertyGet( unsigned short usFeeder, unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= OUTPUT_AC_GROUP_START &&
		usProperty  < OUTPUT_AC_GROUP_END &&
		usFeeder    < OUTPUT_AC_GROUP_COUNT )
	{
		usValue = InterCommDataGet(OUTPUT_AC_GROUP_OFFSET(usFeeder) + usProperty);
	}

	return usValue;
}


void
ACOutputFeederPropertyStringGet( unsigned short usFeeder, unsigned short usProperty, char *pcString )
{
	unsigned short  usValue = ACOutputFeederPropertyGet(usFeeder, usProperty);

	switch( usProperty )
	{
	case OUTPUT_AC_SOURCE:
		usprintf(pcString, "%s", pcACOutputSourceMessage[(usValue < OUTPUT_AC_SOURCE_MESSAGES_COUNT) ? usValue : 0]);
		break;
	case OUTPUT_AC_STATUS:
		usprintf(pcString, "%s", pcACOutputFeederStatusMessage[(usValue < OUTPUT_AC_GROUP_STATUS_MESSAGES_COUNT) ? usValue : 0]);
		break;
	default:
		usprintf(pcString, "Error");
		break;
	}
}



void
ACOutputFeederPropertyValidate( unsigned short usFeeder, unsigned short usProperty, unsigned short *pusValue )
{
	unsigned short usPropertyIndex;

	for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
	{
		if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + OUTPUT_AC_GROUP_OFFSET(usFeeder) )
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
ACOutputFeederPropertySet( unsigned short usFeeder, unsigned short usProperty, unsigned short usValue )
{
	ACOutputFeederPropertyValidate(usFeeder, usProperty, &usValue);
	InterCommDataSet(OUTPUT_AC_GROUP_OFFSET(usFeeder) + usProperty, usValue);
}


unsigned short
ACOutputFeederPropertyStepGet( unsigned short usFeeder, unsigned short usProperty )
{
	unsigned short usStep = 0;
	unsigned short usPropertyIndex;

	if( usFeeder < OUTPUT_AC_GROUP_COUNT )
	{
		for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
		{
			if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + OUTPUT_AC_GROUP_OFFSET(usFeeder) )
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
ACOutputPropertyInit( void )
{

}


#endif /* ( OUTPUT_AC_PHASE_COUNT > 0 ) */
