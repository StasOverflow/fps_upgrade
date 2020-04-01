#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "modbus_slave.h"
#include "drivers/uart.h"
#include "lib/crc.h"

#define MBF_READ_HOLDING_REGISTERS          0x03
#define MBF_READ_INPUT_REGISTERS            0x04
#define MBF_WRITE_SINGLE_REGISTER           0x06
#define MBF_WRITE_MULTIPLE_REGISTERS        0x10
#define MBF_READ_WRITE_MULTIPLE_REGISTERS   0x17
#define MBF_REPORT_SLAVE_ID                 0x11

#define MB_EXCEPTION_NONE                   0x00
#define MB_EXCEPTION_ILLEGAL_FUNCTION       0x01
#define MB_EXCEPTION_ILLEGAL_DATA_ADDRESS   0x02
#define MB_EXCEPTION_ILLEGAL_DATA_VALUE     0x03

#define GETU16(var, ptr)          \
do                                \
{                                 \
	(var) =  *(ptr) << 8;         \
	(var) |= *((ptr) + 1) & 0xFF; \
}                                 \
while(0)

#define SETU16(ptr, var)          \
do                                \
{                                 \
	*(ptr) = (var) >> 8;		  \
	*((ptr) + 1) = (var) & 0xFF;  \
}                                 \
while(0)

#define BUFFER_SIZE                 ( 256 )

static unsigned char  pucModbusBuffer[BUFFER_SIZE];
static portTickType   xModbusCharTimeout;
static unsigned char  ucModbusSlaveID;
static tModbusRegMap  sModbusRegMap;

static void
ModbusSend( unsigned char *pucData, unsigned short usBytesCnt )
{
	GPIO_WriteBit(GPIOC, GPIO_Pin_12, Bit_SET);

	vTaskDelay(1);

	for( int i = 0; i < usBytesCnt; i++ )
	{
		if( !UART3_SendByte(pucData[i]) )
		{
			taskYIELD();
		}
	}

	while( !UART3_IsTransmittionComplete() )
	{
		taskYIELD();
	}

	vTaskDelay(1);

	GPIO_WriteBit(GPIOC, GPIO_Pin_12, Bit_RESET);
}


static unsigned short
ModbusRecv( unsigned char *pucData )
{
	unsigned short usBytesCnt = 0;
	portTickType   xTimeout;
	unsigned char  ucByte;

	UART3_Flush();

	while( !UART3_RecvByte(&ucByte) )
	{
		taskYIELD();
	}

	pucData[usBytesCnt] = ucByte;

	usBytesCnt++;

	xTimeout = xTaskGetTickCount();

	for( ;; )
	{
		if( UART3_RecvByte(&ucByte) )
		{
			xTimeout = xTaskGetTickCount();

			if( usBytesCnt < BUFFER_SIZE )
			{
				pucData[usBytesCnt] = ucByte;

				usBytesCnt++;
			}
		}
		else
		{
			if( (xTaskGetTickCount() - xTimeout) < xModbusCharTimeout )
			{
				taskYIELD();
			}
			else
			{
				break;
			}
		}
	}

	return usBytesCnt;
}


