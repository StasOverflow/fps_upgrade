/*
 * at_eeprom.c
 *
 *  External EEPROM driver
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: V.Striukov
 */

#include "at_eeprom.h"


static uint8_t          module_init;
//static enum {
//    Stage_Init,
//    Stage_Idle,
//    Stage_Read,
//    Stage_Write,
//    Stage_Ack_Poll,
//    Stage_Done,
//} proc_stage_t;

//static union {
//    uint8_t     raw[2];
//    uint16_t    val;
//} data;
//
//static uint16_t     eeprom_reg_addr;
//static uint16_t     eeprom_reg_quan;
//static uint8_t      eeprom_act_next = Stage_Idle;


//
//uint16_t
//at_eeprom_data_get( uint16_t addr )
//{
//    uint16_t    reg_value;
//
//    if( addr < REG_DATA_SIZE )
//    {
//        reg_value = reg[addr].val;
//    }
//    return ;
//}



uint16_t
at_eeprom_rand_read_16( uint16_t reg_addr )
{
    uint8_t     data[2];
    uint16_t    dev_addr = COMM_ADDR | (reg_addr >> 8);

    while( i2c_sw_reg_read(dev_addr, reg_addr * 2, data, 2) == 0 )
    {
        taskYIELD();
    }

    if( i2c_error_get() != I2C_ERR_NONE )
    {
        data[0] = 0xff;
        data[1] = 0xff;
    }

    return (data[0] << 8) | data[1];
}


void
at_eeprom_byte_write_16( uint16_t reg_addr, uint16_t value )
{
    uint8_t     data[2];
    uint16_t    dev_addr = COMM_ADDR | (reg_addr >> 8);

    data[0] = value >> 8;
    data[1] = value & 0xff;

    while( i2c_sw_reg_write(dev_addr, reg_addr * 2, data, 2) == 0 )
    {
        taskYIELD();
    }
}


void
at_eeprom_proc( void )
{
}


void
at_eeprom_init( void )
{
    if( !module_init )
    {
        module_init = 1;
        i2c_sw_init();
    }
    else
    {
        return;
    }
}
