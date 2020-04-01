/*
 * app_data.c
 *
 *  Created on: Jan 29, 2020.
 *      Author: V.Striukov
 */

#include <system.h>
#include <drivers/storage.h>

#include "interface.h"
#include "inter_comm.h"
#include "ext_comm.h"

#include "at_eeprom.h"

#include "serial_defs.h"
#include "app_data.h"


const char *format_caption[] =
{
    "int", "uint", "hex",
};
#define FORMAT_MESSAGES_COUNT                   ( sizeof(format_caption) / sizeof(format_caption[0]) )

const char *scroll_mode_caption[] =
{
    "Page", "Reg",
};
#define SCROLL_MODE_MESSAGES_COUNT              ( sizeof(scroll_mode_caption) / sizeof(scroll_mode_caption[0]) )

static  uint8_t     param_changed;

static  uint16_t    app_data_item[APPDATA_ITEM_QUANTITY];
app_data_settings_t app_data_item_preset[] =
{
    {
        .item_id = APPDATA_MBUS_SLAVE_ID,
        .to_store = true,
        .min_value = 1,
        .max_value = 99,
        .inc_value = 1,
        .default_value = 1,
    },
    {
        .item_id = APPDATA_SERIAL_BAUDRATE,
        .to_store = true,
        .min_value = 0,
        .max_value = Baud_Last,
        .inc_value = 1,
        .default_value = Baud_19200,
    },
    {
        .item_id = APPDATA_SERIAL_DATABITS,
        .to_store = true,
        .min_value = 0,
        .max_value = DataBits_Last,
        .inc_value = 1,
        .default_value = DataBits_8,
    },
    {
        .item_id = APPDATA_SERIAL_STOPBITS,
        .to_store = true,
        .min_value = 0,
        .max_value = StopBits_Last,
        .inc_value = 1,
        .default_value = StopBits_1,
    },
    {
        .item_id = APPDATA_SERIAL_PARITY,
        .to_store = true,
        .min_value = 0,
        .max_value = Parity_Last,
        .inc_value = 1,
        .default_value = Parity_Even,
    },
    {
        .item_id = APPDATA_COMM_REG_START_ADDR,
        .to_store = true,
        .min_value = 1,
        .max_value = 49999,
        .inc_value = 1,
        .default_value = 1000,
    },
    {
        .item_id = APPDATA_COMM_REG_TOTAL_QUAN,
        .to_store = true,
        .min_value = 1,
        .max_value = 125,
        .inc_value = 1,
        .default_value = 36,
    },
    {
        .item_id = APPDATA_COMM_REG_SCROLL_VAL,
        .to_store = false,
        .min_value = 0,
        .max_value = 125,
        .inc_value = 1,
        .default_value = 0,
    },
    {
        .item_id = APPDATA_IFACE_DEFAULT_FORMAT,
        .to_store = false,
        .min_value = 0,
        .max_value = Format_Last,
        .inc_value = 1,
        .default_value = Format_Uint,
    },
    {
        .item_id = APPDATA_IFACE_SCROLL_MODE,
        .to_store = false,
        .min_value = 0,
        .max_value = Scroll_Mode_Last,
        .inc_value = 1,
        .default_value = Scroll_Mode_Page,
    },

//        APPDATA_MBUS_FUNCTION,
//        ,
//        APPDATA_MBUS_REQ_QUANTITY,
//        APPDATA_MBUS_TIMEOUT,

//        APPDATA_SERIAL_BAUDRATE,
//        APPDATA_SERIAL_DATABITS,
//        APPDATA_SERIAL_PARITY,
//        APPDATA_SERIAL_STOPBITS,

//        APPDATA_TCP_ADDR_NAME,
//        APPDATA_TCP_PORT,
//        APPDATA_TCP_TIMEOUT,
//        APPDATA_TCP_IP_VERSION,
};

#define APPDATA_SETTINGS_QUAN          ( sizeof(app_data_item_preset) /  \
                                         sizeof(app_data_item_preset[0]) )


static inline uint8_t
app_data_idx_from_id_get( uint8_t item_id )
{
    uint8_t     id = 0;

    assert_param((item_id < APPDATA_ITEM_QUANTITY));

    for( id = 0; id < APPDATA_SETTINGS_QUAN; id++ )
    {
        if( item_id == app_data_item_preset[id].item_id )
        {
            break;
        }
    }

    return id;
}


