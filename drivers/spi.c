#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>


void SPI1_Init( void )
{
	static unsigned long ulInitComplete;

	if( ulInitComplete )
	{
		return;
	}

	SPI_InitTypeDef    SPI_InitStruct;
	GPIO_InitTypeDef   GPIO_InitStruct;

	/* Enable SPI1 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* Select SPI1 Alternative Function for SCK */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);

	/* Select SPI1 Alternative Function for MISO */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);

	/* Select SPI1 Alternative Function for MOSI */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

	/* Configure SPI SCK/MISO/MOSI GPIO */
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* SPI Configuration */
	SPI_StructInit(&SPI_InitStruct);
	SPI_InitStruct.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode              = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize          = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL              = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA              = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS               = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStruct.SPI_FirstBit          = SPI_FirstBit_MSB;
	SPI_Init(SPI1, &SPI_InitStruct);

	/* Enable SPI */
	SPI_Cmd(SPI1, ENABLE);

	ulInitComplete = 1;
}
