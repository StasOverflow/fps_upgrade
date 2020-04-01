/*
 * fsmc.h
 *
 *  Created on: 05.01.2015
 *      Author: V.Dudnik
 */

#ifndef FSMC_H_
#define FSMC_H_

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


void FSMC_SRAM_Init( void );
void FSMC_LCD_Init( void );


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* FSMC_H_ */
