
#ifndef EXT_COMM_H_
#define EXT_COMM_H_

/****
 *  C Binding header for C++ compiler
 */
#ifdef __cplusplus
extern "C"
{
#endif


void           ExtCommSetupUpdate( void );
unsigned short ExtCommDataGet( unsigned short usIndex );
unsigned short ExtCommDataSet( unsigned short usIndex,
		                       unsigned short usRegValue );

void ExtCommTask( void *pvParameters );


/****
 *  Mark the end of the C Binding section for C++ compilers.
 */
#ifdef __cplusplus
}
#endif

#endif /* EXT_COMM_H_ */
