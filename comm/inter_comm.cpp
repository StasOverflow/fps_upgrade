// Scheduler includes.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "system.h"

#include "lib/types.h"
#include "drivers/modbus_master.h"
#include "modbus_register_table.h"

#include "app_data.h"

#if( INCLUDE_NET == 1 )
#include "net_property.h"
#include "network.h"
#endif

#include "inter_comm.h"

/***
 *  new meta
 */
#define MODBUS_RTU_MAX_REG              125

static uint8_t      inter_comm_activity;



static uint16_t     inter_comm_start_addr;
static uint8_t      inter_comm_data_quan;

static icomm_data   inter_comm_data[MODBUS_RTU_MAX_REG];

//static format_t     inter_comm_data_fmt[MODBUS_RTU_MAX_REG];
//static uint16_t     inter_comm_data_val[MODBUS_RTU_MAX_REG];
static uint16_t     inter_comm_frame_quan;      //  ceil (reg_quantity/frame_size)  (4)

//static uint8_t      inter_comm_data_read_lockbit_mask[ (MODBUS_RTU_MAX_REG / 8) +
//                                                      ((MODBUS_RTU_MAX_REG % 8) ? 1 : 0)];  //would be 16
//static uint8_t      inter_comm_frame_ready_bit_mask[(4 / 8) +
//                                                   ((4 % 8) ? 1 : 0)];  // inter_comm_data_frame_quantity / 8  (1)

/***
 *  old meta
 */
#define INTER_COMM_DATA_COUNT           MODBUS_RTU_MAX_REG

#define INTER_COMM_RECEIVE_FRAME        32
#define INTER_COMM_TRANSMITT_FRAME      8
#define INTER_COMM_ATTEMPTS             5

#define INTER_COMM_RECEIVE_FRAMES_COUNT    ( (INTER_COMM_DATA_COUNT / INTER_COMM_RECEIVE_FRAME) + \
                                            ((INTER_COMM_DATA_COUNT % INTER_COMM_RECEIVE_FRAME) ? 1 : 0) )

static unsigned char    ucInterCommDataReadLockBitMask[(INTER_COMM_DATA_COUNT / 8) +
                                                       ((INTER_COMM_DATA_COUNT % 8) ? 1 : 0)];

static unsigned char    ucInterCommFrameReadyBitMask[(INTER_COMM_RECEIVE_FRAMES_COUNT / 8) +
                                                    ((INTER_COMM_RECEIVE_FRAMES_COUNT % 8) ? 1 : 0)];


static uint32_t         inter_comm_conn_errs = Unknown;
static uint32_t         inter_comm_conn_tries = Unknown;
static bool             inter_comm_conn_present_val;

static uint8_t          inter_comm_conn;

static uint8_t          inter_comm_read_in_process;
static uint8_t          inter_comm_write_in_process;

static uint16_t         inter_comm_slave_addr;

static enum {
    Comm_Notify_Await,
    Comm_Init,
    Comm_Configure,
    Comm_Timeout_Start,
    Comm_Timeout,
    Comm_Transceive,
    Comm_Errs_Handle,
    Comm_Off,
} comm_stage = Comm_Notify_Await;


void
inter_comm_format_reset_to_defaults( void )
{
    uint16_t    def_fmt = app_data_item_get(APPDATA_IFACE_DEFAULT_FORMAT);

    for( uint8_t i = 0; i < MODBUS_RTU_MAX_REG; i++ )
    {
        inter_comm_data[i].fmt = (format_t) def_fmt;
        inter_comm_data[i].fmt_set = false;
    }
}


void
inter_comm_format_update( void )
{
    uint16_t    def_fmt = app_data_item_get(APPDATA_IFACE_DEFAULT_FORMAT);

    for( uint8_t i = 0; i < MODBUS_RTU_MAX_REG; i++ )
    {
        if( inter_comm_data[i].fmt_set == false )
        {
            inter_comm_data[i].fmt = (format_t) def_fmt;
        }
    }
}


uint8_t
inter_comm_activity_get( void )
{
    return inter_comm_activity;
}


void
inter_comm_start_addr_set( uint16_t new_addr )
{
    inter_comm_start_addr = new_addr;
}


