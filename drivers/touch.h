//*****************************************************************************
//
// touch.h - Prototypes for the touch screen driver.
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

#ifndef __TOUCH_H__
#define __TOUCH_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif


//*****************************************************************************
//
// The lowest ADC reading assumed to represent a press on the screen.  Readings
// below this indicate no press is taking place.
//
//*****************************************************************************
#define TOUCH_ADC_MIN 200

//*****************************************************************************
enum
{
	CalibrationPoint_1,
	CalibrationPoint_2,
	CalibrationPoint_3,

	CALIBRATION_POINTS_NUM
};


enum
{
	Coord_X,
	Coord_Y,

	COORD_NUM
};


enum
{
	Coeff_A,
	Coeff_B,
	Coeff_C,
	Coeff_D,
	Coeff_E,
	Coeff_F,
	Coeff_G,

	NUM_TOUCH_PARAMS
};


typedef struct
{
	long lTouchX;
	long lTouchY;

} tTouchPoint;


//*****************************************************************************
//
// Prototypes for the functions exported by the touch screen driver.
//
//*****************************************************************************
extern volatile short g_sTouchADCX;
extern volatile short g_sTouchADCY;
extern volatile short g_sLastTouchADCX;
extern volatile short g_sLastTouchADCY;

extern volatile long  g_lLastTouchX;
extern volatile long  g_lLastTouchY;


extern short g_sTouchMin;
extern void TouchScreenIntHandler(void);
extern void TouchScreenInit(void);
extern void TouchScreenCallbackSet(long (*pfnCallback)(unsigned long ulMessage,
                                                       long lX, long lY));
extern void TouchScreenCalibrationPointCoordinatesGet( tTouchPoint *psTouchPoint, unsigned short usPoint );
extern void TouchScreenADCReadingsGet(short *psADCX, short *psADCY);
extern void TouchScreenSetCalibrationParams(long *lParams);
extern void TouchScreenGetCalibrationParams(long *lParams);
extern void TouchScreenCalibration(tTouchPoint *psPointADCReadings);


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif // __TOUCH_H__
