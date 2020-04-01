#include "inverter_property.h"

#if ( INVERTER_COUNT > 0 )

static const char *pcInverterStatusMessage[] =
{
	"Unknown",
	"Malfunc.",
	"tS short",
	"tS break",
	"Curr Lim",
	"auxV low",
	"! 350V !",
	"Overload",
	"Overheat",
	"Disabled",
	"Offline",
	"Online",
};

#define INVERTER_STATUS_MESSAGES_COUNT           ( sizeof(pcInverterStatusMessage) / sizeof(pcInverterStatusMessage[0]) )


unsigned short
InverterPropertyGet( unsigned short usUnit, unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= INVERTER_START &&
		usProperty  < INVERTER_END &&
		usUnit      < INVERTER_COUNT )
	{
		usValue = InterCommDataGet(INVERTER_OFFSET(usUnit) + usProperty);
	}

	return usValue;
}


void
InverterPropertyStringGet( unsigned short usUnit, unsigned short usProperty, char *pcString )
{
	unsigned short  usValue = InverterPropertyGet(usUnit, usProperty);

	switch( usProperty )
	{
	case INVERTER_INPUT_VOLTAGE:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case INVERTER_INPUT_CURRENT:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case INVERTER_OUTPUT_VOLTAGE:
		usprintf(pcString, "%d", usValue);
		break;
	case INVERTER_OUTPUT_CURRENT:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case INVERTER_OUTPUT_POWER:
		usValue /= 100;
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case INVERTER_OVERLOAD_CURRENT:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case INVERTER_TEMPERATURE_CONV:
	case INVERTER_TEMPERATURE_INT:
		if( (short)usValue < -60 )
			usprintf(pcString, "-");
		else
			usprintf(pcString, "%d", (short)usValue);
		break;
	case INVERTER_STATUS:
		usprintf(pcString, "%s", pcInverterStatusMessage[(usValue < INVERTER_STATUS_MESSAGES_COUNT) ? usValue : 0]);
		break;
	case INVERTER_COMM_ERROR_COUNTER:
		if( usValue > 999 )
		{
			usprintf(pcString, "^^^");
		}
		else
		{
			usprintf(pcString, "%d", usValue);
		}

		break;
	default:
		usprintf(pcString, "Err");
		break;
	}
}


unsigned short
InverterGroupPropertyGet( unsigned short usGroup, unsigned short usProperty )
{
	unsigned short usValue = 0;

	if( usProperty >= INVERTER_GROUP_START &&
		usProperty  < INVERTER_GROUP_END &&
		usGroup     < INVERTER_GROUP_COUNT )
	{
		usValue = InterCommDataGet(INVERTER_GROUP_OFFSET(usGroup) + usProperty);
	}

	return usValue;
}


void
InverterGroupPropertyStringGet( unsigned short usGroup, unsigned short usProperty, char *pcString )
{
	unsigned short  usValue = InverterGroupPropertyGet(usGroup, usProperty);

	switch( usProperty )
	{
	case INVERTER_GROUP_OUTPUT_VOLTAGE:
		usprintf(pcString, "%d", usValue);
		break;
	case INVERTER_GROUP_OUTPUT_CURRENT:
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	case INVERTER_GROUP_OUTPUT_POWER:
		usValue /= 100;
		usprintf(pcString, "%d.%1d", usValue/10, usValue%10);
		break;
	default:
		usprintf(pcString, "Err");
		break;
	}
}


void
InverterPropertyInit( void )
{

}

#endif /* INVERTER_COUNT > 0 */
