//*****************************************************************************
//
// File Name    : 'sys_timer.h'
// Title        :
// Author       : I.Otrokha
// Created      : 07.12.2011
// Revised      : 10.07.2014
// Version      : v2.3
// Target MCU   : ATmega8, ATmega(48,88,168), ATmega128, A90PWM3B
// Editor Tabs  : Not tested int ATmega48, ATmega88
//                v1.9: added 32bit timestamp counter ( not used yet )
//                v2.0: fixed non-atomic reference to 16bit counter
//                v2.1: add new target mcu, code refactored
//                v2.2: refactored
//                v2.3: add dynamic timeproc adding, del 32bit timer
// Description  : 
//
//
// License      : Impuls ltd.
//
//
//*****************************************************************************


#ifndef SYS_TIMER_H_
#define SYS_TIMER_H_


/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif

// only for sys_ticks timer counter
#define _MS( x_100usValue )                   ( (x_100usValue)*10U )

// only for seconds timer counter
#define _SEC( x_1secValue )                   ( x_1secValue )
#define _MN(  x_1secValue )                   ( (x_1secValue)*60U )
#define _HR(  x_1secValue )                   ( (x_1secValue)*3600U )


#define sys_timer_tick_diff_get( timer )      ( sys_timer_tick_get() - timer )
#define sys_timer_tick_update( timer )        do {                           \
                                              timer = sys_timer_tick_get();  \
                                              } while(0)



void       sys_timer_timeproc_add( void (*func)(void), uint32_t period );

uint32_t   sys_timer_tick_get( void );

void       sys_timer_init( void );

/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif


#endif /* SYS_TIMER_H_ */
