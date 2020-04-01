//*****************************************************************************
//
// touch.c - Touch screen driver for the DK-LM3S9B96 board.
//
// Copyright (c) 2008-2010 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 6459 of the DK-LM3S9B96 Firmware Package.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup touch_api
//! @{
//
//*****************************************************************************
#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>

#include "grlib/grlib.h"
#include "drivers/touch.h"
#include "drivers/lcd_driver.h"

//*****************************************************************************
//
// The touch screen max coordinates in landscape orientation.
//
//*****************************************************************************
#define TOUCH_X_MAX                    320
#define TOUCH_Y_MAX	                   240

//*****************************************************************************
//
// The GPIO pins to which the touch screen is connected.
//
//*****************************************************************************
#define TS_XP_PERIPH            RCC_AHB1Periph_GPIOA
#define TS_XN_PERIPH            RCC_AHB1Periph_GPIOG
#define TS_YP_PERIPH            RCC_AHB1Periph_GPIOA
#define TS_YN_PERIPH            RCC_AHB1Periph_GPIOG

#define TS_XP_PORT              GPIOA
#define TS_XN_PORT              GPIOG
#define TS_YP_PORT              GPIOA
#define TS_YN_PORT              GPIOG

#define TS_XP_PIN               GPIO_Pin_5
#define TS_XN_PIN               GPIO_Pin_6
#define TS_YP_PIN               GPIO_Pin_6
#define TS_YN_PIN               GPIO_Pin_7

#define TS_XP_PIN_SOURCE        GPIO_PinSource5
#define TS_XN_PIN_SOURCE        GPIO_PinSource6
#define TS_YP_PIN_SOURCE        GPIO_PinSource6
#define TS_YN_PIN_SOURCE        GPIO_PinSource7


//*****************************************************************************
//
// The ADC channels connected to each of the touch screen contacts.
//
//*****************************************************************************
#define TS_ADC_PERIPH           RCC_APB2Periph_ADC1
#define TS_ADC                  ADC1
#define TS_ADC_XP_CH            ADC_Channel_5
#define TS_ADC_YP_CH            ADC_Channel_6

//*****************************************************************************

#define TS_TIMER_PERIPH         RCC_APB1Periph_TIM3
#define TS_TIMER                TIM3

//*****************************************************************************

#define RESCALE_FACTOR          0x00100000

//*****************************************************************************
//
// Touchscreen calibration parameters.  We store several sets since different
// LCD configurations require different calibration.  Screen orientation is a
// build time selection but the calibration set used is determined at runtime
// based on the hardware configuration.
//
//*****************************************************************************
static long g_lTouchParameters[NUM_TOUCH_PARAMS] =
{
	0x00018484,     // M0
	0xFFFFFBC2,     // M1
	0xFDD14C49,     // M2
	0x00000495,     // M3
	0xFFFEC58C,     // M4
	0x112EDE87,     // M5
	0x00100000,     // M6
};

//*****************************************************************************
//
// A pointer to the current touchscreen calibration parameter set.
//
//*****************************************************************************
const long *g_plParamSet;

//*****************************************************************************
//
// The minimum raw reading that should be considered valid press.
//
//*****************************************************************************
short g_sTouchADCMin = TOUCH_ADC_MIN;

//*****************************************************************************
//
// The current state of the touch screen driver's state machine.  This is used
// to cycle the touch screen interface through the powering sequence required
// to read the two axes of the surface.
//
//*****************************************************************************
static unsigned long g_ulTSState;

enum
{
	TS_STATE_INIT,
	TS_STATE_READ_X,
	TS_STATE_READ_Y,
	TS_STATE_SKIP_X,
	TS_STATE_SKIP_Y
};

//*****************************************************************************
//
// The most recent raw ADC reading for the X position on the screen.  This
// value is not affected by the selected screen orientation.
//
//*****************************************************************************
volatile short g_sTouchADCX;

//*****************************************************************************
//
// The most recent raw ADC reading for the Y position on the screen.  This
// value is not affected by the selected screen orientation.
//
//*****************************************************************************
volatile short g_sTouchADCY;