uint16_t
inter_comm_start_addr_get( void )
{
    return inter_comm_start_addr;
}


void
inter_comm_reg_quantity_set( uint16_t reg_quantity )
{
    inter_comm_data_quan = reg_quantity;
}


uint8_t
inter_comm_conn_state_get( void )
{
    return inter_comm_conn;
}


void
inter_comm_conn_state_set( uint8_t state )
{
    inter_comm_conn = state;
}


static void
InterCommFrameReadySet( unsigned short usFrame )
{
	unsigned short usByteNum;
	unsigned char  ucBitMask;

	usByteNum = usFrame / 8;
	ucBitMask = 1 << (usFrame % 8);

	ucInterCommFrameReadyBitMask[usByteNum] |= ucBitMask;
}


static void
InterCommDataReadLock( unsigned short usRegIndex )
{
	unsigned short usByteNum;
	unsigned char  ucBitMask;

	usByteNum = usRegIndex / 8;
	ucBitMask = 1 << (usRegIndex % 8);

	ucInterCommDataReadLockBitMask[usByteNum] |= ucBitMask;
}


static void
InterCommDataReadUnLock( unsigned short usRegIndex )
{
	unsigned short usByteNum;
	unsigned char  ucBitMask;

	usByteNum = usRegIndex / 8;
	ucBitMask = 1 << (usRegIndex % 8);

	ucInterCommDataReadLockBitMask[usByteNum] &= ~ucBitMask;
}


static long
InterCommDataReadLockStatusGet( unsigned short usRegIndex )
{
	unsigned short usByteNum;
	unsigned char  ucBitMask;

	usByteNum = usRegIndex / 8;
	ucBitMask = 1 << (usRegIndex % 8);

	return (ucInterCommDataReadLockBitMask[usByteNum] & ucBitMask) ? 1 : 0;
}