static unsigned short
ModbusSlaveParse( unsigned char *pucData, unsigned short usBytesCnt )
{
	unsigned char  ucException = MB_EXCEPTION_NONE;
	unsigned char  ucResponseSize = 0;
	unsigned char  ucFunction;
	unsigned short usRegAddr;
	unsigned short usRegCnt;
	unsigned short usTemp;

	if( (usBytesCnt >= 4) &&
		(crc_16_ibm(pucData, usBytesCnt) == 0) &&
		((pucData[0] == ucModbusSlaveID) ||
		 (pucData[0] == 0)) )
	{
		ucFunction = pucData[1];

		switch( ucFunction )
		{
		case MBF_READ_HOLDING_REGISTERS:
		case MBF_READ_INPUT_REGISTERS:
			GETU16(usRegAddr, pucData + 2);
			GETU16(usRegCnt, pucData + 4);

			if( (usRegAddr >= sModbusRegMap.usRegStart) &&
				((usRegAddr + usRegCnt) <= (sModbusRegMap.usRegStart +
						                    sModbusRegMap.usRegCnt)) )
			{
				usRegAddr -= sModbusRegMap.usRegStart;

				for( int i = 0; i < usRegCnt; i++ )
				{
					if( sModbusRegMap.RegRead )
					{
						usTemp = sModbusRegMap.RegRead(usRegAddr++);
					}
					else
					{
						usTemp = 0;
					}

					SETU16(pucData + 3 + (i * 2), usTemp);
				}

				*(pucData + 2) = usRegCnt * 2;

				ucResponseSize = 3 + (usRegCnt * 2);
			}
			else
			{
				ucException = MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
			}

			break;

		case MBF_WRITE_SINGLE_REGISTER:
			GETU16(usRegAddr, pucData + 2);

			if( (usRegAddr >= sModbusRegMap.usRegStart) &&
				((usRegAddr + 1) <= (sModbusRegMap.usRegStart +
						             sModbusRegMap.usRegCnt)) )
			{
				GETU16(usTemp, pucData + 4);

				usRegAddr -= sModbusRegMap.usRegStart;

				if( sModbusRegMap.RegWrite )
				{
					if( !sModbusRegMap.RegWrite(usRegAddr, usTemp) )
					{
						ucException = MB_EXCEPTION_ILLEGAL_DATA_VALUE;
					}
				}

				ucResponseSize = 6;
			}
			else
			{
				ucException = MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
			}

			break;

		case MBF_WRITE_MULTIPLE_REGISTERS:
			GETU16(usRegAddr, pucData + 2);
			GETU16(usRegCnt, pucData + 4);

			if( (usRegAddr >= sModbusRegMap.usRegStart) &&
				((usRegAddr + usRegCnt) <= (sModbusRegMap.usRegStart +
						                    sModbusRegMap.usRegCnt)) )
			{
				usRegAddr -= sModbusRegMap.usRegStart;

				for( int i = 0; i < usRegCnt; i++ )
				{
					GETU16(usTemp, pucData + 7 + (i * 2));

					if( sModbusRegMap.RegWrite )
					{
						if( !sModbusRegMap.RegWrite(usRegAddr++, usTemp) )
						{
							ucException = MB_EXCEPTION_ILLEGAL_DATA_VALUE;
						}
					}
				}

				ucResponseSize = 6;
			}
			else
			{
				ucException = MB_EXCEPTION_ILLEGAL_DATA_ADDRESS;
			}

			break;

		default:
			ucException = MB_EXCEPTION_ILLEGAL_FUNCTION;
			break;
		}

		if( ucException != MB_EXCEPTION_NONE )
		{
			pucData[1] |= 0x80;
			pucData[2]  = ucException;

			ucResponseSize = 3;
		}

		if( pucData[0] == 0 )
		{
			ucResponseSize = 0;
		}
	}

	return ucResponseSize;
}


void
ModbusSlaveProc( void )
{
	unsigned short usBytesCnt;
	unsigned short usCRC;

	usBytesCnt = ModbusRecv(pucModbusBuffer);

	if( usBytesCnt )
	{
		usBytesCnt = ModbusSlaveParse(pucModbusBuffer, usBytesCnt);

		if( usBytesCnt > 0 )
		{
			usCRC = crc_16_ibm(pucModbusBuffer, usBytesCnt);

			pucModbusBuffer[usBytesCnt++] = usCRC & 0xFF;
			pucModbusBuffer[usBytesCnt++] = usCRC >> 8;

			ModbusSend(pucModbusBuffer, usBytesCnt);
		}
	}
}


void
ModbusSlaveRegMapSet( tModbusRegMap *psRegMap )
{
	sModbusRegMap.usRegStart = psRegMap->usRegStart;
	sModbusRegMap.usRegCnt   = psRegMap->usRegCnt;
	sModbusRegMap.RegRead    = psRegMap->RegRead;
	sModbusRegMap.RegWrite   = psRegMap->RegWrite;
}


void
ModbusSlaveConfigure( unsigned long  ulBaudRate,
		              unsigned short usParity,
				      unsigned char  ucSlaveID )
{
	UART3_Configure(ulBaudRate, usParity);

	ucModbusSlaveID = ucSlaveID;

	xModbusCharTimeout = ( ulBaudRate >= 19200 ) ?
		2 : 38500 / ulBaudRate;
}


void
ModbusSlaveInit( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Configure UART DX pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_WriteBit(GPIOC, GPIO_Pin_12, Bit_RESET);

	UART3_Init();
}


