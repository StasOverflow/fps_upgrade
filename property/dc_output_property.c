#include "dc_output_property.h"

#if ( OUTPUT_DC_COUNT > 0 )

static const char *pcDCOutputStatusMessage[] =
{
	"Unknown",
	"Fault",
	"Moderate",
	"Offline",
	"Online",
};

#define OUTPUT_DC_STATUS_MESSAGES_COUNT           ( sizeof(pcDCOutputStatusMessage) / sizeof(pcDCOutputStatusMessage[0]) )


const static tPropertyRange sPropertyRanges[] =
{
	{
		0,
		{
			0,
			65534,
			1
		}
	},
};

#define PROPERTY_RANGES_COUNT           ( sizeof(sPropertyRanges) / sizeof(sPropertyRanges[0]) )


unsigned short
DCOutputPropertyGet( unsigned short usChannel, unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= OUTPUT_DC_START &&
		usProperty  < OUTPUT_DC_END &&
		usChannel   < OUTPUT_DC_COUNT )
	{
		usValue = InterCommDataGet(OUTPUT_DC_OFFSET(usChannel) + usProperty);
	}

	return usValue;
}


void
DCOutputPropertyStringGet( unsigned short usChannel, unsigned short usProperty, char *pcString )
{
	unsigned short  usValue = DCOutputPropertyGet(usChannel, usProperty);
    signed short    sValue;
    signed short  fraction;


    switch( usProperty )
    {
    case OUTPUT_DC_VOLTAGE:
      if( usChannel == 1 )
      {
          usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
      }
      else
      {
          usprintf(pcString, "%d", usValue);
      }

        break;
    case OUTPUT_DC_CURRENT:
        sValue = (signed short) (usValue / 10);

        fraction = (signed short) (usValue % 10);
        fraction = fraction < 0 ? -fraction : fraction;

        usprintf(pcString, "%d.%01d", sValue, fraction);
		break;
	case OUTPUT_DC_POWER:
	    usValue /= 10;
		usprintf(pcString, "%d.%02d", usValue/100, usValue%100);
		break;
	case OUTPUT_DC_STATUS:
	    if( usValue < OUTPUT_DC_STATUS_MESSAGES_COUNT )
	    {
	        usprintf(pcString, "%s", pcDCOutputStatusMessage[usValue]);
	    }
	    else
	    {
            usprintf(pcString, "0x%02X", usValue);
	    }
		break;
	default:
		usprintf(pcString, "Error");
		break;
	}
}


void
DCOutputPropertyValidate( unsigned short usChannel, unsigned short usProperty, unsigned short *pusValue )
{
	unsigned short usPropertyIndex;

	for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
	{
		if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + OUTPUT_DC_OFFSET(usChannel) )
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
DCOutputPropertySet( unsigned short usChannel, unsigned short usProperty, unsigned short usValue )
{
	DCOutputPropertyValidate(usChannel, usProperty, &usValue);
	InterCommDataSet(OUTPUT_DC_OFFSET(usChannel) + usProperty, usValue);
}


unsigned short
DCOutputPropertyStepGet( unsigned short usChannel, unsigned short usProperty )
{
	unsigned short usStep = 0;
	unsigned short usPropertyIndex;

	if( usChannel < OUTPUT_DC_COUNT )
	{
		for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
		{
			if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + OUTPUT_DC_OFFSET(usChannel) )
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
DCOutputPropertyInit( void )
{

}


#endif /* ( OUTPUT_DC_COUNT > 0 ) */
