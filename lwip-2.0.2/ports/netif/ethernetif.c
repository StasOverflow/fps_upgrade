/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "sys_error.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "lwip/err.h"
#include "netif/ethernetif.h"

#include <lib/ustdlib.h>
#include "netif/stm32f2x7_eth.h"



/* TCP and ARP timeouts */
volatile int tcp_end_time, arp_end_time;

/* Define those to better describe your network interface. */
#define IFNAME0                                  's'
#define IFNAME1                                  't'


#define  PHY_ADDRESS                             0x00

#define  PHY_DSCSR                               17

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif
{
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  int unused;
};


uint8_t             MACaddr[6];
ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];/* Ethernet Rx & Tx DMA Descriptors */
uint8_t             Rx_Buff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE], Tx_Buff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE];/* Ethernet buffers */

ETH_DMADESCTypeDef         *DMATxDesc = DMATxDscrTab;
extern __IO ETH_DMADESCTypeDef  *DMATxDescToSet;
extern __IO ETH_DMADESCTypeDef  *DMARxDescToGet;

FrameTypeDef ETH_RxPkt_ChainMode(void);
uint32_t          ETH_GetCurrentTxBuffer(void);
uint32_t          ETH_TxPkt_ChainMode(uint16_t FrameLength);

/* Forward declarations. */
err_t  ethernetif_input(struct netif *netif);



void
ethernetif_mac_get( uint8_t* macaddr )
{
	macaddr[0] = MACaddr[0];
	macaddr[1] = MACaddr[1];
	macaddr[2] = MACaddr[2];
	macaddr[3] = MACaddr[3];
	macaddr[4] = MACaddr[4];
	macaddr[5] = MACaddr[5];
}

/**
 * Setting the MAC address.
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
void
ethernetif_mac_set( uint8_t* macadd )
{
	MACaddr[0] = macadd[0];
	MACaddr[1] = macadd[1];
	MACaddr[2] = macadd[2];
	MACaddr[3] = macadd[3];
	MACaddr[4] = macadd[4];
	MACaddr[5] = macadd[5];

//	ETH_MACAddressConfig(ETH_MAC_Address0, macadd);
}


static void
ethernetif_phy_dump( void )
{
	LWIP_DEBUGF(PHY_DEBUG, ("Phy  BMCR: %"X16_F"\n", ETH_ReadPHYRegister(PHY_ADDRESS, PHY_BCR)));
	LWIP_DEBUGF(PHY_DEBUG, ("Phy  BMSR: %"X16_F"\n", ETH_ReadPHYRegister(PHY_ADDRESS, PHY_BSR)));
	LWIP_DEBUGF(PHY_DEBUG, ("Phy DSCSR: %"X16_F"\n", ETH_ReadPHYRegister(PHY_ADDRESS, PHY_DSCSR)));
}


uint32_t
ethernetif_phy_init( uint16_t PHYAddress )
{
	//
	// PHY transceiver initialization
	//
	GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_RESET);
	sys_arch_delay(100);
	GPIO_WriteBit(GPIOC, GPIO_Pin_0, Bit_SET);
	sys_arch_delay(500);

	/*-------------------- PHY initialization and configuration ----------------*/
	/* Put the PHY in reset mode */
	if( !(ETH_WritePHYRegister(PHYAddress, PHY_BCR, PHY_Reset)) )
	{
		/* Return ERROR in case of write timeout */
		return ETH_ERROR;
	}
	while( ETH_ReadPHYRegister(PHYAddress, PHY_BCR) & PHY_Reset ) ;


	if( !ETH_WritePHYRegister(PHYAddress, PHY_BCR, 0x2100) )
	{
		/* Return ERROR in case of write timeout */
		return ETH_ERROR;
	}

	/* Delay to assure PHY configuration */
	sys_arch_delay(PHY_CONFIG_DELAY);

	return ETH_SUCCESS;
}


void
stm32if_nvic_init( void )
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable the Ethernet global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


