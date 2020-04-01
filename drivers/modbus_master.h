
#ifndef MODBUS_MASTER_H_
#define MODBUS_MASTER_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif

#include "drivers/uart.h"




#define MB_OK                               0
#define MB_ERROR_FORMAT                    -1
#define MB_ERROR_LENGTH                    -2
#define MB_ERROR_PARAMETER                 -3
#define MB_ERROR_OPEN                      -4
#define MB_ERROR_TCP_NOT_CONNECTED         -5
#define MB_ERROR_NOT_OPENED                -6
#define MB_ERROR_BUFFER_TOO_SHORT          -7
#define MB_ERROR_NO_FRAME                  -8
#define MB_ERROR_TIMEOUT                   -9
#define MB_ERROR_EXECPTION                -10
#define MB_ERROR_BAD_FD                   -11
#define MB_ERROR_NET                      -12
#define MB_ERROR_NO_FILE_HANDLE           -13
#define MB_ERROR_PROTOCOL                 -14
#define	MB_ERROR_FUNCTION                 -15
#define	MB_ERROR_MODE                     -16


long    ModbusMasterWriteMultipleRegs( unsigned char  ucSlaveID,
                                       unsigned short usReg,
                                       unsigned short usRegNum,
                                       unsigned short *pusInput );

long    ModbusMasterReadInputRegs( unsigned char  ucSlaveID,
                                   unsigned short usReg,
                                   unsigned short usRegNum,
                                   unsigned short *pusOutput );

void    ModbusMasterConfigure( baudrate_t br_lkp_id, parity_t par_lkp_id, stopbits_t sbit_lkp_id );
void    ModbusMasterInit( void );

/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif


#endif /* MODBUS_MASTER_H_ */
