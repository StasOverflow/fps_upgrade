/*
 * system.h
 *
 *  Created on: 6 июл. 2019 г.
 *      Author: V.Striukov
 */

#ifndef SYSTEM_SYSTEM_H_
#define SYSTEM_SYSTEM_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif

/* Standard includes. */
#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>

#include "stdint.h"

#include "sys_timer.h"


#define ARRAY_ELEMENT_COUNT(x)         (sizeof (x) / sizeof (x[0]))


#define PIN_SET(ARGS)                  __SET_PIN(ARGS)
#define PIN_CLR(ARGS)                  __CLR_PIN(ARGS)
#define PIN_TOGGLE(ARGS)               __TOGGLE_PIN(ARGS)
#define PIN_STATE(ARGS)                __PIN_STATE(ARGS)
#define PIN_OUTPUT(ARGS)               __PIN_OUTPUT(ARGS)
#define PIN_INPUT(ARGS)                __PIN_INPUT(ARGS)

#define __SET_PIN(PORT_LETTER, N)      GPIO ## PORT_LETTER->BSRRL = GPIO_Pin_ ## N
#define __CLR_PIN(PORT_LETTER, N)      GPIO ## PORT_LETTER->BSRRH = GPIO_Pin_ ## N
#define __TOGGLE_PIN(PORT_LETTER, N)   GPIO ## PORT_LETTER->ODR ^= GPIO_Pin_ ## N
#define __PIN_STATE(PORT_LETTER, N)    ( GPIO ## PORT_LETTER->IDR & GPIO_Pin_ ## N )
#define __PIN_OUTPUT(PORT_LETTER, N)   do { GPIO ## PORT_LETTER->MODER &= ~(GPIO_MODER_MODER0 << (N * 2)); \
                                       GPIO ## PORT_LETTER->MODER |=  (0x01UL << (N * 2)) } while(0)
#define __PIN_INPUT(PORT_LETTER, N)    do { GPIO ## PORT_LETTER->MODER &= ~(GPIO_MODER_MODER0 << (N * 2)); \
                                       GPIO ## PORT_LETTER->MODER |=  (0x00UL << (N * 2)) } while(0)
/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_SYSTEM_H_ */
