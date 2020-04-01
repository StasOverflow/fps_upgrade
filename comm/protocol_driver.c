/*
 * protocol_driver.c
 *
 *  Created on: 22 нояб. 2018 г.
 *      Author: V.Striukov
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "config.h"
#include "string.h"

#include "protocol_driver.h"
#include "drivers/uart.h"

#include "sys_timer.h"


/******************************************************************************
 *
 * Package format is as follows:
 *
 *
 *
 *
 *
 *****************************************************************************/

/*
 * Package Defines section
 */

#define MESSAGE_LEN_MAX                 255

/*
 * ID
 */
#define ID_POSITION                     0
#define ID_LEN                          1

enum {
    Input_Id,
    Config_Id,
};

/*
 * Payload length
 */
#define PYLD_SIZE_POSITION              ID_POSITION + ID_LEN
#define PYLD_SIZE_LEN                   1

#define PYLD_SIZE_BOUNDARY_MIN          2
#define PYLD_SIZE_BOUNDARY_MAX          (   MESSAGE_LEN_MAX             \
                                          - PYLD_DATA_OFFSET            \
                                          - CRC_SIZE )

#define boundary_fits(x)                ( ( x >= PYLD_SIZE_BOUNDARY_MIN ) && \
                                          ( x <= PYLD_SIZE_BOUNDARY_MAX ) )


/*
 * Payload data
 */
#define PYLD_DATA_OFFSET                ( PYLD_SIZE_POSITION + PYLD_SIZE_LEN )

#define pyld_calculate_from_len(len)    ( len - PYLD_DATA_OFFSET - CRC_SIZE )
#define len_calculate_from_pyld(pyld)   ( pyld + PYLD_DATA_OFFSET + CRC_SIZE )

/*
 * CRC
 */
#define CRC_SIZE                        0
//#define CRC_SIZE                        2


typedef enum {
    Init,
    TimeoutClear,
    ByteHandle,
} message_handler_stage;


enum {
    Recieve,
    Skip,
};

typedef enum {
    Column_Address,
    Length,
    Data,
    Crc,
} message_sequence;


static unsigned char            length;
static t_package_status         package_status;

static unsigned char            message[MESSAGE_LEN_MAX];
static unsigned char            message_buffer[MESSAGE_LEN_MAX];



unsigned char *
protocol_driver_message_data_get( void )
{
    return message;
}


void
protocol_driver_message_data_flush( void )
{
    for( int i = 0; i < MESSAGE_LEN_MAX; i++ )
    {
        message_buffer[i] = 0;
    }
}


void
protocol_driver_message_len_set_from_pld( unsigned char pyld_len )
{
    length = len_calculate_from_pyld(pyld_len);
}


unsigned char
protocol_driver_payload_len_get( void )
{
    short len = pyld_calculate_from_len(length);

    if( len < 0 ) {
        len = 0;
    }

    return len;
}


unsigned char
protocol_driver_message_len_get( void )
{
    return length;
}


void
protocol_driver_data_status_update( t_package_status status )
{
    package_status = status;
}


t_package_status
protocol_driver_data_status_get( t_package_status status )
{
    return package_status;
}


void
protocol_driver_proc( void )
{
    static TickType_t               xPackageTimeout;
    static message_handler_stage    message_stage;
    static unsigned char            receiver_mode;
    static unsigned char            pyLdLen;
    static unsigned char            pos;
    static unsigned char            cell;
    static unsigned char            pack_recvd;

    unsigned char                   byte_there_is = 0;
    unsigned char                   byte_recvd;


    switch( message_stage ) {

    case Init:
        protocol_driver_message_data_flush();
        receiver_mode           = Recieve;
        message_stage           = TimeoutClear;
        pack_recvd = 0;
        pyLdLen = 0;
        pos = 0;
        cell = 0;
        UART1_Flush();
        break;

    case TimeoutClear:
        message_stage = ByteHandle;
        xPackageTimeout = xTaskGetTickCount();
        break;

    case ByteHandle:
        while( UART1_RecvByte(&byte_recvd) )
        {
            if( receiver_mode == Recieve )
            {
                byte_there_is = 1;
                xPackageTimeout = xTaskGetTickCount();
                if( pos == ID_POSITION )
                {
                    if( byte_recvd == Input_Id )
                    {
                        // vse ok
                    }
                    else
                    if( byte_recvd == Config_Id )
                    {
                        //otdat' parseru instructiyu
                    }
                    else
                    {
                        receiver_mode = Skip;
                    }
                }
                else
                if( pos == PYLD_SIZE_POSITION )
                {
                    if( boundary_fits(byte_recvd) )
                    {
                        pyLdLen = byte_recvd;
                        protocol_driver_message_len_set_from_pld(pyLdLen);
                    }
                    else
                    {
                        receiver_mode = Skip;
                    }
                }
                else
                if( pos >= PYLD_DATA_OFFSET )
                {
                    message_buffer[cell++] = byte_recvd;
                }
                pos++;
                if( ( ( pyLdLen ) && ( pos >= length ) )
                    || pos == 255 )
                {
                    pack_recvd = 1;
                }
            }
        }
        if( xTaskGetTickCount() - xPackageTimeout >= 10 )
        {
            if( pack_recvd )
            {
                memcpy(&message, &message_buffer , 255);
            }
            message_stage = Init;
        }
//        GPIO_WriteBit(GPIOE, GPIO_Pin_2, byte_there_is);
        break;

    default:
        break;
    }

}


void
protocol_driver_init( void )
{
    UART1_Init();

    UART1_Configure(SERIAL_BAUDRATE_19200, UART_PARITY_NONE);
}



