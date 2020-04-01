
#ifndef SPI_H_
#define SPI_H_

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


#define LCD_SPI_Init()                 SPI1_Init()
#define EEPROM_SPI_Init()              SPI1_Init()


void SPI1_Init( void );


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif


#endif /* SPI_H_ */
