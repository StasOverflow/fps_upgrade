#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "modbus_master.h"
#include "drivers/uart.h"
#include "lib/crc.h"

#define MBF_READ_HOLDING_REGISTERS          0x03
#define MBF_READ_INPUT_REGISTERS            0x04
#define MBF_WRITE_SINGLE_REGISTER           0x06
#define MBF_WRITE_MULTIPLE_REGISTERS        0x10
#define MBF_READ_WRITE_MULTIPLE_REGISTERS   0x17
#define MBF_REPORT_SLAVE_ID                 0x11

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

#define BUFFER_SIZE                 ( 512 )

static unsigned char    pucModbusBuffer[BUFFER_SIZE];
static portTickType     xModbusFrameTimeout;
static portTickType     xModbusCharTimeout;


static void
ModbusSend( unsigned char *pucData, unsigned short usBytesCnt )
{
    for( int i = 0; i < usBytesCnt; i++ )
    {
        if( !UART1_SendByte(pucData[i]) )
        {
            taskYIELD();
        }
    }

    while( !UART1_IsTransmittionComplete() )
    {
        taskYIELD();
    }
}


static uint16_t
ModbusRecv( uint8_t *pucData )
{
    uint16_t            byte_cnt = 0;
    uint8_t             byte;
    portTickType        xTimeout;

    UART1_Flush();

    xTimeout = xTaskGetTickCount();

    for( ;; )
    {
        if( UART1_RecvByte(&byte) )
        {
            pucData[byte_cnt] = byte;
            byte_cnt++;

            break;
        }
        else
        {
            if( (xTaskGetTickCount() - xTimeout) < xModbusFrameTimeout )
            {
                taskYIELD();
            }
            else
            {
                break;
            }
        }
    }

    if( byte_cnt > 0 )
    {
        xTimeout = xTaskGetTickCount();

        for( ;; )
        {
            if( UART1_RecvByte(&byte) )
            {
                xTimeout = xTaskGetTickCount();

                if( byte_cnt < BUFFER_SIZE )
                {
                    pucData[byte_cnt] = byte;

                    byte_cnt++;
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
    }

    return byte_cnt;
}


long
ModbusMasterWriteMultipleRegs( unsigned char  ucSlaveID,
                               unsigned short usReg,
                               unsigned short usRegNum,
                               unsigned short *pusInput )
{
    unsigned short  usCRC;
    unsigned short  usBytesCnt = 0;
    long            lReturn = MB_OK;

    pucModbusBuffer[usBytesCnt] = ucSlaveID;
    usBytesCnt += 1;

    pucModbusBuffer[usBytesCnt] = MBF_WRITE_MULTIPLE_REGISTERS;
    usBytesCnt += 1;

    SETU16(pucModbusBuffer + usBytesCnt, usReg);
    usBytesCnt += 2;

    SETU16(pucModbusBuffer + usBytesCnt, usRegNum);
    usBytesCnt += 2;

    pucModbusBuffer[usBytesCnt] = usRegNum * 2;
    usBytesCnt += 1;

    for( int i = 0; i < usRegNum; i++ )
    {
        SETU16(pucModbusBuffer + usBytesCnt, pusInput[i]);

        usBytesCnt += 2;
    }

    usCRC = crc_16_ibm(pucModbusBuffer, usBytesCnt);

    pucModbusBuffer[usBytesCnt++] = usCRC & 0x00FF;
    pucModbusBuffer[usBytesCnt++] = usCRC >> 8;

    ModbusSend(pucModbusBuffer, usBytesCnt);

    if( ucSlaveID != 0 )
    {
        usBytesCnt = ModbusRecv(pucModbusBuffer);

        if( usBytesCnt != 0 )
        {
            if( (usBytesCnt >= 3) &&
                (usBytesCnt <= 255) )
            {
                if( crc_16_ibm(pucModbusBuffer, usBytesCnt) == 0 )
                {
                    if( pucModbusBuffer[1] != MBF_WRITE_MULTIPLE_REGISTERS )
                    {
                        lReturn = MB_ERROR_FUNCTION;
                    }
                }
                else
                {
                    lReturn = MB_ERROR_FORMAT;
                }
            }
            else
            {
                lReturn = MB_ERROR_LENGTH;
            }
        }
        else
        {
            lReturn = MB_ERROR_TIMEOUT;
        }
    }

    return lReturn;
}


long
ModbusMasterReadInputRegs( unsigned char  ucSlaveID,
                           unsigned short usReg,
                           unsigned short usRegNum,
                           unsigned short *pusOutput )
{
    unsigned short usCRC;
    unsigned short usBytesCnt = 0;
    long           lReturn = MB_OK;

    pucModbusBuffer[usBytesCnt] = ucSlaveID;
    usBytesCnt += 1;

    pucModbusBuffer[usBytesCnt] = MBF_READ_HOLDING_REGISTERS;
    usBytesCnt += 1;

    SETU16(pucModbusBuffer + usBytesCnt, usReg);
    usBytesCnt += 2;

    SETU16(pucModbusBuffer + usBytesCnt, usRegNum);
    usBytesCnt += 2;

    usCRC = crc_16_ibm(pucModbusBuffer, usBytesCnt);

    pucModbusBuffer[usBytesCnt++] = usCRC & 0x00FF;
    pucModbusBuffer[usBytesCnt++] = usCRC >> 8;

    ModbusSend(pucModbusBuffer, usBytesCnt);

    if( ucSlaveID != 0 )
    {
        usBytesCnt = ModbusRecv(pucModbusBuffer);

        if( usBytesCnt != 0 )
        {
            if( (usBytesCnt >= 3) &&
                (usBytesCnt <= 255) )
            {
                if( crc_16_ibm(pucModbusBuffer, usBytesCnt) == 0 )
                {
                    if( pucModbusBuffer[1] == MBF_READ_HOLDING_REGISTERS )
                    {
                        for( int i = 0; i < pucModbusBuffer[2] / 2; i++ )
                        {
                            GETU16(pusOutput[i], pucModbusBuffer + 3 + i * 2);
                        }
                    }
                    else
                    {
                        lReturn = MB_ERROR_FUNCTION;
                    }
                }
                else
                {
                    lReturn = MB_ERROR_FORMAT;
                }
            }
            else
            {
                lReturn = MB_ERROR_LENGTH;
            }
        }
        else
        {
            lReturn = MB_ERROR_TIMEOUT;
        }
    }

    return lReturn;
}


void
ModbusMasterConfigure( baudrate_t br_lkp_id, parity_t par_lkp_id, stopbits_t sbit_lkp_id )
{
    UART1_Configure(br_lkp_id, par_lkp_id, sbit_lkp_id);

    xModbusCharTimeout = ( br_lkp_id >= Baud_19200 ) ?
        2 : 38500 / baud_lkp[br_lkp_id];
    xModbusFrameTimeout = 100;
}

void
ModbusMasterInit( void )
{
    UART1_Init();
}


