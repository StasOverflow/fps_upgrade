// Scheduler includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include "drivers/modbus_slave.h"
#include "modbus_register_table.h"

#include "ext_comm.h"

#include "inter_comm.h"
#include "modbus_property.h"

#if ( INCLUDE_EXTMODBUS == 1 )

typedef struct
{
	unsigned short usRegAddr;

	struct
	{
		long lMin;
		long lMax;

	} sRange;

} tRWDataItem;


static const unsigned short usExtCommROData[] =
{
};

#define EXT_COMM_RO_DATA_COUNT      ( sizeof(usExtCommROData) / \
                                      sizeof(usExtCommROData[0]) )


static const tRWDataItem usExtCommRWData[] =
{
};

#define EXT_COMM_RW_DATA_COUNT      ( sizeof(usExtCommRWData) / \
                                      sizeof(usExtCommRWData[0]) )


void
ExtCommSetupUpdate( void )
{
#if 0
	unsigned long  ulBaudRate;
	unsigned short usParity;
	unsigned char  ucSlaveID;
	         char  pcString[16];

	ModbusPropertyStringGet(EXTMODBUS_BAUDRATE, pcString);

	ulBaudRate = ustrtoul(pcString, 0, 10);

	ModbusPropertyStringGet(EXTMODBUS_PARITY, pcString);

	switch( pcString[0] )
	{
	case 'N':
	case 'n':
		usParity = UART_PARITY_NONE;
		break;

	case 'O':
	case 'o':
		usParity = UART_PARITY_ODD;
		break;

	case 'E':
	case 'e':
		usParity = UART_PARITY_EVEN;
		break;

	default:
		usParity = UART_PARITY_NONE;
		break;
	}

	ucSlaveID = ModbusPropertyValueGet(EXTMODBUS_SLAVE_ADDRESS);

	ModbusSlaveConfigure(ulBaudRate, usParity, ucSlaveID);
#endif
}


unsigned short
ExtCommDataGet( unsigned short usIndex )
{
	unsigned short usAllowed = 0;
	unsigned short usValue = 0;
	unsigned short usReg = 0;

	if( usIndex < EXT_COMM_RO_DATA_COUNT )
	{
		usReg = usExtCommROData[usIndex];

		usValue = InterCommDataGet(usReg);

		usAllowed = 1;
	}
	else if( usIndex < (EXT_COMM_RO_DATA_COUNT +
			            EXT_COMM_RW_DATA_COUNT) )
	{
		usReg = usExtCommRWData[usIndex - EXT_COMM_RO_DATA_COUNT].usRegAddr;

		usValue = InterCommDataGet(usReg);

		usAllowed = 1;
	}

	if( usAllowed )
	{
		switch( usReg )
		{
		default:
			break;
		}
	}

	return usValue;
}


unsigned short
ExtCommDataSet( unsigned short usIndex, unsigned short usRegValue )
{
	unsigned short usAccepted = 0;
	unsigned short usReg = 0;
	long           lMin = 0;
	long           lMax = 0;

	if( (usIndex >= EXT_COMM_RO_DATA_COUNT) &&
		(usIndex < (EXT_COMM_RO_DATA_COUNT +
				    EXT_COMM_RW_DATA_COUNT)) )
	{
		usIndex -= EXT_COMM_RO_DATA_COUNT;

		usReg = usExtCommRWData[usIndex].usRegAddr;

		lMin = usExtCommRWData[usIndex].sRange.lMin;
		lMax = usExtCommRWData[usIndex].sRange.lMax;

		switch( usReg )
		{
		default:
			break;
		}

		if( (usRegValue >= lMin) &&
			(usRegValue <= lMax) )
		{
			InterCommDataSet(usReg, usRegValue);

			usAccepted = 1;
		}
	}

	return usAccepted;
}


static void
ext_comm_proc( void )
{
    static tModbusRegMap sModbusRegMap =
    {
        .usRegStart = 0,
        .usRegCnt   = EXT_COMM_RO_DATA_COUNT + EXT_COMM_RW_DATA_COUNT,
        .RegRead    = ExtCommDataGet,
        .RegWrite   = ExtCommDataSet
    };

    static enum {
        Ext_Comm_Notify_Await,
        Ext_Comm_Init,
        Ext_Comm_Wait_Till_Ready,
        Ext_Comm_Setup_Update,
        Ext_Comm_Execute,
    } stage = Ext_Comm_Notify_Await;

    switch( stage )
    {
    case Ext_Comm_Notify_Await:
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        stage = Ext_Comm_Init;
        break;


    case Ext_Comm_Init:
        ModbusSlaveInit();

        ModbusSlaveRegMapSet(&sModbusRegMap);

        stage = Ext_Comm_Wait_Till_Ready;
        break;

    case Ext_Comm_Wait_Till_Ready:
        while( !InterCommDataReady() )
        {
            taskYIELD();
        }

        stage = Ext_Comm_Setup_Update;
        break;

    case Ext_Comm_Setup_Update:
        ExtCommSetupUpdate();
        stage = Ext_Comm_Setup_Update;
        break;

    case Ext_Comm_Execute:
        ModbusSlaveProc();
        break;

    default:
        break;
    }

    vTaskDelay(1);
}


void
ExtCommTask( void *pvParameters )
{
    for( ;; )
    {
        ext_comm_proc();
    }
}

#endif