//*****************************************************************************
//
// ADC Raw reading for X coordinate of last touched point
//
//*****************************************************************************
volatile short g_sLastTouchADCX;

//*****************************************************************************
//
// ADC Raw reading for Y coordinate of last touched point
//
//*****************************************************************************
volatile short g_sLastTouchADCY;

//*****************************************************************************
//
// X coordinate of last touched point
//
//*****************************************************************************
volatile long g_lLastTouchX;

//*****************************************************************************
//
// Y coordinate of last touched point
//
//*****************************************************************************
volatile long g_lLastTouchY;

//*****************************************************************************
//
// A pointer to the function to receive messages from the touch screen driver
// when events occur on the touch screen (debounced presses, movement while
// pressed, and debounced releases).
//
//*****************************************************************************
static long (*g_pfnTSHandler)(unsigned long ulMessage, long lX, long lY);

//*****************************************************************************
//
// The current state of the touch screen debouncer.  When zero, the pen is up.
// When three, the pen is down.  When one or two, the pen is transitioning from
// one state to the other.
//
//*****************************************************************************
static unsigned char g_cState = 0;

//*****************************************************************************
//
// The queue of debounced pen positions.  This is used to slightly delay the
// returned pen positions, so that the pen positions that occur while the pen
// is being raised are not send to the application.
//
//*****************************************************************************
 short g_psSamples[8];

//*****************************************************************************
//
// The count of pen positions in g_psSamples.  When negative, the buffer is
// being pre-filled as a result of a detected pen down event.
//
//*****************************************************************************
static signed char g_cIndex = 0;


//*****************************************************************************
//
// Coordinates of points used for touch screen calibration operation
//
//*****************************************************************************
const
static tTouchPoint sCalibrationPoints[CALIBRATION_POINTS_NUM] =
{
	{(TOUCH_X_MAX * 50) / 100, (TOUCH_Y_MAX * 10) / 100},
	{(TOUCH_X_MAX * 90) / 100, (TOUCH_Y_MAX * 50) / 100},
	{(TOUCH_X_MAX * 10) / 100, (TOUCH_Y_MAX * 90) / 100},
};


//*****************************************************************************
//
// Remap X coordinate.
//
// \param lX is the X coordinate.
// \param lY is the Y coordinate.
//
// This function remaps X coordinate for various display orientation.
//
// \return Returns remaped X coordinate.
//
//*****************************************************************************
long TouchScreenRemapX(long lX, long lY)
{
	long lMappedX = 0;

	switch(DisplayOrientationGet())
	{
	case DISPLAY_ORIENTATION_LANDSCAPE:
		lMappedX = lX;
		break;

	case DISPLAY_ORIENTATION_LANDSCAPE_FLIP:
		lMappedX = 319 - lX;
		break;

	case DISPLAY_ORIENTATION_PORTRAIT:
		lMappedX = 239 - lY;
		break;

	case DISPLAY_ORIENTATION_PORTRAIT_FLIP:
		lMappedX = lY;
		break;
	}

	return lMappedX;
}


//*****************************************************************************
//
// Remap Y coordinate.
//
// \param lX is the X coordinate.
// \param lY is the Y coordinate.
//
// This function remaps Y coordinate for various display orientation.
//
// \return Returns remaped Y coordinate.
//
//*****************************************************************************
long TouchScreenRemapY(long lX, long lY)
{
	long lMappedY = 0;

	switch(DisplayOrientationGet())
	{
	case DISPLAY_ORIENTATION_LANDSCAPE:
		lMappedY = lY;
		break;

	case DISPLAY_ORIENTATION_LANDSCAPE_FLIP:
		lMappedY = 239 - lY;
		break;

	case DISPLAY_ORIENTATION_PORTRAIT:
		lMappedY = lX;
		break;

	case DISPLAY_ORIENTATION_PORTRAIT_FLIP:
		lMappedY = 319 - lX;
		break;
	}

	return lMappedY;
}