uint16_t
sys_param_set_via_protocol( uint8_t param, uint16_t value )
{
#if 0
    switch( param )
    {
    case OUTPUT_VOLTAGE_SETUP:
        if( value < VOLTAGE_SET_MIN )
        {
            value = VOLTAGE_SET_MIN;
        }
        else
        if( value > VOLTAGE_SET_MAX )
        {
            value = VOLTAGE_SET_MAX;
        }
        break;
    case OUTPUT_CURRENT_LIMIT:
        if( value < CURRENT_SET_MIN )
        {
            value = CURRENT_SET_MIN;
        }
        else
        if( value > CURRENT_SET_MAX )
        {
            value = CURRENT_SET_MAX;
        }
        break;
    case OUTPUT_VOLTAGE_SETUP_CTRL:
            if( value < VOLTAGE_SET_MIN_CTRL )
            {
                value = VOLTAGE_SET_MIN_CTRL;
            }
            else
            if( value > VOLTAGE_SET_MAX_CTRL )
            {
                value = VOLTAGE_SET_MAX_CTRL;
            }
            break;
	case OUTPUT_CURRENT_LIMIT_CTRL:
        if( value < CURRENT_SET_MIN_CTRL )
        {
            value = CURRENT_SET_MIN_CTRL;
        }
        else
        if( value > CURRENT_SET_MAX_CTRL )
        {
            value = CURRENT_SET_MAX_CTRL;
        }
        break;
    }
#endif

    return app_data_item_set(param, value);
}


#if 0
static uint8_t
sys_param_multiple_set( uint16_t index, uint16_t len, uint8_t* data )
{
	uint8_t exception = UMODBUS_EXCEPTION_NONE;
	uint16_t temp;

	while( len > 0 )
	{
		temp  = *data++ << 8;
		temp |= *data++;

		sys_param_set_via_protocol(index, temp);

		index++;
		len--;
	}

    return exception;
}
#endif


#if 0
static uint8_t
sys_param_extended_multiple_set( uint16_t index, uint16_t len, uint8_t* data )
{
	uint8_t exception = UMODBUS_EXCEPTION_NONE;
	uint16_t temp;

	while( len > 0 )
	{
		temp = *data++ << 8;
		temp |= *data++;

		sys_param_extended_set(index, temp);

		index++;
		len--;
	}

	return exception;
}
#endif


static void
app_data_force_save( void )
{
#if 0
    uint16_t param;
    uint16_t value;

    for( uint8_t i = 0; i < APPDATA_SETTINGS_QUAN; i++ )
    {
        param = app_data_item_preset[i].item_id;
        value = app_data_item[param];

        storage_write(param, value);
    }
#endif
}


static uint8_t
app_data_param_add_to_save_queue( uint16_t item_id )
{
    uint8_t success = 0;
    uint8_t id      = app_data_idx_from_id_get(item_id);

    if( id < APPDATA_SETTINGS_QUAN )
    {
        app_data_item_preset[id].save_flag = 1;
        success = 1;
    }

    return success;
}


static uint16_t
app_data_item_load( uint16_t item_id )
{
    uint8_t  id    = app_data_idx_from_id_get(item_id);
    uint16_t value = at_eeprom_rand_read_16(item_id);


    app_data_item_set(APPDATA_DEBUG_FIELD_1 + id, value);

    if( (value == 0xffff) || (app_data_item_preset[id].to_store == 0) )
    {
        value = app_data_item_preset[id].default_value;
    }
    else
    {
        sys_param_validate(item_id, value);
    }

    return value;
}


void
app_data_item_save( uint16_t item_id )
{
    uint16_t value = app_data_item[item_id];

    at_eeprom_byte_write_16(item_id, value);
}


void
sys_param_setup_init( void )
{

}


void sys_param_default_set( void )
{

}


void
sys_param_setup_callback( void )
{
	sys_param_save_standart();
}


#if 0
uint16_t
sys_param_extended_get( uint8_t param )
{
	int16_t value;

	value = sys_param_ext[param];

	return value;
}


uint16_t
sys_param_extended_set( uint8_t param, uint16_t value )
{
	uint16_t accepted_value;

	accepted_value = app_data_item[param];

	switch( param )
	{
//	case xxx:
//		if( value >= xxx && value <= xxx )
//			accepted_value = value;
//		break;

	default:
		accepted_value = value;
	}

	sys_param_ext[param] = accepted_value;

	return accepted_value;
}
#endif


uint8_t
sys_param_validate( uint16_t item_id, uint16_t& value )
{
    uint16_t        accepted_value = app_data_item[item_id];

    uint8_t         changed = false;
    uint16_t        index = app_data_idx_from_id_get(item_id);

    if( index < APPDATA_SETTINGS_QUAN )
    {
        if( value >= app_data_item_preset[index].min_value )
        {
            if( value <= app_data_item_preset[index].max_value )
            {
                value = value - value % app_data_item_preset[index].inc_value;
            }
            else
            {
                value = app_data_item_preset[index].max_value;
            }
        }
        else
        {
            value = app_data_item_preset[index].min_value;
        }
        if( accepted_value != value )
            changed = true;
    }

    return changed;
}


uint16_t
app_data_item_set( uint16_t item_id, uint16_t new_value )
{
    if( item_id < APPDATA_ITEM_QUANTITY )
    {
        if( app_data_item[item_id] != new_value )
        {
            app_data_item[item_id] = new_value;
            if( app_data_param_add_to_save_queue(item_id) )
            {
                param_changed = 1;
            }
        }
    }

    return new_value;
}


