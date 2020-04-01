
#ifndef MODBUS_SLAVE_H_
#define MODBUS_SLAVE_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif

#include "drivers/uart.h"


typedef struct
{
	unsigned short usRegStart;
	unsigned short usRegCnt;
	unsigned short (*RegRead)(unsigned short usIndex);
	unsigned short (*RegWrite)(unsigned short usIndex,
			                   unsigned short usRegValue);
} tModbusRegMap;


void ModbusSlaveProc( void );
void ModbusSlaveRegMapSet( tModbusRegMap *psRegMap );
void ModbusSlaveConfigure( unsigned long  ulBaudRate,
		                   unsigned short usParity,
				           unsigned char  ucSlaveID );
void ModbusSlaveInit( void );

/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif

#endif /* MODBUS_SLAVE_H_ */
