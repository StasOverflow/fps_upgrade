
#ifndef SRAM_H_
#define SRAM_H_

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


#define SRAM_BASE_ADDR              ( 0x60000000UL )
#if BOARD == 714153
    #define SRAM_SIZE               (  512 * 1024 )
#elif BOARD == 714200
    #define SRAM_SIZE               ( 1024 * 1024 )
#else
    #error "Unknown Board (pls refer to makefile)"
#endif


unsigned long  SRAM_TestStatusGet( void );
void           SRAM_Test( void );
void           SRAM_Init( void );


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif /* SRAM_H_ */