//*****************************************************************************
//
//! Debounces presses of the touch screen.
//!
//! This function is called when a new X/Y sample pair has been captured in
//! order to perform debouncing of the touch screen.
//!
//! \return None.
//
//*****************************************************************************
void
TouchScreenDebouncer(void)
{
	long lX, lY, lRemapedX, lRemapedY, lTemp;

	//
	// Convert the ADC readings into pixel values on the screen.
	//
	lX = g_sTouchADCX;
	lY = g_sTouchADCY;
	lTemp = (((lX * g_plParamSet[0]) + (lY * g_plParamSet[1]) + g_plParamSet[2]) /
			g_plParamSet[6]);
	lY = (((lX * g_plParamSet[3]) + (lY * g_plParamSet[4]) + g_plParamSet[5]) /
			g_plParamSet[6]);
	lX = lTemp;

	//
	// See if the touch screen is being touched.
	//
	if((g_sTouchADCX < g_sTouchADCMin) || (g_sTouchADCY < g_sTouchADCMin))
	{
		//
		// See if the pen is not up right now.
		//
		if(g_cState != 0x00)
		{
			//
			// Decrement the state count.
			//
			g_cState--;

			//
			// See if the pen has been detected as up three times in a row.
			//
			if(g_cState == 0x80)
			{
				//
				// Indicate that the pen is up.
				//
				g_cState = 0x00;

				//
				// See if there is a touch screen event handler.
				//
				if(g_pfnTSHandler)
				{
					//
					// Remap coordinates.
					//
					lRemapedX = TouchScreenRemapX(g_psSamples[g_cIndex],
							               g_psSamples[g_cIndex + 1]);
					lRemapedY = TouchScreenRemapY(g_psSamples[g_cIndex],
							               g_psSamples[g_cIndex + 1]);
					//
					// Send the pen up message to the touch screen event
					// handler.
					//
					g_pfnTSHandler(WIDGET_MSG_PTR_UP, lRemapedX, lRemapedY);
				}
			}
		}
	}
	else
	{
		//
		// See if the pen is not down right now.
		//
		if(g_cState != 0x83)
		{
			//
			// Increment the state count.
			//
			g_cState++;

			//
			// See if the pen has been detected as down three times in a row.
			//
			if(g_cState == 0x03)
			{
				//
				// Indicate that the pen is up.
				//
				g_cState = 0x83;

				//
				// Set the index to -8, so that the next 3 samples are stored
				// into the sample buffer before sending anything back to the
				// touch screen event handler.
				//
				g_cIndex = -8;

				//
				// Store this sample into the sample buffer.
				//
				g_psSamples[0] = lX;
				g_psSamples[1] = lY;

				//
				// Store current ADC readings. It's a touch point.
				//
				g_sLastTouchADCX = g_sTouchADCX;
				g_sLastTouchADCY = g_sTouchADCY;
			}
		}
		else
		{
			//
			// See if the sample buffer pre-fill has completed.
			//
			if(g_cIndex == -2)
			{
				//
				// See if there is a touch screen event handler.
				//
				if(g_pfnTSHandler)
				{
					//
					// Remap coordinates.
					//
					lRemapedX = TouchScreenRemapX(g_psSamples[0], g_psSamples[1]);
					lRemapedY = TouchScreenRemapY(g_psSamples[0], g_psSamples[1]);

					//
					// Store last touched coordinates.
					//
					g_lLastTouchX = lRemapedX;
					g_lLastTouchY = lRemapedY;

					//
					// Send the pen down message to the touch screen event
					// handler.
					//
					g_pfnTSHandler(WIDGET_MSG_PTR_DOWN, lRemapedX, lRemapedY);
				}

				//
				// Store this sample into the sample buffer.
				//
				g_psSamples[0] = lX;
				g_psSamples[1] = lY;

				//
				// Set the index to the next sample to send.
				//
				g_cIndex = 2;
			}

			//
			// Otherwise, see if the sample buffer pre-fill is in progress.
			//
			else if(g_cIndex < 0)
			{
				//
				// Store this sample into the sample buffer.
				//
				g_psSamples[g_cIndex + 10] = lX;
				g_psSamples[g_cIndex + 11] = lY;

				//
				// Increment the index.
				//
				g_cIndex += 2;
			}

			//
			// Otherwise, the sample buffer is full.
			//
			else
			{
				//
				// See if there is a touch screen event handler.
				//
				if(g_pfnTSHandler)
				{
					//
					// Remap coordinates.
					//
					lRemapedX = TouchScreenRemapX(g_psSamples[g_cIndex],
							               g_psSamples[g_cIndex + 1]);
					lRemapedY = TouchScreenRemapY(g_psSamples[g_cIndex],
							               g_psSamples[g_cIndex + 1]);

					//
					// Send the pen move message to the touch screen event
					// handler. Coordinates are relative from last touch point.
					//
					g_pfnTSHandler(WIDGET_MSG_PTR_MOVE, lRemapedX - g_lLastTouchX,
							                            lRemapedY - g_lLastTouchY);
				}

				//
				// Store this sample into the sample buffer.
				//
				g_psSamples[g_cIndex] = lX;
				g_psSamples[g_cIndex + 1] = lY;

				//
				// Increment the index.
				//
				g_cIndex = (g_cIndex + 2) & 7;
			}
		}
	}
}

