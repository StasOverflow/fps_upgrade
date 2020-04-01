
#ifndef INTER_COMM_H_
#define INTER_COMM_H_

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

#define CMD_NONE                                    0
#define CMD_SHUTDOWN_IRPS                           1
#define CMD_SHUTDOWN_CONV                           2
#define CMD_ENABLE_CONV                             3
#define CMD_REBOOT                                  4

#define CMD_SAVE_STD                                5
#define CMD_SAVE_EXP                                6
#define CMD_SAVE_MBCONFIG                           7
#define CMD_RESET_STD                               8
#define CMD_FACTORY_RESET                           9


#define CMD_BATT_MODE_ABM                           10
#define CMD_BATT_MODE_CBM                           11
#define CMD_BATT_CHARGE_BOOST                       12
#define CMD_OUTPUT_AC_PROTECTION_RESET              13

// batt command
#define CMD_BATT_CAPACITY_COMPUTATION_ENABLE        16 // defines on arg
#define CMD_BATT_CAPACITY_RESET                     17
#define CMD_FORCE_CALIBRATE_IM_RDIV                 18

// service commands
#define CMD_RESET_COUNTERS                          32
#define CMD_RESET_COMM_ERR_COUNTERS                 33

#define CMD_SYS_LOG_FLUSH                           40
#define CMD_SYS_LOG_DEQUEUE                         41
#define CMD_SYS_LOG_DEQUEUE_BY_INDEX                42

#define CMD_EEPROM_READ                             64
#define CMD_EEPROM_SET_ADDR                         65
#define CMD_EEPROM_WRITE                            66

#define CMD_UNIT_SEND_PARAM                         128


// access commands
#define CMD_MAIN_PASSWORD_ENT                       192
#define CMD_USER_PASSWORD_ENT                       193

#define CMD_MAIN_PASSWORD_RESET                     194
#define CMD_MAIN_PASSWORD_SET                       195
#define CMD_USER_PASSWORD_SET                       196

#define CMD_GET_ACCESS_TYPE                         197
#define CMD_LOGOUT                                  198

#define CMD_PLAY_MELODY                             200


#define CONTROL_MAIN_PWD_RESET_VALUE                17687
#define CONTROL_MAIN_PWD_DEFAULT_VALUE              60312
#define CONTROL_USER_PWD_DEFAULT_VALUE              12345

#define CONTROL_MAIN_PWD_EEPROM_ADDR                128
#define CONTROL_USER_PWD_EEPROM_ADDR                130


enum {
    Unknown,
};


typedef struct {
    uint16_t        val;
    format_t        fmt;
    uint8_t         fmt_set;   /* if 0, means use default parameter */
} icomm_data;

long            InterCommDataReady( void );

uint8_t         inter_comm_activity_get( void );
uint32_t        inter_comm_conn_instances_get( void );
uint32_t        inter_comm_conn_errs_get( void );

uint8_t         inter_comm_conn_present( void );

uint8_t         inter_comm_conn_state_get( void );
void            inter_comm_conn_state_set( uint8_t state );

void            inter_comm_format_reset_to_defaults( void );
uint8_t         inter_comm_data_has_format( uint16_t icomm_idx );
void            inter_comm_format_update( void );
uint16_t        inter_comm_data_format_get( uint16_t icomm_idx );
void            inter_comm_data_format_set( uint16_t icomm_idx, format_t fmt );

unsigned short  InterCommDataGet( unsigned short usRegIndex );
void            InterCommDataSet( unsigned short usRegIndex, unsigned short usValue );

void            inter_comm_start_addr_set( uint16_t new_addr );
uint16_t        inter_comm_start_addr_get( void );

void            inter_comm_task( void *pvParameters );

/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif

#endif /* INTER_COMM_H_ */