void
stm32if_gpio_init( void )
{
	GPIO_InitTypeDef gpio;


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	//
	// Enable SYSCFG clock
	//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);


	//
	// ETH_RST (PC0)
	//
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_Mode  = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_OD;
	gpio.GPIO_PuPd  = GPIO_PuPd_UP;
	gpio.GPIO_Pin   = GPIO_Pin_0;
	GPIO_Init(GPIOC, &gpio);

	//
	// MCO2 (PC9)
	//
	gpio.GPIO_Mode  = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	gpio.GPIO_Pin   = GPIO_Pin_9;
	GPIO_Init(GPIOC, &gpio);

	//
	// Configure MCO2 pin to output the HSE clock (25MHz)
	//
	RCC_MCO2Config(RCC_MCO2Source_HSE, RCC_MCO2Div_1);


	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	gpio.GPIO_Mode  = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	//
	// Select MII interface mode
	//
	SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);

	//
	// ETH_MII_CRS    (PA0)
	// ETH_MII_RX_CLK (PA1)
	// ETH_MDIO       (PA2)
	// ETH_MII_COL    (PA3)
	// ETH_MII_RX_DV  (PA7)
	//

	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &gpio);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);

	//
	// ETH_MII_RXD2  (PB0)
	// ETH_MII_RXD3  (PB1)
	// ETH_MII_TXD3  (PB8)
	// ETH_MII_RX_ER (PB10)
	//
	gpio.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_10;
	GPIO_Init(GPIOB, &gpio);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_ETH);

	//
	// ETH_MDC        (PC1)
	// ETH_MII_TXD2   (PC2)
	// ETH_MII_TX_CLK (PC3)
	// ETH_MII_RXD0   (PC4)
	// ETH_MII_RXD1   (PC5)
	//
	gpio.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &gpio);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);

	//
	// ETH_MII_TX_EN (PG11)
	// ETH_MII_TXD0  (PG13)
	// ETH_MII_TXD1  (PG14)
	//
	gpio.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOG, &gpio);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);
}


uint32_t
stm32if_mac_init( void )
{
	sys_prot_t         prot;
	uint32_t           tmpreg = 0;
	ETH_InitTypeDef    ETH_InitStructure;


	//
	// Enable Ethernet MAC clock
	//
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC |
	                       RCC_AHB1Periph_ETH_MAC_Tx | RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);


	//
	// Reset Ethernet MAC peripheral
	//
	RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_ETH_MAC, ENABLE);
	RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_ETH_MAC, DISABLE);

	//
	// PHY Reset
	//
	ethernetif_phy_init(PHY_ADDRESS);

	//
	// Perform a MAC software reset
	//
	ETH->DMABMR |= ETH_DMABMR_SR;
	ETH->DMABMR |= ETH_DMABMR_SR;
	ETH->DMABMR |= ETH_DMABMR_SR;
	ETH->DMABMR |= ETH_DMABMR_SR;
	while( ETH->DMABMR & ETH_DMABMR_SR )
	{
		sys_arch_delay(1);
	}

	/* ETHERNET Configuration ------------------------------------------------------*/
	/* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
	ETH_StructInit(&ETH_InitStructure);

	/* Fill ETH_InitStructure parametrs */
	/*------------------------   MAC   -----------------------------------*/
