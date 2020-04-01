/*
 * app_data.h
 *
 *  Created on: Jan 29, 2020.
 *      Author: V.Striukov
 */

#ifndef SYSTEM_APP_DATA_H_
#define SYSTEM_APP_DATA_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "app_data.h"
#include "lib/types.h"
#include "ustdlib.h"


enum AppData {
    APPDATA_MBUS_SLAVE_ID,
    APPDATA_MBUS_FUNCTION,
    APPDATA_COMM_REG_START_ADDR,
    APPDATA_COMM_REG_TOTAL_QUAN,
    APPDATA_COMM_REG_SCROLL_VAL,
    APPDATA_COMM_TIMEOUT,

    APPDATA_SERIAL_BAUDRATE,
    APPDATA_SERIAL_DATABITS,
    APPDATA_SERIAL_PARITY,
    APPDATA_SERIAL_STOPBITS,

    APPDATA_IFACE_DEFAULT_FORMAT,
    APPDATA_IFACE_SCROLL_MODE,

    APPDATA_TCP_ADDR_NAME,
    APPDATA_TCP_PORT,
    APPDATA_TCP_TIMEOUT,
    APPDATA_TCP_IP_VERSION,

    APPDATA_DEBUG_FIELD_1,
    APPDATA_DEBUG_FIELD_2,
    APPDATA_DEBUG_FIELD_3,
    APPDATA_DEBUG_FIELD_4,
    APPDATA_DEBUG_FIELD_5,
    APPDATA_DEBUG_FIELD_6,
    APPDATA_DEBUG_FIELD_7,
    APPDATA_DEBUG_FIELD_8,
    APPDATA_DEBUG_FIELD_9,
    APPDATA_DEBUG_FIELD_0,

/**/
    APPDATA_ITEM_QUANTITY,
};

typedef struct
{
    uint16_t item_id;
    uint8_t  to_store;
    uint8_t  save_flag;
    uint16_t min_value;
    uint16_t max_value;
    uint16_t inc_value;
    uint16_t default_value;
} app_data_settings_t;

typedef enum {
    Format_Int, Format_Uint, Format_Hex,

    Format_Last = Format_Hex,
} format_t;

typedef enum {
    DataBits_8, DataBits_9,

    DataBits_Last = DataBits_9,
} databit_t;

typedef enum {
    Scroll_Mode_Page, Scroll_Mode_Reg,

    Scroll_Mode_Last = Scroll_Mode_Reg,
} scroll_mode_t;

extern app_data_settings_t  app_data_item_preset[];

uint16_t    sys_param_set_via_protocol( uint8_t param, uint16_t value );

uint16_t    sys_param_extended_set( uint8_t param, uint16_t value );
uint16_t    sys_param_extended_get( uint8_t param );

void        sys_param_setup_init( void );
void        sys_param_setup_callback( void );
void        app_data_item_save( uint16_t param );
void        sys_param_calibration( uint8_t param );
void        sys_param_save_standart( void );
void        sys_param_default_set( void );
uint16_t    app_data_item_set( uint16_t param, uint16_t value );
uint16_t    app_data_item_step_get( uint16_t param );
uint16_t    app_data_item_adjust( uint16_t param, int16_t ajust_value );
uint16_t    app_data_item_get( uint16_t param );
void        sys_param_inc( uint16_t param );

uint16_t    sys_param_compute( uint16_t value, uint16_t coeff_param );

uint8_t     sys_param_validate( uint16_t param, uint16_t& value );

void        app_data_restore_defaults( void );
void        app_data_str_get( uint16_t property, char * string );
void        app_data_str_from_val( uint16_t property, uint16_t value, char *string );
void        app_data_task( void *pvParameters );

/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_APP_DATA_H_ */


