#include "battery_property.h"

#if ( BATTERY_GROUP_COUNT > 0 )

static const char *pcBatteryStatusMessages[] =
{
		"Unknown",
		"Overheat",
		"Depleted",
		"Disconnected",
		"Low Level",
		"Discharge",
		"Fast",
		"Hold",
		"Charge",
};

#define BATTERY_STATUS_MESSAGES_COUNT            ( sizeof(pcBatteryStatusMessages) / sizeof(pcBatteryStatusMessages[0]) )


static const char *pcBatteryLevelMessage[] =
{
		"Unknown",
		"Depleted",
		"Low",
		"Medium",
		"High",
		"Too High",
};

#define BATTERY_LEVEL_MESSAGES_COUNT             ( sizeof(pcBatteryLevelMessage) / sizeof(pcBatteryLevelMessage[0]) )


const static tPropertyRange sPropertyRanges[] =
{
	{
		BATTERY_CURRENT_LIMIT + BATTERY_OFFSET(0),

		{
			BATTERY_CURRENT_LIMIT_SET_MIN,
			BATTERY_CURRENT_LIMIT_SET_MAX,
			BATTERY_CURRENT_LIMIT_SET_INC
		}
	},

	{
		BATTERY_LOW_VOLTAGE_ALARM_LEVEL + BATTERY_OFFSET(0),

		{
			BATTERY_ALARM_VOLTAGE_SET_MIN,
			BATTERY_ALARM_VOLTAGE_SET_MAX,
			BATTERY_ALARM_VOLTAGE_SET_INC
		}
	},
};

#define PROPERTY_RANGES_COUNT           ( sizeof(sPropertyRanges) / sizeof(sPropertyRanges[0]) )


unsigned short
BatteryPropertyGet( unsigned short usBatt, unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= BATTERY_START &&
		usProperty  < BATTERY_END &&
		usBatt      < BATTERY_GROUP_COUNT )
	{
		usValue = InterCommDataGet(BATTERY_OFFSET(usBatt) + usProperty);
	}

	return usValue;
}


void
BatteryPropertyStringGet( unsigned short usBatt, unsigned short usProperty, char *pcString )
{
	unsigned short  usValue = BatteryPropertyGet(usBatt, usProperty);
	         short  sSign = 0;

	switch( usProperty )
	{
	case BATTERY_VOLTAGE:
        usprintf(pcString, "%d", usValue);
//		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case BATTERY_CURRENT:
#if 0
		//
		// Rounding...
		//
		usValue += ( (short)usValue < 0 ) ? -5 : 5;
		usValue = (short)usValue / 10;
#endif
		if( (short)usValue < 0 )
		{
			sSign = 1;
			usValue = -usValue;
		}

#if 0
		usprintf(pcString, "%s%d", ( sSign ) ? "-" : "", usValue);
#else
		usprintf(pcString, "%s%d.%1d", ( sSign ) ? "-" : "", usValue/10, usValue%10);
#endif
		break;

	case BATTERY_TEMPERATURE:
		if( (short)usValue < -60 )
			usprintf(pcString, "%s", "-");
		else
			usprintf(pcString, "%d", (short)usValue);
		break;
	case BATTERY_LEVEL:
		usprintf(pcString, "%s", pcBatteryLevelMessage[usValue]);
		break;
	case BATTERY_USED_CAPACITY:
	case BATTERY_FULL_CAPACITY:
	    if( usValue > 9999 )
	    {
            usprintf(pcString, "Error");
	    }
	    else
	    {
	        usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
	    }
		break;

	case BATTERY_CURRENT_NEG:
	case BATTERY_CURRENT_POS:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;

	case BATTERY_CURRENT_NEG_ADCV:
	case BATTERY_CURRENT_POS_ADCV:
		usprintf(pcString, "%d.%02d", usValue/100, usValue%100);
		break;

	case BATTERY_VOLTAGE_SETUP:
		usprintf(pcString, "%d.%02d", usValue/10, usValue%10);
		break;

	case BATTERY_CURRENT_LIMIT:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;

	case BATTERY_CURRENT_CALIBRATION_COEFF_NEG:
	case BATTERY_CURRENT_CALIBRATION_COEFF_POS:
	case BATTERY_CURRENT_CALIBRATION_OFFSET_NEG:
	case BATTERY_CURRENT_CALIBRATION_OFFSET_POS:
		usprintf(pcString, "%d", usValue);
		break;

	case BATTERY_LOW_VOLTAGE_ALARM_LEVEL:
        usprintf(pcString, "%d", usValue);
//		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;

	case BATTERY_VC_ENABLE:
	case BATTERY_CC_ENABLE:
		usprintf(pcString, "%d", usValue);
		break;

	case BATTERY_VC_PID_ERROR:
	case BATTERY_VC_PID_KP:
	case BATTERY_VC_PID_KI:
	case BATTERY_VC_PID_KD:
	case BATTERY_CC_PID_ERROR:
	case BATTERY_CC_PID_KP:
	case BATTERY_CC_PID_KI:
	case BATTERY_CC_PID_KD:
		usprintf(pcString, "%d", (int16_t)usValue);
		break;

	case BATTERY_PID_DT:
		usprintf(pcString, "%d.%03d", usValue/1000, usValue%1000);
		break;

	case BATTERY_STATUS:
	    if( usValue < BATTERY_STATUS_MESSAGES_COUNT )
	    {
	        usprintf(pcString, "%s", pcBatteryStatusMessages[usValue]);
	    }
	    else
	    {
	        usprintf(pcString, "Error!!");
	    }
//        usprintf(pcString, "0x%02X", pcString);
		break;

	case BATTERY_FLAGS:
		usprintf(pcString, "0x%02x", usValue);
		break;

	default:
		usprintf(pcString, "Error");
		break;
	}
}


void
BatteryPropertyValidate( unsigned short usBatt, unsigned short usProperty, unsigned short *pusValue )
{
	unsigned short usPropertyIndex;

	for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
	{
		if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + BATTERY_OFFSET(usBatt) )
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


unsigned char
BatteryPropertyIsValid( unsigned short usBatt, unsigned short usProperty, unsigned short usValue )
{
	unsigned short usPropertyIndex;

	for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
	{
		if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + BATTERY_OFFSET(usBatt) )
		{
			break;
		}
	}

	if( usPropertyIndex < PROPERTY_RANGES_COUNT )
	{
		if( ((short)usValue >= sPropertyRanges[usPropertyIndex].sPropertyRange.lMin) &&
			((short)usValue <= sPropertyRanges[usPropertyIndex].sPropertyRange.lMax) )
		{
			return 1;
		}
	}

	return 0;
}


void
BatteryPropertySet( unsigned short usBatt, unsigned short usProperty, unsigned short usValue )
{
	BatteryPropertyValidate(usBatt, usProperty, &usValue);
	InterCommDataSet(BATTERY_OFFSET(usBatt) + usProperty, usValue);
}


unsigned short
BatteryPropertyStepGet( unsigned short usBatt, unsigned short usProperty )
{
	unsigned short usStep = 0;
	unsigned short usPropertyIndex;

	if( usBatt < BATTERY_GROUP_COUNT )
	{
		for( usPropertyIndex = 0; usPropertyIndex < PROPERTY_RANGES_COUNT; usPropertyIndex++ )
		{
			if( sPropertyRanges[usPropertyIndex].usProperty == usProperty + BATTERY_OFFSET(usBatt) )
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
BatteryPropertyInit( void )
{

}

#endif /* BATTERY_GROUP_COUNT > 0 */
