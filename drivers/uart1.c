
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


void USART1_IRQHandler( void )
{
	unsigned char ucData;

	if( USART_GetITStatus(USART1, USART_IT_RXNE) )
	{
    /* From stm32f2xx_usart.c
      *  @note
      * PE (Parity error), FE (Framing error), NE (Noise error), ORE (OverRun
      * error) and IDLE (Idle line detected) flags are cleared by software
      * sequence: a read operation to USART_SR register (USART_GetFlagStatus())
      * followed by a read operation to USART_DR register (USART_ReceiveData()).
     */
		if( !USART_GetFlagStatus(USART1, USART_FLAG_NE |
				                         USART_FLAG_FE |
				                         USART_FLAG_PE |
				                         USART_FLAG_ORE) )
		{
	        ucData = USART_ReceiveData(USART1);
			RxEnqueue(ucData);
		}
		else
		{
		    // To clear flags
            ucData = USART_ReceiveData(USART1);
		}

		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
	else if( USART_GetITStatus(USART1, USART_IT_TXE) )
	{
		if( TxDequeue(&ucData) )
		{
			USART_SendData(USART1, ucData);
		}
		else
		{
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
	}
	else if( USART_GetITStatus(USART1, USART_IT_TC) )
	{
		ucTransmittionComplete = SUCCESS;

		USART_ClearITPendingBit(USART1, USART_IT_TC);
	}
}


long UART1_RecvByte( unsigned char *pucByte )
{
	long lReturn;

	NVIC_DisableIRQ(USART1_IRQn);

	lReturn = RxDequeue(pucByte);

	NVIC_EnableIRQ(USART1_IRQn);

	return lReturn;
}


long UART1_SendByte( unsigned char ucByte )
{
	long lReturn;

	NVIC_DisableIRQ(USART1_IRQn);

	lReturn = TxEnqueue(ucByte);

	if( lReturn == SUCCESS )
	{
		ucTransmittionComplete = 0;

		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}

	NVIC_EnableIRQ(USART1_IRQn);

	return lReturn;
}


long UART1_IsTransmittionComplete( void )
{
	return ucTransmittionComplete;
}


void UART1_Flush( void )
{
	unsigned char ucByte;

	while( UART1_RecvByte(&ucByte) );

	USART_ReceiveData(USART1);
}


void
UART1_Configure( baudrate_t baud_lkp_id, parity_t par_lkp_id, stopbits_t sbit_lkp_id )
{
    USART_InitTypeDef USART_InitStructure;

    /* Disable USART */
    USART_Cmd(USART1, DISABLE);

    /* Configure USART */
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = baud_lkp[baud_lkp_id];
    USART_InitStructure.USART_WordLength = ( parity_lkp[par_lkp_id] != USART_Parity_No ) ?
            USART_WordLength_9b : USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = stopbit_lkp[sbit_lkp_id];
    USART_InitStructure.USART_Parity = parity_lkp[par_lkp_id];
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(USART1, ENABLE);
}


void
UART1_DeInit( void )
{
    /* Configure USART interrupts */
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART1, USART_IT_TC, DISABLE);

    /* Enable USART */
    USART_Cmd(USART1, DISABLE);
}


void UART1_Init( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* Enable USART clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Configure USART alternative functions */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/* Configure USART RX pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART TX pin */
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART */
	USART_StructInit(&USART_InitStructure);
	USART_Init(USART1, &USART_InitStructure);

	/* Configure USART IRQ */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Configure USART interrupts */
    USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
}
