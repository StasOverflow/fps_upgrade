
#include "modbus_property.h"

#if ( INCLUDE_EXTMODBUS == 1 )

const char *baud_caption[] =
{
	"1200",
	"2400",
	"4800",
	"9600",
	"19200",
	"38400",
	"57600",
	"115200",
};

#define BAUDRATE_MESSAGES_COUNT                  ( sizeof(pcBaudRateMessage) / sizeof(pcBaudRateMessage[0]) )


const char *parity_caption[] =
{
	"NONE",
	"ODD",
	"EVEN",
};

#define PARITY_MESSAGES_COUNT                    ( sizeof(pcParityMessage) / sizeof(pcParityMessage[0]) )


const static tPropertyRange sPropertyRanges[] =
{
	{
		EXTMODBUS_BAUDRATE,

		{
			0,
			BAUDRATE_MESSAGES_COUNT - 1,
			1
		},
	},

	{
		EXTMODBUS_PARITY,

		{
			0,
			PARITY_MESSAGES_COUNT - 1,
			1
		},
	},

	{
		EXTMODBUS_SLAVE_ADDRESS,

		{
			1,
			EXTMODBUS_SLAVE_ADDR_MAX,
			1
		},
	},
};

#define MODBUS_PROPERTY_RANGES_COUNT             ( sizeof(sPropertyRanges) / sizeof(sPropertyRanges[0]) )


unsigned short
ModbusPropertyValueGet( unsigned short usProperty )
{
	return InterCommDataGet(usProperty);
}


void
ModbusPropertyStringGet( unsigned short usProperty, char *pcString )
{
	unsigned short usValue;

	usValue = ModbusPropertyValueGet(usProperty);

	switch( usProperty )
	{
	case EXTMODBUS_BAUDRATE:
		usprintf(pcString, "%s", baud_caption[(usValue < BAUDRATE_MESSAGES_COUNT) ? usValue : 0]);
		break;
	case EXTMODBUS_PARITY:
		usprintf(pcString, "%s", parity_caption[(usValue < PARITY_MESSAGES_COUNT) ? usValue : 0]);
		break;
	case EXTMODBUS_SLAVE_ADDRESS:
		usprintf(pcString, "%d", usValue);
		break;
	default:
		usprintf(pcString, "Error");
	}
}


void
ModbusPropertyStringGetFromValue( unsigned short usProperty, unsigned short usValue, char *pcString )
{
	switch( usProperty )
	{
	case EXTMODBUS_BAUDRATE:
		usprintf(pcString, "%s", baud_caption[(usValue < BAUDRATE_MESSAGES_COUNT) ? usValue : 0]);
		break;
	case EXTMODBUS_PARITY:
		usprintf(pcString, "%s", parity_caption[(usValue < PARITY_MESSAGES_COUNT) ? usValue : 0]);
		break;
	case EXTMODBUS_SLAVE_ADDRESS:
		usprintf(pcString, "%d", usValue);
		break;
	default:
		usprintf(pcString, "Error");
	}
}


void
ModbusPropertyValidate( unsigned short usProperty, unsigned short *pusValue )
{
	unsigned short usPropertyIndex;

	for( usPropertyIndex = 0; usPropertyIndex < MODBUS_PROPERTY_RANGES_COUNT; usPropertyIndex++ )
	{
		if( sPropertyRanges[usPropertyIndex].usProperty == usProperty )
		{
			break;
		}
	}

	if( usPropertyIndex < MODBUS_PROPERTY_RANGES_COUNT )
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
ModbusPropertySet( unsigned short usProperty, unsigned short usValue )
{
	ModbusPropertyValidate(usProperty, &usValue);
	InterCommDataSet(usProperty, usValue);
}


void
ModbusPropertyInit( void )
{

}


#endif /* INCLUDE_EXTMODBUS == 1 */
