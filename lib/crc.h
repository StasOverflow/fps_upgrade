
#ifndef CRC_H_
#define CRC_H_

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


unsigned short  crc_16_ibm( unsigned char * pucFrame, unsigned short usLen );
unsigned char   crc_8_dallas( unsigned char *pcBlock, unsigned char len );


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif /* CRC_H_ */
