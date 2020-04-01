
#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>

#include "uart.h"

#define RX_QUEUE_SIZE   ( 16 )

static unsigned char    ucRxQueue[RX_QUEUE_SIZE];
static unsigned short   usRxQueueReadIndex;
static unsigned short   usRxQueueWriteIndex;
static unsigned char    ucRxQueueFull;

#define TX_QUEUE_SIZE   ( 256 )

static unsigned char    ucTxQueue[TX_QUEUE_SIZE];
static unsigned short   usTxQueueReadIndex;
static unsigned short   usTxQueueWriteIndex;
static unsigned char    ucTxQueueFull;

static unsigned char    ucTransmittionComplete;


static
long RxEnqueue( unsigned char ucData )
{
	long lReturn = ERROR;

	if( !ucRxQueueFull )
	{
		ucRxQueue[usRxQueueWriteIndex] = ucData;

		usRxQueueWriteIndex = (usRxQueueWriteIndex + 1) % RX_QUEUE_SIZE;

		if( usRxQueueWriteIndex == usRxQueueReadIndex )
		{
			ucRxQueueFull = 1;
		}

		lReturn = SUCCESS;
	}

	return lReturn;
}


static
long RxDequeue( unsigned char *pucByte )
{
	long lReturn = ERROR;

	if( (usRxQueueReadIndex != usRxQueueWriteIndex) || ucRxQueueFull )
	{
		*pucByte = ucRxQueue[usRxQueueReadIndex];

		usRxQueueReadIndex = (usRxQueueReadIndex + 1) % RX_QUEUE_SIZE;

		ucRxQueueFull = 0;

		lReturn = SUCCESS;
	}

	return lReturn;
}


static
long TxEnqueue( unsigned char ucByte )
{
	long lReturn = ERROR;

	if( !ucTxQueueFull )
	{
		ucTxQueue[usTxQueueWriteIndex] = ucByte;

		usTxQueueWriteIndex = (usTxQueueWriteIndex + 1) % TX_QUEUE_SIZE;

		if( usTxQueueWriteIndex == usTxQueueReadIndex )
		{
			ucTxQueueFull = 1;
		}

		lReturn = SUCCESS;
	}

	return lReturn;
}


static
long TxDequeue( unsigned char *pucByte )
{
	long lReturn = ERROR;

	if( (usTxQueueReadIndex != usTxQueueWriteIndex) || ucTxQueueFull )
	{
		*pucByte = ucTxQueue[usTxQueueReadIndex];

		usTxQueueReadIndex = (usTxQueueReadIndex + 1) % TX_QUEUE_SIZE;

		ucTxQueueFull = 0;

		lReturn = SUCCESS;
	}

	return lReturn;
}


void USART3_IRQHandler( void )
{
	unsigned char ucData;

	if( USART_GetITStatus(USART3, USART_IT_RXNE) )
	{
		ucData = USART_ReceiveData(USART3);

		if( !USART_GetFlagStatus(USART3, USART_FLAG_NE |
				                         USART_FLAG_FE |
				                         USART_FLAG_PE |
				                         USART_FLAG_ORE) )
		{
			RxEnqueue(ucData);
		}
		else
		{
			USART_ClearFlag(USART3, USART_FLAG_NE |
                                    USART_FLAG_FE |
                                    USART_FLAG_PE |
                                    USART_FLAG_ORE);
		}

		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
	else if( USART_GetITStatus(USART3, USART_IT_TXE) )
	{
		if( TxDequeue(&ucData) )
		{
			USART_SendData(USART3, ucData);
		}
		else
		{
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		}
	}
	else if( USART_GetITStatus(USART3, USART_IT_TC) )
	{
		ucTransmittionComplete = SUCCESS;

		USART_ClearITPendingBit(USART3, USART_IT_TC);
	}
}


long UART3_RecvByte( unsigned char *pucByte )
{
	long lReturn;

	NVIC_DisableIRQ(USART3_IRQn);

	lReturn = RxDequeue(pucByte);

	NVIC_EnableIRQ(USART3_IRQn);

	return lReturn;
}


long UART3_SendByte( unsigned char ucByte )
{
	long lReturn;

	NVIC_DisableIRQ(USART3_IRQn);

	lReturn = TxEnqueue(ucByte);

	if( lReturn == SUCCESS )
	{
		ucTransmittionComplete = 0;

		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}

	NVIC_EnableIRQ(USART3_IRQn);

	return lReturn;
}


long UART3_IsTransmittionComplete( void )
{
	return ucTransmittionComplete;
}


void UART3_Flush( void )
{
	unsigned char ucByte;

	while( UART3_RecvByte(&ucByte) );

	USART_ReceiveData(USART3);
}


void UART3_Configure( unsigned long ulBaudRate, unsigned short usParity )
{
	USART_InitTypeDef USART_InitStructure;

	/* Disable USART */
	USART_Cmd(USART3, DISABLE);

	/* Configure USART */
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = ulBaudRate;
	USART_InitStructure.USART_WordLength = ( usParity != USART_Parity_No ) ?
			USART_WordLength_9b : USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = usParity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	/* Enable USART */
	USART_Cmd(USART3, ENABLE);
}


void UART3_Init( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* Configure USART alternative functions */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

	/* Configure USART RX pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure USART TX pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Configure USART */
	USART_StructInit(&USART_InitStructure);
	USART_Init(USART3, &USART_InitStructure);

	/* Configure USART IRQ */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Configure USART interrupts */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART3, USART_IT_TC, ENABLE);

	/* Enable USART */
	USART_Cmd(USART3, ENABLE);
}
