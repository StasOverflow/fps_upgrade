
#include "debug_property.h"


const static tPropertyRange sPropertyRanges[] =
{
	{
		ACQM_VOLTAGE_COEFF,

		{
			0,
			65534,
			1
		}
	},
};

#define PROPERTY_RANGES_COUNT           ( sizeof(sPropertyRanges) / sizeof(sPropertyRanges[0]) )


unsigned short
DebugPropertyGet( unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= DEBUG_START &&
		usProperty  < DEBUG_END )
	{
		usValue = InterCommDataGet(usProperty);
	}

	return usValue;
}


void
DebugPropertyStringGet( unsigned short usProperty, char *pcString )
{
	unsigned short  usValue = DebugPropertyGet(usProperty);

	switch( usProperty )
	{
	case COMM_ROUND_RATIO:
	case COMM_ROUND_RATIO_MAX:
	case ACQM_COMM_ERRORS:
	case APS_COMM_ERRORS:
		usprintf(pcString, "%d", usValue);
		break;

	case ACQM_TEMP_1_VALUE:
	case ACQM_TEMP_2_VALUE:
	case ACQM_TEMP_3_VALUE:
		usprintf(pcString, "%d", (short)usValue);
		break;

	case ACQM_VOLTAGE_VALUE:
        usprintf(pcString, "%d", usValue);
//		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;

	case ACQM_VOLTAGE_COEFF:
		usprintf(pcString, "%d", (short)usValue);
		break;

	case ACQM_VOLTAGE_ADCV:
		usprintf(pcString, "%d.%02d", usValue/100, usValue%100);
		break;

	case ACQM_COMM_STATUS:
	case APS_COMM_STATUS:
		usprintf(pcString, "%s", (usValue) ? "OK" : "FAULT");
		break;

	case APS_INPUT_VOLTAGE:
	case APS_OUTPUT_VOLTAGE:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;

	default:
		usprintf(pcString, "Error");
	}
}


void
DebugPropertyValidate( unsigned short usProperty, long *plValue )
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
		if( *plValue >= sPropertyRanges[usPropertyIndex].sPropertyRange.lMin )
		{
			if( *plValue <= sPropertyRanges[usPropertyIndex].sPropertyRange.lMax )
			{
				*plValue -= *plValue % sPropertyRanges[usPropertyIndex].sPropertyRange.usStep;
			}
			else
			{
				*plValue = sPropertyRanges[usPropertyIndex].sPropertyRange.lMax;
			}
		}
		else
		{
			*plValue = sPropertyRanges[usPropertyIndex].sPropertyRange.lMin;
		}
	}
}


void
DebugPropertySet( unsigned short usProperty, unsigned short usValue )
{
	long lValue = (short)usValue;

	DebugPropertyValidate(usProperty, &lValue);
	InterCommDataSet(usProperty, lValue);
}


void
DebugPropertyInit( void )
{

}