//	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable;
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
	#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
	#endif

	/*------------------------   DMA   -----------------------------------*/
	/* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
	the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum,
	if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

	ETH_InitStructure.ETH_ForwardErrorFrames          = ETH_ForwardErrorFrames_Disable;
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
	ETH_InitStructure.ETH_SecondFrameOperate          = ETH_SecondFrameOperate_Enable;
	ETH_InitStructure.ETH_AddressAlignedBeats         = ETH_AddressAlignedBeats_Enable;
	ETH_InitStructure.ETH_FixedBurst                  = ETH_FixedBurst_Enable;
	ETH_InitStructure.ETH_RxDMABurstLength            = ETH_RxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_TxDMABurstLength            = ETH_TxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_DMAArbitration              = ETH_DMAArbitration_RoundRobin_RxTx_2_1;


	/*-------------------------------- MAC Config ------------------------------*/
	/*---------------------- ETHERNET MACMIIAR Configuration -------------------*/
	/* Get the ETHERNET MACMIIAR value */
	tmpreg = ETH->MACMIIAR;

	/* Clear CSR Clock Range CR[2:0] bits */
	tmpreg &= MACMIIAR_CR_MASK;

	/* CSR Clock Range between 100-120 MHz */
	tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div62;

	/* Write to ETHERNET MAC MIIAR: Configure the ETHERNET CSR Clock Range */
	ETH->MACMIIAR = (uint32_t)tmpreg;


	prot = sys_arch_protect();
	/*------------------------ ETHERNET MACCR Configuration --------------------*/
	/* Get the ETHERNET MACCR value */
	tmpreg = ETH->MACCR;
	/* Clear WD, PCE, PS, TE and RE bits */
	tmpreg &= MACCR_CLEAR_MASK;
	/* Set the WD bit according to ETH_Watchdog value */
	/* Set the JD: bit according to ETH_Jabber value */
	/* Set the IFG bit according to ETH_InterFrameGap value */
	/* Set the DCRS bit according to ETH_CarrierSense value */
	/* Set the FES bit according to ETH_Speed value */
	/* Set the DO bit according to ETH_ReceiveOwn value */
	/* Set the LM bit according to ETH_LoopbackMode value */
	/* Set the DM bit according to ETH_Mode value */
	/* Set the IPCO bit according to ETH_ChecksumOffload value */
	/* Set the DR bit according to ETH_RetryTransmission value */
	/* Set the ACS bit according to ETH_AutomaticPadCRCStrip value */
	/* Set the BL bit according to ETH_BackOffLimit value */
	/* Set the DC bit according to ETH_DeferralCheck value */
	tmpreg |= (uint32_t)(ETH_InitStructure.ETH_Watchdog             |
						 ETH_InitStructure.ETH_Jabber               |
						 ETH_InitStructure.ETH_InterFrameGap        |
						 ETH_InitStructure.ETH_CarrierSense         |
						 ETH_InitStructure.ETH_Speed                |
						 ETH_InitStructure.ETH_ReceiveOwn           |
						 ETH_InitStructure.ETH_LoopbackMode         |
						 ETH_InitStructure.ETH_Mode                 |
						 ETH_InitStructure.ETH_ChecksumOffload      |
						 ETH_InitStructure.ETH_RetryTransmission    |
						 ETH_InitStructure.ETH_AutomaticPadCRCStrip |
						 ETH_InitStructure.ETH_BackOffLimit         |
						 ETH_InitStructure.ETH_DeferralCheck);

	/* Write to ETHERNET MACCR */
	ETH->MACCR = (uint32_t)tmpreg;
	ETH->MACCR = (uint32_t)tmpreg;
	ETH->MACCR = (uint32_t)tmpreg;
	ETH->MACCR = (uint32_t)tmpreg;

	/*----------------------- ETHERNET MACFFR Configuration --------------------*/
	/* Set the RA bit according to ETH_ReceiveAll value */
	/* Set the SAF and SAIF bits according to ETH_SourceAddrFilter value */
	/* Set the PCF bit according to ETH_PassControlFrames value */
	/* Set the DBF bit according to ETH_BroadcastFramesReception value */
	/* Set the DAIF bit according to ETH_DestinationAddrFilter value */
	/* Set the PR bit according to ETH_PromiscuousMode value */
	/* Set the PM, HMC and HPF bits according to ETH_MulticastFramesFilter value */
	/* Set the HUC and HPF bits according to ETH_UnicastFramesFilter value */
	/* Write to ETHERNET MACFFR */
	ETH->MACFFR = (uint32_t)(ETH_InitStructure.ETH_ReceiveAll               |
							 ETH_InitStructure.ETH_SourceAddrFilter         |
							 ETH_InitStructure.ETH_PassControlFrames        |
							 ETH_InitStructure.ETH_BroadcastFramesReception |
							 ETH_InitStructure.ETH_DestinationAddrFilter    |
							 ETH_InitStructure.ETH_PromiscuousMode          |
							 ETH_InitStructure.ETH_MulticastFramesFilter    |
							 ETH_InitStructure.ETH_UnicastFramesFilter);

	/*--------------- ETHERNET MACHTHR and MACHTLR Configuration ---------------*/
	/* Write to ETHERNET MACHTHR */
	ETH->MACHTHR = (uint32_t)ETH_InitStructure.ETH_HashTableHigh;

	/* Write to ETHERNET MACHTLR */
	ETH->MACHTLR = (uint32_t)ETH_InitStructure.ETH_HashTableLow;

	/*----------------------- ETHERNET MACFCR Configuration --------------------*/
	/* Get the ETHERNET MACFCR value */
	tmpreg = ETH->MACFCR;

	/* Clear xx bits */
	tmpreg &= MACFCR_CLEAR_MASK;

	/* Set the PT bit according to ETH_PauseTime value */
	/* Set the DZPQ bit according to ETH_ZeroQuantaPause value */
	/* Set the PLT bit according to ETH_PauseLowThreshold value */
	/* Set the UP bit according to ETH_UnicastPauseFrameDetect value */
	/* Set the RFE bit according to ETH_ReceiveFlowControl value */
	/* Set the TFE bit according to ETH_TransmitFlowControl value */
	tmpreg |= (uint32_t)((ETH_InitStructure.ETH_PauseTime << 16) |
						 ETH_InitStructure.ETH_ZeroQuantaPause |
						 ETH_InitStructure.ETH_PauseLowThreshold |
						 ETH_InitStructure.ETH_UnicastPauseFrameDetect |
						 ETH_InitStructure.ETH_ReceiveFlowControl |
						 ETH_InitStructure.ETH_TransmitFlowControl);

	/* Write to ETHERNET MACFCR */
	ETH->MACFCR = (uint32_t)tmpreg;
	/*----------------------- ETHERNET MACVLANTR Configuration -----------------*/
	/* Set the ETV bit according to ETH_VLANTagComparison value */
	/* Set the VL bit according to ETH_VLANTagIdentifier value */
	ETH->MACVLANTR = (uint32_t)(ETH_InitStructure.ETH_VLANTagComparison |
								ETH_InitStructure.ETH_VLANTagIdentifier);

	/*-------------------------------- DMA Config ------------------------------*/
	/*----------------------- ETHERNET DMAOMR Configuration --------------------*/
	/* Get the ETHERNET DMAOMR value */
	tmpreg = ETH->DMAOMR;

	/* Clear xx bits */
	tmpreg &= DMAOMR_CLEAR_MASK;

	/* Set the DT bit according to ETH_DropTCPIPChecksumErrorFrame value */
	/* Set the RSF bit according to ETH_ReceiveStoreForward value */
	/* Set the DFF bit according to ETH_FlushReceivedFrame value */
	/* Set the TSF bit according to ETH_TransmitStoreForward value */
	/* Set the TTC bit according to ETH_TransmitThresholdControl value */
	/* Set the FEF bit according to ETH_ForwardErrorFrames value */
	/* Set the FUF bit according to ETH_ForwardUndersizedGoodFrames value */
	/* Set the RTC bit according to ETH_ReceiveThresholdControl value */
	/* Set the OSF bit according to ETH_SecondFrameOperate value */
	tmpreg |= (uint32_t)(ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame |
						ETH_InitStructure.ETH_ReceiveStoreForward |
						ETH_InitStructure.ETH_FlushReceivedFrame |
						ETH_InitStructure.ETH_TransmitStoreForward |
						ETH_InitStructure.ETH_TransmitThresholdControl |
						ETH_InitStructure.ETH_ForwardErrorFrames |
						ETH_InitStructure.ETH_ForwardUndersizedGoodFrames |
						ETH_InitStructure.ETH_ReceiveThresholdControl |
						ETH_InitStructure.ETH_SecondFrameOperate);

	/* Write to ETHERNET DMAOMR */
	ETH->DMAOMR = (uint32_t)tmpreg;
	ETH->DMAOMR = (uint32_t)tmpreg;
	ETH->DMAOMR = (uint32_t)tmpreg;
	ETH->DMAOMR = (uint32_t)tmpreg;

	/*----------------------- ETHERNET DMABMR Configuration --------------------*/
	/* Set the AAL bit according to ETH_AddressAlignedBeats value */
	/* Set the FB bit according to ETH_FixedBurst value */
	/* Set the RPBL and 4*PBL bits according to ETH_RxDMABurstLength value */
	/* Set the PBL and 4*PBL bits according to ETH_TxDMABurstLength value */
	/* Set the DSL bit according to ETH_DesciptorSkipLength value */
	/* Set the PR and DA bits according to ETH_DMAArbitration value */
	ETH->DMABMR = (uint32_t)(ETH_InitStructure.ETH_AddressAlignedBeats |
							ETH_InitStructure.ETH_FixedBurst |
							ETH_InitStructure.ETH_RxDMABurstLength | /* !! if 4xPBL is selected for Tx or Rx it is applied for the other */
							ETH_InitStructure.ETH_TxDMABurstLength |
							(ETH_InitStructure.ETH_DescriptorSkipLength << 2) |
							ETH_InitStructure.ETH_DMAArbitration |
							ETH_DMABMR_USP | /* Enable use of separate PBL for Rx and Tx */
							ETH_DMABMR_EDE); /* Enable the Enhanced DMA descriptors */


	ETH_MACAddressConfig(ETH_MAC_Address0, MACaddr);

	/* Initialize Tx Descriptors list: Chain Mode */
	ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
	/* Initialize Rx Descriptors list: Chain Mode  */
	ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

	/* Enable Ethernet Rx interrrupt */
	for( int i = 0; i < ETH_RXBUFNB; i++ )
	{
		ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
	}

	/*----------------------- ETHERNET Start -----------------------------------*/

	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");

	/* Enable the MAC transmission and reception */
	ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
	ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
	ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
	ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
	ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
	ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;
	ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;

	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");

	/* Set the Flush Transmit FIFO bit */
	ETH->DMAOMR |= ETH_DMAOMR_FTF;
	ETH->DMAOMR |= ETH_DMAOMR_FTF;
	ETH->DMAOMR |= ETH_DMAOMR_FTF;
	ETH->DMAOMR |= ETH_DMAOMR_FTF;

	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");
	__asm volatile("nop");

	/* Enable the DMA transmission and reception */
	ETH->DMAOMR |= ETH_DMAOMR_ST | ETH_DMAOMR_SR;
	ETH->DMAOMR |= ETH_DMAOMR_ST | ETH_DMAOMR_SR;
	ETH->DMAOMR |= ETH_DMAOMR_ST | ETH_DMAOMR_SR;
	ETH->DMAOMR |= ETH_DMAOMR_ST | ETH_DMAOMR_SR;
	ETH->DMAOMR |= ETH_DMAOMR_ST | ETH_DMAOMR_SR;
	ETH->DMAOMR |= ETH_DMAOMR_ST | ETH_DMAOMR_SR;
	ETH->DMAOMR |= ETH_DMAOMR_ST | ETH_DMAOMR_SR;
	ETH->DMAOMR |= ETH_DMAOMR_ST | ETH_DMAOMR_SR;

	/* Enable the Ethernet Rx Interrupt */
	ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);

	sys_arch_unprotect(prot);

	/* Return Ethernet configuration success */
	return ETH_SUCCESS;
}


