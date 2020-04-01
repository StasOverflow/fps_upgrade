/*
 * at_eeprom.h
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: V.Striukov
 */

#ifndef DRIVERS_AT_EEPROM_H_
#define DRIVERS_AT_EEPROM_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif

#include "i2c_sw.h"
#define RAW_DATA_SIZE       ( 512 )
#define REG_DATA_SIZE       (RAW_DATA_SIZE >> 1)


#define MANDATORY_SEQUENCE  ( 0xA )          // Defined by Datasheet
#define THREE_BIT_ADDRESS   (   0 )          // Defined by A1, A2
#define COMM_ADDR           ( (MANDATORY_SEQUENCE << 3) | THREE_BIT_ADDRESS )

uint16_t    at_eeprom_rand_read_16( uint16_t reg_addr );
void        at_eeprom_byte_write_16( uint16_t reg_addr, uint16_t value );

void        at_eeprom_reset( void );

//uint8_t     at_eeprom_rand_read( uint16_t reg_addr, uint16_t reg_quan );
void        at_eeprom_proc( void );
void        at_eeprom_init( void );



/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_AT_EEPROM_H_ */
