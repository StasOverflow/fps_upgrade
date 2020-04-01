/*
 * i2c_sw.h
 *
 *  Created on: 24 февр. 2020 г.
 *      Author: V.Striukov
 */

#ifndef DRIVERS_I2C_SW_H_
#define DRIVERS_I2C_SW_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif

#include "system.h"
#include "string.h"

#define I2C_BUF_SIZE        ( 128 )
#define SEQ_SEPARATORS      ( I2C_BUF_SIZE >> 5 ) // 2 ^ 5 = 32
#define I2C_RW_READ         (  1  )
#define I2C_RW_WRITE        (  0  )
#define ADRR_7_BIT_SIZE     (  1  )
#define ADRR_10_BIT_SIZE    (  2  )

#define SDA_PIN             C, 11
#define SCK_PIN             C, 10

#define SDA_GET             PIN_STATE(SDA_PIN)

#define SDA_SET             PIN_SET(SDA_PIN)
#define SDA_CLR             PIN_CLR(SDA_PIN)

#define SCK_SET             PIN_SET(SCK_PIN)
#define SCK_CLR             PIN_CLR(SCK_PIN)


typedef enum {
    I2C_ERR_NONE,
    I2C_ERR_BUSY,
    I2C_ERR_OVERFLOW,
    I2C_ERR_ACK,
} i2c_err_t;

void      i2c_error_clr( void );
i2c_err_t i2c_error_get( void );

uint8_t   i2c_sw_error_get( void );
void      i2c_sw_error_clr( void );

uint8_t   i2c_sw_reg_write(uint16_t addr, uint8_t reg, uint8_t* data,
                            uint8_t size);
uint8_t   i2c_sw_reg_read (uint16_t addr, uint8_t reg, uint8_t* data,
                            uint8_t size);

void      i2c_sw_bus_stop( void );
void      i2c_sw_bus_resume( void );

void      i2c_sw_proc( void );
void      i2c_sw_init( void );

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define I2C_ENTER_CRITICAL()        taskENTER_CRITICAL()
#define I2C_EXIT_CRITICAL()         taskEXIT_CRITICAL()

/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_I2C_SW_H_ */