//*****************************************************************************
//
//! Handles the ADC interrupt for the touch screen.
//!
//! This function is called when the ADC sequence that samples the touch screen
//! has completed its acquisition.  The touch screen state machine is advanced
//! and the acquired ADC sample is processed appropriately.
//!
//! It is the responsibility of the application using the touch screen driver
//! to ensure that this function is installed in the interrupt vector table for
//! the ADC3 interrupt.
//!
//! \return None.
//
//*****************************************************************************
void
ADC_IRQHandler(void)
{
	//
	// Increment Grlib general timer
	//
	GrTimerInc();

	switch( g_ulTSState )
	{
		//
		// The new sample is an X axis sample that should be discarded.
		//
		case TS_STATE_SKIP_X:
		{
			//
			// Read and throw away the ADC sample.
			//
			TS_ADC->DR;

			//
			// Set the analog mode select for the YP pin.
			//
			TS_YP_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_YP_PIN_SOURCE * 2));
			TS_YP_PORT->MODER |=  (GPIO_Mode_AN << (TS_YP_PIN_SOURCE * 2));

			//
			// Configure the Y axis touch layer pins as inputs.
			//
			TS_YN_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_YN_PIN_SOURCE * 2));
			TS_YN_PORT->MODER |=  (GPIO_Mode_IN << (TS_YN_PIN_SOURCE * 2));

			//
			// The next sample will be a valid X axis sample.
			//
			g_ulTSState = TS_STATE_READ_X;

			//
			// This state has been handled.
			//
			break;
		}

		//
		// The new sample is an X axis sample that should be processed.
		//
		case TS_STATE_READ_X:
		{
			//
			// Read the raw ADC sample.
			//
			g_sTouchADCX = TS_ADC->DR;

			//
			// Configure X axis touch layers as outputs.
			//
			TS_XP_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_XP_PIN_SOURCE * 2));
			TS_XP_PORT->MODER |=   GPIO_Mode_OUT << (TS_XP_PIN_SOURCE * 2);

			TS_XN_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_XN_PIN_SOURCE * 2));
			TS_XN_PORT->MODER |=   GPIO_Mode_OUT << (TS_XN_PIN_SOURCE * 2);

			//
			// Configure Y axis touch layers as outputs.
			//
			TS_YP_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_YP_PIN_SOURCE * 2));
			TS_YP_PORT->MODER |=   GPIO_Mode_OUT << (TS_YP_PIN_SOURCE * 2);

			TS_YN_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_YN_PIN_SOURCE * 2));
			TS_YN_PORT->MODER |=   GPIO_Mode_OUT << (TS_YN_PIN_SOURCE * 2);

			//
			// Drive the positive side of the Y axis touch layer with VDD and
			// the negative side with GND.  Also, drive both sides of the X
			// axis layer with GND to discharge any residual voltage (so that
			// a no-touch condition can be properly detected).
			//
			TS_YP_PORT->BSRRL = TS_YP_PIN;
			TS_YN_PORT->BSRRH = TS_YN_PIN;

			TS_XP_PORT->BSRRH = TS_XP_PIN;
			TS_XN_PORT->BSRRH = TS_XN_PIN;

			//
			// Configure the sample sequence to capture the X axis value.
			//
			ADC_RegularChannelConfig(TS_ADC, TS_ADC_XP_CH, 1, ADC_SampleTime_480Cycles);

			//
			// The next sample will be an invalid Y axis sample.
			//
			g_ulTSState = TS_STATE_SKIP_Y;

			//
			// This state has been handled.
			//
			break;
		}

		//
		// The new sample is a Y axis sample that should be discarded.
		//
		case TS_STATE_SKIP_Y:
		{
			//
			// Read and throw away the ADC sample.
			//
			TS_ADC->DR;

			//
			// Set the analog mode select for the XP pin.
			//
			TS_XP_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_XP_PIN_SOURCE * 2));
			TS_XP_PORT->MODER |=  (GPIO_Mode_AN << (TS_XP_PIN_SOURCE * 2));

			//
			// Configure the X axis touch layer pins as inputs.
			//
			TS_XN_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_XN_PIN_SOURCE * 2));
			TS_XN_PORT->MODER |=  (GPIO_Mode_IN << (TS_XN_PIN_SOURCE * 2));

			//
			// The next sample will be a valid Y axis sample.
			//
			g_ulTSState = TS_STATE_READ_Y;

			//
			// This state has been handled.
			//
			break;
		}

		//
		// The new sample is a Y axis sample that should be processed.
		//
		case TS_STATE_READ_Y:
		{
			//
			// Read the raw ADC sample.
			//
			g_sTouchADCY = TS_ADC->DR;

			//
			// The next configuration is the same as the initial configuration.
			// Therefore, fall through into the initialization state to avoid
			// duplicating the code.
			//
		}

		//
		// The state machine is in its initial state
		//
		case TS_STATE_INIT:
		{
			//
			// Configure X axis touch layers as outputs.
			//
			TS_XP_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_XP_PIN_SOURCE * 2));
			TS_XP_PORT->MODER |=   GPIO_Mode_OUT << (TS_XP_PIN_SOURCE * 2);

			TS_XN_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_XN_PIN_SOURCE * 2));
			TS_XN_PORT->MODER |=   GPIO_Mode_OUT << (TS_XN_PIN_SOURCE * 2);

			//
			// Configure Y axis touch layers as outputs.
			//
			TS_YP_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_YP_PIN_SOURCE * 2));
			TS_YP_PORT->MODER |=   GPIO_Mode_OUT << (TS_YP_PIN_SOURCE * 2);

			TS_YN_PORT->MODER &= ~(GPIO_MODER_MODER0 << (TS_YN_PIN_SOURCE * 2));
			TS_YN_PORT->MODER |=   GPIO_Mode_OUT << (TS_YN_PIN_SOURCE * 2);

			//
			// Drive one side of the X axis touch layer with VDD and the other
			// with GND.  Also, drive both sides of the Y axis layer with GND
			// to discharge any residual voltage (so that a no-touch condition
			// can be properly detected).
			//
			TS_XP_PORT->BSRRL = TS_XP_PIN;
			TS_XN_PORT->BSRRH = TS_XN_PIN;

			TS_YP_PORT->BSRRH = TS_YP_PIN;
			TS_YN_PORT->BSRRH = TS_YN_PIN;

			//
			// Configure the sample sequence to capture the Y axis value.
			//
			ADC_RegularChannelConfig(TS_ADC, TS_ADC_YP_CH, 1, ADC_SampleTime_480Cycles);

			//
			// If this is the valid Y sample state, then there is a new X/Y
			// sample pair.  In that case, run the touch screen debouncer.
			//
			if(g_ulTSState == TS_STATE_READ_Y)
			{
				TouchScreenDebouncer();
			}

			//
			// The next sample will be an invalid X axis sample.
			//
			g_ulTSState = TS_STATE_SKIP_X;

			//
			// This state has been handled.
			//
			break;
		}
	}
}