uint32_t  ethernet_rxpkt_size_get( void )
{
	return ETH_GetRxPktSize((ETH_DMADESCTypeDef *)DMARxDescToGet);
}


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output( struct netif *netif, struct pbuf *p )
{
	struct pbuf *q;

	int l = 0;
	u8 *buffer =  (u8 *)ETH_GetCurrentTxBuffer();

	for( q = p; q != NULL; q = q->next )
	{
		memcpy((u8_t*)&buffer[l], q->payload, q->len);
		l = l + q->len;
	}

	return ( ETH_TxPkt_ChainMode(l) == ETH_ERROR ) ? ERR_IF : ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input( struct netif *netif )
{
	struct pbuf *p, *q;
	u16_t len;
	int l =0;
	FrameTypeDef frame;
	u8 *buffer;

	p = NULL;
	frame = ETH_RxPkt_ChainMode();
	/* Obtain the size of the packet and put it into the "len"
	 variable. */
	len = frame.length;

	buffer = (u8 *)frame.buffer;

	/* We allocate a pbuf chain of pbufs from the pool. */
	p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

	if( p != NULL )
	{
		for (q = p; q != NULL; q = q->next)
		{
			memcpy((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
			l = l + q->len;
		}
	}


	/* Set Own bit of the Rx descriptor Status: gives the buffer back to ETHERNET DMA */
	frame.descriptor->Status = ETH_DMARxDesc_OWN;
#if 1
	/* When Rx Buffer unavailable flag is set: clear it and resume reception */
	if( (ETH->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET )
	{
		/* Clear RBUS ETHERNET DMA flag */
		ETH->DMASR = ETH_DMASR_RBUS;
		/* Resume DMA reception */
		ETH->DMARPDR = 0;
	}
#endif

	return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
err_t
ethernetif_input( struct netif *netif )
{
	err_t err;
	struct pbuf *p;

	/* move received packet into a new pbuf */
	p = low_level_input(netif);

	/* no packet could be read, silently ignore this */
	if( p == NULL ) return ERR_MEM;

	err = netif->input(p, netif);

	if( err != ERR_OK )
	{
		LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
		pbuf_free(p);
		p = NULL;
	}

	return err;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init( struct netif *netif )
{
	struct ethernetif *ethernetif;

	LWIP_ASSERT("netif != NULL", (netif != NULL));

	ethernetif = mem_malloc(sizeof(struct ethernetif));

	if( ethernetif == NULL )
	{
		LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
		return ERR_MEM;
	}

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

	/*
	* Initialize the snmp variables and counters inside the struct netif.
	* The last argument should be replaced with your link speed, in units
	* of bits per second.
	*/
	NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

	netif->state = ethernetif;
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	/* We directly use etharp_output() here to save a function call.
	* You can instead declare your own function an call etharp_output()
	* from it if you have to do some checks before sending (e.g. if link
	* is available...) */
	netif->output = etharp_output;
	netif->linkoutput = low_level_output;

	/* maximum transfer unit */
	netif->mtu = 1500;

	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* set MAC hardware address */
	netif->hwaddr[0] =  MACaddr[0];
	netif->hwaddr[1] =  MACaddr[1];
	netif->hwaddr[2] =  MACaddr[2];
	netif->hwaddr[3] =  MACaddr[3];
	netif->hwaddr[4] =  MACaddr[4];
	netif->hwaddr[5] =  MACaddr[5];

	/* device capabilities */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

	ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

	/* initialize the hardware */
	stm32if_gpio_init();

	stm32if_nvic_init();

	stm32if_mac_init();

	ethernetif_phy_dump();

	return ERR_OK;
}

/*******************************************************************************
* Function Name  : ETH_RxPkt_ChainMode
* Description    : Receives a packet.
* Input          : None
* Output         : None
* Return         : frame: farme size and location
*******************************************************************************/
FrameTypeDef ETH_RxPkt_ChainMode( void )
{ 
	uint32_t framelength = 0;
	FrameTypeDef frame = {0,0};

	/* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
	if( (DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (uint32_t)RESET )
	{
		frame.length = ETH_ERROR;

		if( (ETH->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET )
		{
			/* Clear RBUS ETHERNET DMA flag */
			ETH->DMASR = ETH_DMASR_RBUS;

			/* Resume DMA reception */
			ETH->DMARPDR = 0;
		}

		/* Return error: OWN bit set */
		return frame;
	}

	if( ((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET) &&
        ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (uint32_t)RESET) &&
        ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (uint32_t)RESET) )
	{
		/* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
		framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;

		/* Get the addrees of the actual buffer */
		frame.buffer = DMARxDescToGet->Buffer1Addr;
	}
	else
	{
		/* Return ERROR */
		framelength = ETH_ERROR;
	}

	frame.length = framelength;

	frame.descriptor = DMARxDescToGet;

	/* Update the ETHERNET DMA global Rx descriptor with next Rx decriptor */
	/* Chained Mode */
	/* Selects the next DMA Rx descriptor list for next buffer to read */
	DMARxDescToGet = (ETH_DMADESCTypeDef*) (DMARxDescToGet->Buffer2NextDescAddr);

	/* Return Frame */
	return (frame);
}

/*******************************************************************************
* Function Name  : ETH_TxPkt_ChainMode
* Description    : Transmits a packet, from application buffer, pointed by ppkt.
* Input          : - FrameLength: Tx Packet size.
* Output         : None
* Return         : ETH_ERROR: in case of Tx desc owned by DMA
*                  ETH_SUCCESS: for correct transmission
*******************************************************************************/
uint32_t ETH_TxPkt_ChainMode( uint16_t FrameLength )
{
	/* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
	if( (DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (uint32_t)RESET )
	{
		/* Return ERROR: OWN bit set */
		return ETH_ERROR;
	}

	/* Setting the Frame Length: bits[12:0] */
	DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1);

	/* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
	DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

	/* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
	DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

	/* When Tx Buffer unavailable flag is set: clear it and resume transmission */
	if( (ETH->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET )
	{
		/* Clear TBUS ETHERNET DMA flag */
		ETH->DMASR = ETH_DMASR_TBUS;
		/* Resume DMA transmission*/
		ETH->DMATPDR = 0;
	}

	/* Update the ETHERNET DMA global Tx descriptor with next Tx decriptor */
	/* Chained Mode */
	/* Selects the next DMA Tx descriptor list for next buffer to send */
	DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);


	/* Return SUCCESS */
	return ETH_SUCCESS;
}


/*******************************************************************************
* Function Name  : ETH_GetCurrentTxBuffer
* Description    : Return the address of the buffer pointed by the current descritor.
* Input          : None
* Output         : None
* Return         : Buffer address
*******************************************************************************/
uint32_t ETH_GetCurrentTxBuffer( void )
{ 
	/* Return Buffer address */
	return (DMATxDescToSet->Buffer1Addr);
}