static void
InterCommDataTransmitt( uint32_t *pulCommErrors )
{
	unsigned long  ulCommAttempts;
	unsigned short usRegData[INTER_COMM_TRANSMITT_FRAME];
	unsigned short usRegCount;
	unsigned short usRegIndex = 0;
	         short sCommStatus;


     while( usRegIndex < INTER_COMM_DATA_COUNT )
	{
		usRegCount = 0;

		//
		// Find regs with read lock status (to be written) in succession
		//
		for( int i = 0; ((usRegIndex + i) < INTER_COMM_DATA_COUNT); i++ )
		{
			if( InterCommDataReadLockStatusGet(usRegIndex + i) )
			{
				taskENTER_CRITICAL();
				usRegData[usRegCount] = inter_comm_data[usRegIndex + usRegCount].val;
				InterCommDataReadUnLock(usRegIndex + usRegCount);
				taskEXIT_CRITICAL();

				if( ++usRegCount >= INTER_COMM_TRANSMITT_FRAME )
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		if( usRegCount )
		{
			ulCommAttempts = INTER_COMM_ATTEMPTS;

			while( ulCommAttempts )
			{
				sCommStatus = ModbusMasterWriteMultipleRegs(inter_comm_slave_addr, inter_comm_start_addr + usRegIndex,
                                                            usRegCount, usRegData);
				if( sCommStatus == MB_OK )
				{
					break;
				}
				else
				{
					ulCommAttempts--;
					(*pulCommErrors)++;
				}
			}

			usRegIndex += usRegCount;
		}
		else
		{
			usRegIndex++;
		}
	}
}


static void
inter_comm_data_receive( uint32_t *comm_errs )
{
    static uint16_t     curr_frame;
           unsigned short usRegData[INTER_COMM_RECEIVE_FRAME];
           unsigned short usRegCount;
           unsigned short usRegIndex;
                    short sCommStatus = MB_ERROR_BAD_FD;

    usRegIndex = curr_frame * INTER_COMM_RECEIVE_FRAME;
    usRegCount   = ( curr_frame == (inter_comm_frame_quan - 1) ) ?
                   ( inter_comm_data_quan - usRegIndex ) : INTER_COMM_RECEIVE_FRAME;


    sCommStatus = ModbusMasterReadInputRegs(inter_comm_slave_addr, (inter_comm_start_addr + usRegIndex),
                                            usRegCount, usRegData);

    if( sCommStatus == MB_OK )
    {
        InterCommFrameReadySet(curr_frame);

        for( int i = 0; i < usRegCount; i++ )
        {
            if( !InterCommDataReadLockStatusGet(usRegIndex + i) )
            {
//                *(inter_comm_data_val + usRegIndex + i) = *(usRegData + i);
//                (inter_comm_data + usRegIndex + i)->val = *(usRegData + i);
                inter_comm_data[usRegIndex + i].val = usRegData[i];
            }
        }
    }
    else
    {
        // error
        *comm_errs += 1;
    }

    if( ++curr_frame >= inter_comm_frame_quan )
    {
        curr_frame = 0;
        inter_comm_read_in_process = false;
    }
    else
    {
        inter_comm_read_in_process = true;
    }
}


#if( INCLUDE_NET == 1 )
static void
vInterCommNetSettingsLoad( long *pulCommErrors )
{
	static tBoolean       isRunning = false;
	static tBoolean       isLoaded = false;
	static portTickType   xTimer;


	if( !isRunning || *pulCommErrors )
	{
		isRunning = true;

		xTimer  = xTaskGetTickCount();
	}

	if(!isLoaded)
	{
		if((xTaskGetTickCount() - xTimer) >= 1500)
		{
			net_settings_load();

			isLoaded = true;

			network_init();
		}
	}
}
#endif


long
InterCommDataReady( void )
{
	unsigned short usByteNum;
	unsigned char  ucBitMask;
	         long  lReturn = 1;

	for( int i = 0; i < INTER_COMM_RECEIVE_FRAMES_COUNT; i++ )
	{
		usByteNum = i / 8;
		ucBitMask = 1 << (i % 8);

		if( !(ucInterCommFrameReadyBitMask[usByteNum] & ucBitMask) )
		{
			lReturn = 0;

			break;
		}
	}

	return lReturn;
}


unsigned short
InterCommDataGet( unsigned short usRegIndex )
{
	unsigned short usValue = 0;

	if( usRegIndex < INTER_COMM_DATA_COUNT )
	{
		taskENTER_CRITICAL();
		usValue = inter_comm_data[usRegIndex].val;
		taskEXIT_CRITICAL();
	}

	return usValue;
}


void
InterCommDataSet( unsigned short usRegIndex, unsigned short usValue )
{
    if( usRegIndex < INTER_COMM_DATA_COUNT )
    {
        InterCommDataReadLock(usRegIndex);
        inter_comm_data[usRegIndex].val = usValue;
    }
}


uint16_t
inter_comm_data_format_get( uint16_t icomm_idx )
{
    uint16_t value = 0;

    if( icomm_idx < INTER_COMM_DATA_COUNT )
    {
        taskENTER_CRITICAL();
        value = inter_comm_data[icomm_idx].fmt;
        taskEXIT_CRITICAL();
    }

    return value;
}


uint8_t
inter_comm_data_has_format( uint16_t icomm_idx )
{
    uint8_t value = 0;

    if( icomm_idx < INTER_COMM_DATA_COUNT )
    {
        taskENTER_CRITICAL();
        value = inter_comm_data[icomm_idx].fmt_set;
        taskEXIT_CRITICAL();
    }
    return value;
}


void
inter_comm_data_format_set( uint16_t icomm_idx, format_t fmt )
{
    if( icomm_idx < INTER_COMM_DATA_COUNT )
    {
        taskENTER_CRITICAL();
        inter_comm_data[icomm_idx].fmt_set = true;
        inter_comm_data[icomm_idx].fmt = fmt;
        taskEXIT_CRITICAL();
    }
}


uint32_t
inter_comm_conn_instances_get( void )
{
    return inter_comm_conn_tries;
}


uint32_t
inter_comm_conn_errs_get( void )
{
    return inter_comm_conn_errs;
}


uint8_t
inter_comm_conn_present( void )
{
    return inter_comm_conn_present_val;
}


uint8_t
inter_comm_mbus_settings_update( void )
{
    static uint16_t         baud_prev;
    static uint16_t         parity_prev;
    static uint16_t         stopbits_prev;
    uint16_t                baud = app_data_item_get(APPDATA_SERIAL_BAUDRATE);
    uint16_t                parity = app_data_item_get(APPDATA_SERIAL_PARITY);
    uint16_t                stopbits = app_data_item_get(APPDATA_SERIAL_STOPBITS);
    uint8_t                 update_flag = false;


    if( baud_prev != baud )
    {
        baud_prev = baud;
        update_flag = true;
    }

    if( parity_prev != parity )
    {
        parity_prev = parity;
        update_flag = true;
    }

    if( stopbits_prev != stopbits )
    {
        stopbits_prev = stopbits;
        update_flag = true;
    }

    if( update_flag )
    {
        ModbusMasterConfigure((baudrate_t)baud, (parity_t)parity, (stopbits_t)stopbits);
    }

    return update_flag;
}


static void
inter_comm_config_update( void )
{
    if( !inter_comm_read_in_process && !inter_comm_write_in_process )
    {
        inter_comm_start_addr = app_data_item_get(APPDATA_COMM_REG_START_ADDR);
        inter_comm_data_quan = app_data_item_get(APPDATA_COMM_REG_TOTAL_QUAN);
        inter_comm_frame_quan = inter_comm_data_quan / INTER_COMM_RECEIVE_FRAME +
                               (inter_comm_data_quan % INTER_COMM_RECEIVE_FRAME ? 1 : 0);
        inter_comm_slave_addr = app_data_item_get(APPDATA_MBUS_SLAVE_ID);

        if( comm_stage >= Comm_Configure )
        {
            inter_comm_mbus_settings_update();
        }
    }
}


static void
inter_comm_proc( void )
{
    static TickType_t   comm_timer;
    static TickType_t   activity_timer;
    static uint32_t     errs;
    static BitAction    led_state;


    inter_comm_config_update();
    if( inter_comm_conn_present_val )
    {
        if( inter_comm_activity )
        {
            if( xTaskGetTickCount() - activity_timer >= 80 )
            {
                inter_comm_activity = 0;
            }
        }
    }

    switch( comm_stage )
    {
    case Comm_Notify_Await:
//        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
        comm_stage = Comm_Init;
        break;

    case Comm_Init:
        ModbusMasterInit();
        inter_comm_format_reset_to_defaults();
        comm_stage = Comm_Configure;
        break;

    case Comm_Configure:
        // mbus_configuration happens in inter_comm_config_update() func
        comm_stage = Comm_Timeout_Start;
        break;

    case Comm_Timeout_Start:
        comm_timer = xTaskGetTickCount();
        comm_stage = Comm_Timeout;
        break;

    case Comm_Timeout:
        if( xTaskGetTickCount() - comm_timer >= 100 )
        {
            if( inter_comm_conn == false )
            {
                comm_stage = Comm_Off;
            }
            else
            {
                comm_stage = Comm_Transceive;
            }
        }
        break;

    case Comm_Transceive:
        errs = 0;

        inter_comm_data_receive(&errs);
        vTaskDelay(5);
        InterCommDataTransmitt(&errs);

        inter_comm_conn_tries++;

        comm_stage = Comm_Errs_Handle;
        break;

    case Comm_Errs_Handle:
        if( errs )
        {
            inter_comm_conn_errs  += errs;
            inter_comm_conn_present_val = false;
            inter_comm_activity = false;
        }
        else
        {
            activity_timer = xTaskGetTickCount();
            inter_comm_conn_present_val  = true;
            inter_comm_activity = true;
        }

        led_state = inter_comm_conn_present_val ? Bit_RESET : Bit_SET;
        comm_stage = Comm_Timeout_Start;
        break;

    case Comm_Off:
        led_state = Bit_RESET;
        if( inter_comm_conn == true )
        {
            comm_stage = Comm_Timeout_Start;
        }
        break;

    default:
        break;
    }

//    GPIO_WriteBit(GPIOE, GPIO_Pin_2, led_state); // Red LED
}


void
inter_comm_task( void *pvParameters )
{
    for( ;; )
    {
#if( INCLUDE_NET == 1 )
        vInterCommNetSettingsLoad(&errs);
#endif
        inter_comm_proc();
    }
}