//*****************************************************************************
//
//! Initializes the touch screen driver.
//!
//! \return None.
//
//*****************************************************************************
void
TouchScreenInit(void)
{
	ADC_InitTypeDef				ADC_InitStruct;
	GPIO_InitTypeDef			GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef		TIM_TimeBaseInitStruct;

	//
	// Set the initial state of the touch screen driver's state machine.
	//
	g_ulTSState = TS_STATE_INIT;

	//
	// Determine which calibration parameter set we will be using.
	//
	g_plParamSet = g_lTouchParameters;

	//
	// There is no touch screen handler initially.
	//
	g_pfnTSHandler = 0;

	//
	// Enable the peripherals used by the touch screen interface.
	//
	RCC_APB2PeriphClockCmd(TS_ADC_PERIPH, ENABLE);
	RCC_AHB1PeriphClockCmd(TS_XP_PERIPH, ENABLE);
	RCC_AHB1PeriphClockCmd(TS_XN_PERIPH, ENABLE);
	RCC_AHB1PeriphClockCmd(TS_YP_PERIPH, ENABLE);
	RCC_AHB1PeriphClockCmd(TS_YN_PERIPH, ENABLE);

	RCC_APB1PeriphClockCmd(TS_TIMER_PERIPH, ENABLE);

	//
	// Configure the ADC to read the touch screen reading.
	//

	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfConversion = 1;
	ADC_Init(TS_ADC, &ADC_InitStruct);

	ADC_Cmd(TS_ADC, ENABLE);

	//
	// Enable the ADC sample sequence interrupt.
	//
	ADC_ITConfig(TS_ADC, ADC_IT_EOC, ENABLE);
	NVIC_EnableIRQ(ADC_IRQn);

	//
	// Configure the GPIOs used to drive the touch screen layers.
	//
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = TS_XP_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(TS_XP_PORT, &GPIO_InitStruct);

	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = TS_XN_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(TS_XN_PORT, &GPIO_InitStruct);

	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = TS_YP_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(TS_YP_PORT, &GPIO_InitStruct);

	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = TS_YN_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(TS_YN_PORT, &GPIO_InitStruct);

	GPIO_WriteBit(TS_XP_PORT, TS_XP_PIN, Bit_RESET);
	GPIO_WriteBit(TS_XN_PORT, TS_XN_PIN, Bit_RESET);
	GPIO_WriteBit(TS_YP_PORT, TS_YP_PIN, Bit_RESET);
	GPIO_WriteBit(TS_YN_PORT, TS_YN_PIN, Bit_RESET);

	//
	// Configure the timer to trigger the sampling of the touch screen
	// every millisecond.
	//
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
	TIM_TimeBaseInitStruct.TIM_Prescaler = 1;
	TIM_TimeBaseInitStruct.TIM_Period = ((SystemCoreClock / 4) / 1000);
	TIM_TimeBaseInit(TS_TIMER, &TIM_TimeBaseInitStruct);

	TIM_SelectOutputTrigger(TS_TIMER, TIM_TRGOSource_Update);

	//
	// Enable the timer.  At this point, the touch screen state machine
	// will sample and run once per millisecond.
	//
	TIM_Cmd(TS_TIMER, ENABLE);
}