uint16_t
app_data_item_step_get( uint16_t item_id )
{
    uint8_t index;
    uint16_t step = 0;

    for( index = 0; index < APPDATA_SETTINGS_QUAN; index++ )
    {
        if( app_data_item_preset[index].item_id == item_id )
            break;
    }

    if( index < APPDATA_SETTINGS_QUAN )
        step = app_data_item_preset[index].inc_value;

    return step;
}


uint16_t
app_data_item_adjust( uint16_t param, int16_t adjust_value )
{
    int32_t     value;

    value = app_data_item[param] + adjust_value;
    if( value < 0 )
        value = 0;
    sys_param_validate(param, (uint16_t &) value);

    return app_data_item_set(param, value);
}


uint16_t
app_data_item_get( uint16_t param )
{
    uint16_t value;

    value = app_data_item[param];

    return value;
}


static void
app_data_settings_restore( void )
{
    for( uint8_t i = 0; i < APPDATA_SETTINGS_QUAN; i++ )
    {
        app_data_item[i] = app_data_item_load(i);
    }
}


extern TaskHandle_t inter_comm_handler;
extern TaskHandle_t ext_comm_handler;
extern TaskHandle_t interface_handler;

static inline void
app_data_notify_loading_done( void )
{
    xTaskNotifyGive( ext_comm_handler );
    xTaskNotifyGive( inter_comm_handler );
    xTaskNotifyGive( interface_handler );
}


static void
app_data_task_proc( void )
{
    uint16_t            param_c;
    static uint16_t     index;
    static enum {
        Param_Restore,
        Param_Restore_Done_Notify,
        Param_Changing_Monitor,
        Param_Changing_Cooldown,
        Param_Save
    } stage;
    static TickType_t   cooldown_timer;


    switch( stage )
    {
    case Param_Restore:
        app_data_settings_restore();
        stage = Param_Restore_Done_Notify;
        break;

    case Param_Restore_Done_Notify:
        app_data_notify_loading_done();
        stage = Param_Changing_Monitor;
        break;

    case Param_Changing_Monitor:
//        app_data_item_set(APPDATA_DEBUG_FIELD_1, param_changed);
        if( param_changed )
        {
            param_changed = 0;
            cooldown_timer = xTaskGetTickCount();

            stage = Param_Changing_Cooldown;
        }
        break;

    case Param_Changing_Cooldown:
        if( param_changed )
        {
            stage = Param_Changing_Monitor;
        }
        else
        if( (xTaskGetTickCount() - cooldown_timer) >= 2500 )
        {
            stage = Param_Save;
        }
        break;

    case Param_Save:
        if( app_data_item_preset[index].save_flag == 1 )
        {
            app_data_item_preset[index].save_flag = 0;
            app_data_item_save(app_data_item_preset[index].item_id);
        }

        if( ++index >= APPDATA_SETTINGS_QUAN )
        {
            cooldown_timer = xTaskGetTickCount();

            index = 0;
            stage = Param_Changing_Monitor;
        }
        break;
    }
//    app_data_item_set(APPDATA_DEBUG_FIELD_2, stage);
}


void
app_data_str_from_val( uint16_t property, uint16_t value, char *string )
{
    switch( property )
    {
    case APPDATA_MBUS_SLAVE_ID:
        usprintf(string, "%d", value);
        break;
    case APPDATA_SERIAL_BAUDRATE:
        usprintf(string, "%s", (value < BAUDRATE_MESSAGES_COUNT) ? baud_caption[value] : "OvfErr");
        break;
    case APPDATA_SERIAL_STOPBITS:
        usprintf(string, "%s", (value < STOPBIT_MESSAGES_COUNT) ? stopbit_caption[value] : "OvfErr");
        break;
    case APPDATA_SERIAL_DATABITS:
        usprintf(string, "%s", (value < DATABIT_MESSAGES_COUNT) ? databit_caption[value] : "OvfErr");
        break;
    case APPDATA_SERIAL_PARITY:
        usprintf(string, "%s", (value < PARITY_MESSAGES_COUNT) ? parity_caption[value] : "OvfErr");
        break;
    case APPDATA_IFACE_DEFAULT_FORMAT:
        usprintf(string, "%s", (value < FORMAT_MESSAGES_COUNT) ? format_caption[value] : "OvfErr");
        break;
    case APPDATA_IFACE_SCROLL_MODE:
        usprintf(string, "%s", (value < SCROLL_MODE_MESSAGES_COUNT) ? scroll_mode_caption[value] : "OvfErr");
        break;
    default:
        usprintf(string, "%d", value);
        break;
    }
}


void
app_data_str_get( uint16_t property, char * string )
{
    app_data_str_from_val(property, app_data_item[property], string);
}


void
app_data_restore_defaults( void )
{
    for( uint8_t i = 0; i < APPDATA_SETTINGS_QUAN; i++ )
    {
        app_data_item_set(app_data_item_preset[i].item_id, app_data_item_preset[i].default_value);
    }
}


void
app_data_task( void *pvParameters )
{
    at_eeprom_init();

    for(;;)
    {
        app_data_task_proc();

        vTaskDelay(10);
    }
}
