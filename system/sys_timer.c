//*****************************************************************************
//
// File Name    : 'sys_timer.c'
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
//                v2.3: add dynamic timeproc adding
// Description  : 
//
//
// License      : Impuls ltd.
//
//
//*****************************************************************************

#include <stm32f2xx.h>
#include <stm32f2xx_rcc.h>
#include <stm32f2xx_tim.h>

#include "sys_timer.h"

#ifndef TIMEPROC_COUNT_MAX
#define TIMEPROC_COUNT_MAX                    (  8  )
#endif

#define SYS_TIMER_FREQ_HZ                     ( 50000 )



typedef struct timeproc_tag
{
	void (*pfunc) (void);

	unsigned long  period;
	unsigned long  timer;
} timeproc_t;


static volatile unsigned long       sys_timer_counter;
static          timeproc_t          timeproc[TIMEPROC_COUNT_MAX];
static          uint8_t       timeproc_count;



void TIM5_IRQHandler( void )
{
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);


	sys_timer_counter += 1;


	for( uint8_t i = 0; i < timeproc_count; i++ )
	{
		timeproc[i].timer += 1;

		if( timeproc[i].timer >= timeproc[i].period )
		{
			timeproc[i].timer = 0;

			timeproc[i].pfunc();
		}
	}

}


uint32_t  sys_timer_tick_get( void )
{
    unsigned long ticks;

	ticks = sys_timer_counter;

	return ticks;
}


void sys_timer_timeproc_add( void (*func)(void), uint32_t period )
{
	if( timeproc_count >= TIMEPROC_COUNT_MAX )
		while( 1 ) ;

	timeproc[timeproc_count].pfunc = func;
	timeproc[timeproc_count].period = period;
	timeproc[timeproc_count].timer = 0;

	timeproc_count ++;
}


void sys_timer_init( void )
{
	TIM_TimeBaseInitTypeDef  timer;
	NVIC_InitTypeDef  nvic;


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	//
	// Timer Base configuration
	//
	timer.TIM_Prescaler         = 0;
	timer.TIM_CounterMode       = TIM_CounterMode_Up;
	timer.TIM_Period            = SystemCoreClock / 2 / SYS_TIMER_FREQ_HZ;
	timer.TIM_ClockDivision     = TIM_CKD_DIV2;
	timer.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM5, &timer);

	nvic.NVIC_IRQChannel = TIM5_IRQn;
	nvic.NVIC_IRQChannelPreemptionPriority = 2;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic);

	//
	// TIM5 enable
	//
	TIM_Cmd(TIM5, ENABLE);

	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
}