//*****************************************************************************
//
//! Sets the callback function for touch screen events.
//!
//! \param pfnCallback is a pointer to the function to be called when touch
//! screen events occur.
//!
//! This function sets the address of the function to be called when touch
//! screen events occur.  The events that are recognized are the screen being
//! touched (``pen down''), the touch position moving while the screen is
//! touched (``pen move''), and the screen no longer being touched (``pen
//! up'').
//!
//! \return None.
//
//*****************************************************************************
void
TouchScreenCallbackSet(long (*pfnCallback)(unsigned long ulMessage, long lX,
                                           long lY))
{
	//
	// Save the pointer to the callback function.
	//
	g_pfnTSHandler = pfnCallback;
}


//*****************************************************************************
//
//! Gets calibration points coordinates.
//!
//! \param usPoint is a num of point which coordinates to be read
//! \param psTouchPoint is a pointer to tTouchPoint struct where
//! coordinates to be written.
//!
//! \return None.
//
//*****************************************************************************
void TouchScreenCalibrationPointCoordinatesGet( tTouchPoint *psTouchPoint, unsigned short usPoint )
{
	psTouchPoint->lTouchX = sCalibrationPoints[usPoint].lTouchX;
	psTouchPoint->lTouchY = sCalibrationPoints[usPoint].lTouchY;
}

