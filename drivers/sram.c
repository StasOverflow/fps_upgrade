#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>

#include "fsmc.h"
#include "sram.h"

#define     TEST_DATA_QUANTITY          3

static unsigned long        ulSRAMTestStatus;
static unsigned long        broken_reg;
static unsigned short       broken_val;
static unsigned short       broken_val_read;

unsigned short test_dat[TEST_DATA_QUANTITY] = {0xAAAA, 0x5555, 0x0};

unsigned long
SRAM_TestStatusGet( void )
{
    return ulSRAMTestStatus;
}


unsigned long
SRAM_reg_broken_get( void )
{
    return broken_reg;
}


unsigned short
SRAM_val_broken_get( void )
{
    return broken_val;
}


unsigned short
SRAM_val_read_broken_get( void )
{
    return broken_val_read;
}


void
SRAM_Test( void )
{
    unsigned short index;
    unsigned short read_data;
    unsigned char  led_state = 0;

    for( unsigned long ulAddr = SRAM_BASE_ADDR; ulAddr < (SRAM_BASE_ADDR + SRAM_SIZE); ulAddr++ )
    {
        index = 0;

        if( ulAddr & 0xf0 )
        {
            led_state ^= 1;
        }

//        if( led_state )
//        {
//            PIN_SET(LED_RED_PIN);
//        }
//        else
//        {
//            PIN_CLR(LED_RED_PIN);
//        }

        do
        {
            *(volatile unsigned long *)ulAddr = test_dat[index];
            read_data = *(volatile unsigned long *)ulAddr;

            broken_reg = ulAddr;
            broken_val = test_dat[index];
            broken_val_read = read_data;

            if( read_data != test_dat[index] )
            {
                return;
            }

            index++;
        }
        while( index < TEST_DATA_QUANTITY );
    }

    ulSRAMTestStatus = 1;
}


void SRAM_Init( void )
{
	FSMC_SRAM_Init();
}
