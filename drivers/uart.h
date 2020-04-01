
#ifndef UART_H_
#define UART_H_

#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>

#include "serial_defs.h"

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

long UART1_RecvByte( unsigned char *pucByte );
long UART1_SendByte( unsigned char ucByte );
long UART1_IsTransmittionComplete( void );
void UART1_Flush( void );
void UART1_Configure( baudrate_t baud_lkp_id, parity_t par_lkp_id, stopbits_t sbit_lkp_id );
void UART1_DeInit( void );
void UART1_Init( void );

long UART3_RecvByte( unsigned char *pucByte );
long UART3_SendByte( unsigned char ucByte );
long UART3_IsTransmittionComplete( void );
void UART3_Flush( void );
void UART3_Configure( unsigned long ulBaudRate, unsigned short usParity );
void UART3_Init( void );


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif /* UART_H_ */