//*****************************************************************************
//
//! Gets ADC readings for last touched point.
//!
//! \param psADCX is a pointer to the X coordinate to be read
//! \param psADCY is a pointer to the Y coordinate to be read
//!
//! \return None.
//
//*****************************************************************************
void TouchScreenADCReadingsGet(short *psADCX, short *psADCY)
{
	*psADCX = g_sLastTouchADCX;
	*psADCY = g_sLastTouchADCY;
}


//*****************************************************************************
//
//! Sets touch screen calibration parameters.
//!
//! \param lParams is a pointer to the array of calibration parameters
//!
//! \return None.
//
//*****************************************************************************
void TouchScreenSetCalibrationParams(long *lParams)
{
	for( int i = 0; i < NUM_TOUCH_PARAMS; i++ )
	{
		g_lTouchParameters[i] = *(lParams + i);
	}
}


//*****************************************************************************
//
//! Gets touch screen calibration parameters.
//!
//! \param lParams is a pointer to the array of calibration parameters
//!
//! \return None.
//
//*****************************************************************************
void TouchScreenGetCalibrationParams(long *lParams)
{
	for( int i = 0; i < NUM_TOUCH_PARAMS; i++ )
	{
		*(lParams + i) = g_lTouchParameters[i];
	}
}


//*****************************************************************************
//
//! Runs touch screen calibration procedure.
//!
//! \param sCalibrationPoints is a pointer to the array of calibration
//! points coordinates.
//!
//! \param psPointADCReadings is a pointer to the array of calibration
//! points adc readings.
//!
//! \param lTouchCalibrationParams is a pointer to the array where calibration
//! params is stored.
//!
//! \return None.
//
//*****************************************************************************
void TouchScreenCalibration(tTouchPoint *psPointADCReadings)
{
	double	llTemp1;
	double	llTemp2;
	double	llCalA;
	double	llCalB;
	double	llCalC;
	double	llCalD;
	double	llCalE;
	double	llCalF;

	//
	// Calibration coeff A
	//
	llTemp1 = ((double) sCalibrationPoints[CalibrationPoint_1].lTouchX   *
			  ((double) psPointADCReadings[CalibrationPoint_2].lTouchY   -
			   (double) psPointADCReadings[CalibrationPoint_3].lTouchY)) +
			  ((double) sCalibrationPoints[CalibrationPoint_2].lTouchX   *
		      ((double) psPointADCReadings[CalibrationPoint_3].lTouchY   -
		       (double) psPointADCReadings[CalibrationPoint_1].lTouchY)) +
			  ((double) sCalibrationPoints[CalibrationPoint_3].lTouchX   *
			  ((double) psPointADCReadings[CalibrationPoint_1].lTouchY   -
			   (double) psPointADCReadings[CalibrationPoint_2].lTouchY));

	llTemp2 = ((double) psPointADCReadings[CalibrationPoint_1].lTouchX   *
			  ((double) psPointADCReadings[CalibrationPoint_2].lTouchY   -
			   (double) psPointADCReadings[CalibrationPoint_3].lTouchY)) +
			  ((double) psPointADCReadings[CalibrationPoint_2].lTouchX   *
			  ((double) psPointADCReadings[CalibrationPoint_3].lTouchY   -
			   (double) psPointADCReadings[CalibrationPoint_1].lTouchY)) +
			  ((double) psPointADCReadings[CalibrationPoint_3].lTouchX   *
			  ((double) psPointADCReadings[CalibrationPoint_1].lTouchY   -
			   (double) psPointADCReadings[CalibrationPoint_2].lTouchY));

	llCalA =  llTemp1 / llTemp2;

	g_lTouchParameters[Coeff_A] = (long) (llCalA * RESCALE_FACTOR);

	//
	// Calibration coeff B
	//
	llTemp1 = (llCalA *
			  ((double) psPointADCReadings[CalibrationPoint_3].lTouchX   -
			   (double) psPointADCReadings[CalibrationPoint_2].lTouchX)) +
			   (double) sCalibrationPoints[CalibrationPoint_2].lTouchX   -
			   (double) sCalibrationPoints[CalibrationPoint_3].lTouchX;

	llTemp2 = (double) psPointADCReadings[CalibrationPoint_2].lTouchY -
			  (double) psPointADCReadings[CalibrationPoint_3].lTouchY;

	llCalB = llTemp1 / llTemp2;

	g_lTouchParameters[Coeff_B] = (long) (llCalB * RESCALE_FACTOR);

	//
	// Calibration coeff C
	//
	llCalC = (double) sCalibrationPoints[CalibrationPoint_3].lTouchX  -
			 (llCalA *
			 (double) psPointADCReadings[CalibrationPoint_3].lTouchX) -
			 (llCalB *
			 (double) psPointADCReadings[CalibrationPoint_3].lTouchY);

	g_lTouchParameters[Coeff_C] = (long) (llCalC * RESCALE_FACTOR);

	//
	// Calibration coeff D
	//
	llTemp1 = ((double) sCalibrationPoints[CalibrationPoint_1].lTouchY   *
			  ((double) psPointADCReadings[CalibrationPoint_2].lTouchY   -
			   (double) psPointADCReadings[CalibrationPoint_3].lTouchY)) +
			  ((double) sCalibrationPoints[CalibrationPoint_2].lTouchY   *
			  ((double) psPointADCReadings[CalibrationPoint_3].lTouchY   -
			   (double) psPointADCReadings[CalibrationPoint_1].lTouchY)) +
			  ((double) sCalibrationPoints[CalibrationPoint_3].lTouchY   *
			  ((double) psPointADCReadings[CalibrationPoint_1].lTouchY   -
			   (double) psPointADCReadings[CalibrationPoint_2].lTouchY));

	llTemp2 = ((double) psPointADCReadings[CalibrationPoint_1].lTouchX   *
			  ((double) psPointADCReadings[CalibrationPoint_2].lTouchY   -
			   (double) psPointADCReadings[CalibrationPoint_3].lTouchY)) +
			  ((double) psPointADCReadings[CalibrationPoint_2].lTouchX   *
			  ((double) psPointADCReadings[CalibrationPoint_3].lTouchY   -
			   (double) psPointADCReadings[CalibrationPoint_1].lTouchY)) +
			  ((double) psPointADCReadings[CalibrationPoint_3].lTouchX   *
			  ((double) psPointADCReadings[CalibrationPoint_1].lTouchY   -
			   (double) psPointADCReadings[CalibrationPoint_2].lTouchY));

	llCalD =  llTemp1 / llTemp2;

	g_lTouchParameters[Coeff_D] = (long) (llCalD * RESCALE_FACTOR);

	//
	// Calibration coeff E
	//
	llTemp1 = (llCalD *
			  ((double) psPointADCReadings[CalibrationPoint_3].lTouchX   -
			   (double) psPointADCReadings[CalibrationPoint_2].lTouchX)) +
			   (double) sCalibrationPoints[CalibrationPoint_2].lTouchY   -
			   (double) sCalibrationPoints[CalibrationPoint_3].lTouchY;

	llTemp2 = (double) psPointADCReadings[CalibrationPoint_2].lTouchY -
			  (double) psPointADCReadings[CalibrationPoint_3].lTouchY;

	llCalE = llTemp1 / llTemp2;

	g_lTouchParameters[Coeff_E] = (long) (llCalE * RESCALE_FACTOR);

	//
	// Calibration coeff F
	//
	llCalF = (double) sCalibrationPoints[CalibrationPoint_3].lTouchY  -
			 (llCalD *
			 (double) psPointADCReadings[CalibrationPoint_3].lTouchX) -
			 (llCalE *
			 (double) psPointADCReadings[CalibrationPoint_3].lTouchY);

	g_lTouchParameters[Coeff_F] = (long) (llCalF * RESCALE_FACTOR);

	//
	// Rescale factor
	//
	g_lTouchParameters[Coeff_G] = RESCALE_FACTOR;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
