#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>


static void FSMC_GPIO_Init( void )
{
	static unsigned long     ulInitComplete;
	       GPIO_InitTypeDef  GPIO_InitStruct;

	if( ulInitComplete )
	{
		return;
	}

	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

	/*
	 +-------------------+--------------------+-------------------+-------------------+
	 | PD0  <-> FSMC_D2  | PE7  <-> FSMC_D4   | PF0  <-> FSMC_A0  | PG0  <-> FSMC_A10 |
	 | PD1  <-> FSMC_D3  | PE8  <-> FSMC_D5   | PF1  <-> FSMC_A1  | PG1  <-> FSMC_A11 |
	 | PD3  <-> FSMC_CLK | PE9  <-> FSMC_D6   | PF2  <-> FSMC_A2  | PG2  <-> FSMC_A12 |
	 | PD4  <-> FSMC_NOE | PE10 <-> FSMC_D7   | PF3  <-> FSMC_A3  | PG3  <-> FSMC_A13 |
	 | PD5  <-> FSMC_NWE | PE11 <-> FSMC_D8   | PF4  <-> FSMC_A4  | PG4  <-> FSMC_A14 |
	 | PD7  <-> FSMC_NE1 | PE12 <-> FSMC_D9   | PF5  <-> FSMC_A5  | PG5  <-> FSMC_A15 |
	 | PD8  <-> FSMC_D13 | PE13 <-> FSMC_D10  | PF12 <-> FSMC_A6  | PG9  <-> FSMC_NE2 |
	 | PD9  <-> FSMC_D14 | PE14 <-> FSMC_D11  | PF13 <-> FSMC_A7  | PG10 <-> FSMC_NE3 |
	 | PD10 <-> FSMC_D15 | PE15 <-> FSMC_D12  | PF14 <-> FSMC_A8  | PG12 <-> FSMC_NE4 |
	 | PD11 <-> FSMC_A16 |--------------------| PF15 <-> FSMC_A9  |-------------------+
	 | PD12 <-> FSMC_A17 |                    +-------------------+
	 | PD14 <-> FSMC_D0  |
	 | PD15 <-> FSMC_D1  |
	 +-------------------+
	*/

	/* GPIOD configuration */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Pin  |= GPIO_Pin_9  | GPIO_Pin_8  | GPIO_Pin_5  | GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Pin  |= GPIO_Pin_3  | GPIO_Pin_0  | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Pin  |= GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	/* GPIOE configuration */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Pin  |= GPIO_Pin_10 | GPIO_Pin_9  | GPIO_Pin_8  | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);

	/* GPIOF configuration */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FSMC);

	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Pin  |= GPIO_Pin_4  | GPIO_Pin_3  | GPIO_Pin_2  | GPIO_Pin_1  | GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	/* GPIOG configuration */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource0,  GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource1,  GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource2,  GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource3,  GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource4,  GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource5,  GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource9,  GPIO_AF_FSMC);
//	GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_FSMC);
//	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);

	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_5  | GPIO_Pin_4  | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Pin  |= GPIO_Pin_2  | GPIO_Pin_1  | GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Pin  |= GPIO_Pin_9;
//	GPIO_InitStruct.GPIO_Pin  |= GPIO_Pin_10;
//	GPIO_InitStruct.GPIO_Pin  |= GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStruct);

	ulInitComplete = 1;
}


