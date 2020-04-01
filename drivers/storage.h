/*
 * store.h
 *
 *  Created on: 6 апр. 2018 г.
 *      Author: V.Dudnik
 */

#ifndef DRIVERS_STORAGE_H_
#define DRIVERS_STORAGE_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif

#define VOLTAGE_RANGE           (uint8_t)VoltageRange_3

uint32_t        storage_read(uint16_t param);
void            storage_write(uint16_t param, uint32_t value);

void            storage_init(void (*save_cb)(void));

void            storage_proc( void );

/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_STORAGE_H_ */