void FSMC_SRAM_Init( void )
{
	FSMC_NORSRAMInitTypeDef        FSMC_InitStruct;
	FSMC_NORSRAMTimingInitTypeDef  FSMCTiming_InitStruct;

	FSMC_GPIO_Init();

    /* FSMC Configuration */
#if BOARD == 714153
    FSMCTiming_InitStruct.FSMC_AddressSetupTime      = 1;
    FSMCTiming_InitStruct.FSMC_AddressHoldTime       = 0;
    FSMCTiming_InitStruct.FSMC_DataSetupTime         = 2;
#elif BOARD == 714200
	FSMCTiming_InitStruct.FSMC_AddressSetupTime      = 3;
	FSMCTiming_InitStruct.FSMC_AddressHoldTime       = 0;
	FSMCTiming_InitStruct.FSMC_DataSetupTime         = 5;
#else
    #error "Unknown Board (pls refer to makefile)"
#endif
	FSMCTiming_InitStruct.FSMC_BusTurnAroundDuration = 0;
	FSMCTiming_InitStruct.FSMC_CLKDivision           = 0;
	FSMCTiming_InitStruct.FSMC_DataLatency           = 0;
	FSMCTiming_InitStruct.FSMC_AccessMode            = FSMC_AccessMode_A;

	FSMC_InitStruct.FSMC_Bank                        = FSMC_Bank1_NORSRAM1;
	FSMC_InitStruct.FSMC_DataAddressMux              = FSMC_DataAddressMux_Disable;

#if BOARD == 714153
    FSMC_InitStruct.FSMC_MemoryType                  = FSMC_MemoryType_SRAM;
#elif BOARD == 714200
    FSMC_InitStruct.FSMC_MemoryType                  = FSMC_MemoryType_PSRAM;
#else
    #error "Unknown Board (pls refer to makefile)"
#endif
	FSMC_InitStruct.FSMC_MemoryDataWidth             = FSMC_MemoryDataWidth_16b;
	FSMC_InitStruct.FSMC_BurstAccessMode             = FSMC_BurstAccessMode_Disable;
	FSMC_InitStruct.FSMC_AsynchronousWait            = FSMC_AsynchronousWait_Disable;
	FSMC_InitStruct.FSMC_WaitSignalPolarity          = FSMC_WaitSignalPolarity_Low;
	FSMC_InitStruct.FSMC_WrapMode                    = FSMC_WrapMode_Disable;
	FSMC_InitStruct.FSMC_WaitSignalActive            = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_InitStruct.FSMC_WriteOperation              = FSMC_WriteOperation_Enable;
	FSMC_InitStruct.FSMC_WaitSignal                  = FSMC_WaitSignal_Disable;
	FSMC_InitStruct.FSMC_ExtendedMode                = FSMC_ExtendedMode_Disable;
	FSMC_InitStruct.FSMC_WriteBurst                  = FSMC_WriteBurst_Disable;
	FSMC_InitStruct.FSMC_ReadWriteTimingStruct       = &FSMCTiming_InitStruct;
	FSMC_InitStruct.FSMC_WriteTimingStruct           = &FSMCTiming_InitStruct;

	FSMC_NORSRAMInit(&FSMC_InitStruct);

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}


void FSMC_LCD_Init( void )
{
    FSMC_NORSRAMInitTypeDef        fsmc;
    FSMC_NORSRAMTimingInitTypeDef  fsmc_timing_conf;

    FSMC_GPIO_Init();

    /* FSMC Configuration */
    fsmc_timing_conf.FSMC_AddressSetupTime      = 0;
    fsmc_timing_conf.FSMC_AddressHoldTime       = 0;
    fsmc_timing_conf.FSMC_DataSetupTime         = 0;
    fsmc_timing_conf.FSMC_BusTurnAroundDuration = 0;
    fsmc_timing_conf.FSMC_CLKDivision           = 0;
    fsmc_timing_conf.FSMC_DataLatency           = 0;
    fsmc_timing_conf.FSMC_AccessMode            = FSMC_AccessMode_A;

    fsmc.FSMC_Bank                        = FSMC_Bank1_NORSRAM2;
    fsmc.FSMC_DataAddressMux              = FSMC_DataAddressMux_Disable;
    fsmc.FSMC_MemoryType                  = FSMC_MemoryType_PSRAM;
    fsmc.FSMC_MemoryDataWidth             = FSMC_MemoryDataWidth_16b;
    fsmc.FSMC_BurstAccessMode             = FSMC_BurstAccessMode_Disable;
    fsmc.FSMC_AsynchronousWait            = FSMC_AsynchronousWait_Disable;
    fsmc.FSMC_WaitSignalPolarity          = FSMC_WaitSignalPolarity_Low;
    fsmc.FSMC_WrapMode                    = FSMC_WrapMode_Disable;
    fsmc.FSMC_WaitSignalActive            = FSMC_WaitSignalActive_BeforeWaitState;
    fsmc.FSMC_WriteOperation              = FSMC_WriteOperation_Enable;
    fsmc.FSMC_WaitSignal                  = FSMC_WaitSignal_Disable;
    fsmc.FSMC_ExtendedMode                = FSMC_ExtendedMode_Disable;
    fsmc.FSMC_WriteBurst                  = FSMC_WriteBurst_Disable;
    fsmc.FSMC_ReadWriteTimingStruct       = &fsmc_timing_conf;
    fsmc.FSMC_WriteTimingStruct           = &fsmc_timing_conf;

    FSMC_NORSRAMInit(&fsmc);

    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM2, ENABLE);
}
