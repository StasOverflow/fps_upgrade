

#include "lwiplib.h"

//*****************************************************************************
//
// Include lwIP high-level API code.
//
//*****************************************************************************
#include "lwip-2.0.2/src/api/api_lib.c"
#include "lwip-2.0.2/src/api/api_msg.c"
#include "lwip-2.0.2/src/api/err.c"
#include "lwip-2.0.2/src/api/netbuf.c"
#include "lwip-2.0.2/src/api/netdb.c"
#include "lwip-2.0.2/src/api/netifapi.c"
#include "lwip-2.0.2/src/api/sockets.c"
#include "lwip-2.0.2/src/api/tcpip.c"

//*****************************************************************************
//
// Include the core lwIP TCP/IP stack code.
//
//*****************************************************************************
#include "lwip-2.0.2/src/core/def.c"
#include "lwip-2.0.2/src/core/dns.c"
#include "lwip-2.0.2/src/core/inet_chksum.c"
#include "lwip-2.0.2/src/core/init.c"
#include "lwip-2.0.2/src/core/ip.c"
#include "lwip-2.0.2/src/core/mem.c"
//#include "drivers/mem.c"
#include "lwip-2.0.2/src/core/memp.c"
#include "lwip-2.0.2/src/core/netif.c"
#include "lwip-2.0.2/src/core/pbuf.c"
#include "lwip-2.0.2/src/core/raw.c"
#include "lwip-2.0.2/src/core/stats.c"
#include "lwip-2.0.2/src/core/sys.c"
#include "lwip-2.0.2/src/core/tcp.c"
#include "lwip-2.0.2/src/core/tcp_in.c"
#include "lwip-2.0.2/src/core/tcp_out.c"
#include "lwip-2.0.2/src/core/udp.c"
#include "lwip-2.0.2/src/core/timeouts.c"

//*****************************************************************************
//
// Include the IPV4 code.
//
//*****************************************************************************
#include "lwip-2.0.2/src/core/ipv4/autoip.c"
#include "lwip-2.0.2/src/core/ipv4/dhcp.c"
#include "lwip-2.0.2/src/core/ipv4/etharp.c"
#include "lwip-2.0.2/src/core/ipv4/icmp.c"
#include "lwip-2.0.2/src/core/ipv4/igmp.c"
#include "lwip-2.0.2/src/core/ipv4/ip4.c"
#include "lwip-2.0.2/src/core/ipv4/ip4_addr.c"
#include "lwip-2.0.2/src/core/ipv4/ip4_frag.c"

//*****************************************************************************
//
// Include the lwIP SNMP code.
//
//*****************************************************************************
#include "lwip-2.0.2/src/apps/snmp/snmp_core.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_asn1.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_mib2.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_mib2_ip.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_mib2_icmp.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_mib2_interfaces.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_mib2_snmp.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_mib2_system.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_mib2_tcp.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_mib2_udp.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_msg.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_netconn.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_pbuf_stream.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_raw.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_scalar.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_table.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_threadsync.c"
#include "lwip-2.0.2/src/apps/snmp/snmpv3.c"
#include "lwip-2.0.2/src/apps/snmp/snmp_traps.c"
#include "lwip-2.0.2/src/apps/snmp/snmpv3_dummy.c"
#include "lwip-2.0.2/src/apps/snmp/snmpv3_mbedtls.c"

//*****************************************************************************
//
// Include the Network Interface code.
//
//*****************************************************************************
#include "lwip-2.0.2/src/netif/ethernet.c"
#include "lwip-2.0.2/src/netif/ethernetif.c"

#include "ports/arch/perf.c"
#include "ports/arch/sys_arch.c"
#include "ports/netif/ethernetif.c"
#include "ports/netif/stm32f2x7_eth.c"

#include <lib/types.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define LWIP_INTERRUPT_STACK_SIZE      (configMINIMAL_STACK_SIZE * 2)
#define ETHERNETIF_LINK_CHECK_INTERVAL (  100  )


static struct netif     g_netif;

static uint32_t         g_ipmode = IPADDR_USE_STATIC;
static uint32_t         g_ipaddr;
static uint32_t         g_netmask;
static uint32_t         g_gwaddr;

static xSemaphoreHandle sem_lwip_interrupt;



unsigned long
lwiplib_local_ipaddr_get( void )
{
	return ( (uint32_t)g_netif.ip_addr.addr );
}


uint32_t
lwiplib_local_netmask_get( void )
{
	return ( (uint32_t)g_netif.netmask.addr );
}


uint32_t
lwiplib_local_gwaddr_get( void )
{
	return ( (uint32_t)g_netif.gw.addr );
}


void
lwiplib_local_mac_get( uint8_t *mac )
{
	ethernetif_mac_get(mac);
}

//*****************************************************************************
//
// Completes the network configuration change.  This is directly called when
// not using a RTOS and provided as a callback to the TCP/IP thread when using
// a RTOS.
//
//*****************************************************************************
static void
lwiplib_private_network_config_change(void *p_arg)
{
	unsigned long ip_mode;
	ip_addr_t  addr;
	ip_addr_t  net_mask;
	ip_addr_t  gw_addr;


	//
	// Get the new address mode.
	//
	ip_mode = (unsigned long)p_arg;

	//
	// Setup the network address values.
	//
	if( ip_mode == IPADDR_USE_STATIC )
	{
		addr.addr = htonl(g_ipaddr);
		net_mask.addr = htonl(g_netmask);
		gw_addr.addr = htonl(g_gwaddr);
	}
#if LWIP_DHCP || LWIP_AUTOIP
	else
	{
		addr.addr = 0;
		net_mask.addr = 0;
		gw_addr.addr = 0;
	}
#endif

	//
	// Switch on the current IP Address Aquisition mode.
	//
	switch( g_ipmode )
	{
		//
		// Static IP
		//
		case IPADDR_USE_STATIC:
		{
			//
			// Set the new address parameters.  This will change the address
			// configuration in lwIP, and if necessary, will reset any links
			// that are active.  This is valid for all three modes.
			//
			netif_set_addr(&g_netif, &addr, &net_mask, &gw_addr);

			//
			// If we are going to DHCP mode, then start the DHCP server now.
			//
	#if LWIP_DHCP
			if(ip_mode == IPADDR_USE_DHCP)
			{
				dhcp_start(&g_netif);
			}
	#endif

			//
			// If we are going to AutoIP mode, then start the AutoIP process
			// now.
			//
	#if LWIP_AUTOIP
			if(ip_mode == IPADDR_USE_AUTOIP)
			{
				autoip_start(&g_netif);
			}
	#endif
			//
			// And we're done.
			//
			break;
		}

		//
		// DHCP (with AutoIP fallback).
		//
	#if LWIP_DHCP
		case IPADDR_USE_DHCP:
		{
			//
			// If we are going to static IP addressing, then disable DHCP and
			// force the new static IP address.
			//
			if(ip_mode == IPADDR_USE_STATIC)
			{
				dhcp_stop(&g_netif);
				netif_set_addr(&g_netif, &addr, &net_mask, &gw_addr);
			}

			//
			// If we are going to AUTO IP addressing, then disable DHCP, set
			// the default addresses, and start AutoIP.
			//
	#if LWIP_AUTOIP
			else if(ip_mode == IPADDR_USE_AUTOIP)
			{
				dhcp_stop(&g_netif);
				netif_set_addr(&g_netif, &addr, &net_mask, &gw_addr);
				autoip_start(&g_netif);
			}
	#endif
			break;
		}
	#endif

		//
		// AUTOIP
		//
	#if LWIP_AUTOIP
		case IPADDR_USE_AUTOIP:
		{
			//
			// If we are going to static IP addressing, then disable AutoIP and
			// force the new static IP address.
			//
			if(ip_mode == IPADDR_USE_STATIC)
			{
				autoip_stop(&g_netif);
				netif_set_addr(&g_netif, &addr, &net_mask, &gw_addr);
			}

			//
			// If we are going to DHCP addressing, then disable AutoIP, set the
			// default addresses, and start dhcp.
			//
	#if LWIP_DHCP
			else if(ip_mode == IPADDR_USE_DHCP)
			{
				autoip_stop(&g_netif);
				netif_set_addr(&g_netif, &addr, &net_mask, &gw_addr);
				dhcp_start(&g_netif);
			}
	#endif
			break;
		}
#endif
	}

	//
	// Save the new mode.
	//
	g_ipmode = ip_mode;
}


void
lwiplib_network_config_change( uint32_t ipaddr, uint32_t netmask,
                               uint32_t gwaddr, uint32_t ipmode)
{
	//
	// Save the network configuration for later use by the private network
	// configuration change.
	//
	g_ipaddr  = ipaddr;
	g_netmask = netmask;
	g_gwaddr  = gwaddr;

	//
	// Complete the network configuration change.  The remainder is done
	// immediately if not using a RTOS and it is deferred to the TCP/IP
	// thread's context if using a RTOS.
	//
	tcpip_callback(lwiplib_private_network_config_change, (void *)ipmode);
}


static void
lwiplib_ethernetif_link_check( void *arg )
{
	static int8_t is_init;

	if( ETH_ReadPHYRegister(0x00, PHY_BSR) & PHY_Linked_Status )
	{
		if( !netif_is_link_up(&g_netif) )
		{
			//
			// Bring the interface UP.
			//
			netif_set_link_up(&g_netif);
			netif_set_up(&g_netif);

			if( !is_init )
			{
				is_init = true;

				//
				// Start DHCP, if enabled.
				//
			#if LWIP_DHCP
				if( g_ipmode == IPADDR_USE_DHCP )
				{
					dhcp_start(&g_netif);
				}
			#endif

				//
				// Start AutoIP, if enabled and DHCP is not.
				//
			#if LWIP_AUTOIP
				if( g_ipmode == IPADDR_USE_AUTOIP )
				{
					autoip_start(&g_netif);
				}
			#endif
			}
		}
	}
	else
	{
		if( netif_is_link_up(&g_netif) )
		{
			//
			// Bring the interface DOWN.
			//
			netif_set_link_down(&g_netif);
			netif_set_down(&g_netif);
		}
	}

	sys_timeout(ETHERNETIF_LINK_CHECK_INTERVAL, lwiplib_ethernetif_link_check, NULL);
}


static void
lwiplib_interrupt_task( void *pvArg )
{
	//
	// Loop forever.
	//
	while( 1 )
	{
		//
		// Wait until the semaphore has been signalled.
		//
		while( xSemaphoreTake(sem_lwip_interrupt, portMAX_DELAY) != pdPASS );

		//
		// Processes any packets waiting to be sent or received.
		//
		while( ethernet_rxpkt_size_get() != 0 )
		{
			ethernetif_input(&g_netif);
		}
	}
}


//*****************************************************************************
//
// Completes the initialization of lwIP.  This is directly called when not
// using a RTOS and provided as a callback to the TCP/IP thread when using a
// RTOS.
//
//*****************************************************************************
static void
lwiplib_private_init( void *p_arg )
{
	ip_addr_t  addr;
	ip_addr_t  net_mask;
	ip_addr_t  gw_addr;

	//
	// If using a RTOS, create a queue (to be used as a semaphore) to signal
	// the Ethernet interrupt task from the Ethernet interrupt handler.
	//
	portENTER_CRITICAL();
	vSemaphoreCreateBinary( sem_lwip_interrupt );
	xSemaphoreTake(sem_lwip_interrupt, portMAX_DELAY);

	//
	// If using a RTOS, create the Ethernet interrupt task.
	//
	sys_thread_new((portCHAR *)"eth_int", lwiplib_interrupt_task, 0,
			LWIP_INTERRUPT_STACK_SIZE, LWIP_INTERRUPT_TASK_PRIO);
	portEXIT_CRITICAL();

	//
	// Setup the network address values.
	//
	if( g_ipmode == IPADDR_USE_STATIC )
	{
		addr.addr = htonl(g_ipaddr);
		net_mask.addr = htonl(g_netmask);
		gw_addr.addr = htonl(g_gwaddr);
	}
	else
	{
		addr.addr = 0;
		net_mask.addr = 0;
		gw_addr.addr = 0;
	}

	//
	// Create, configure and add the Ethernet controller interface with
	// default settings.  ip_input should be used to send packets directly to
	// the stack when not using a RTOS and tcpip_input should be used to send
	// packets to the TCP/IP thread's queue when using a RTOS.
	//
	netif_add(&g_netif, &addr, &net_mask, &gw_addr, NULL, ethernetif_init, tcpip_input);

	netif_set_default(&g_netif);

	sys_timeout(ETHERNETIF_LINK_CHECK_INTERVAL, lwiplib_ethernetif_link_check, NULL);
}


void
lwiplib_init( const uint8_t *mac, uint32_t ipaddr, uint32_t netmask,
                                  uint32_t gwaddr, uint32_t ipmode )
{
	//
	// Save the network configuration for later use by the private
	// initialization.
	//
	g_ipmode = ipmode;
	g_ipaddr = ipaddr;
	g_netmask = netmask;
	g_gwaddr = gwaddr;

	ethernetif_mac_set((unsigned char *)mac);

	//
	// Initialize lwIP.  The remainder of initialization is done immediately if
	// not using a RTOS and it is deferred to the TCP/IP thread's context if
	// using a RTOS.
	//
	tcpip_init(lwiplib_private_init, 0);
}


void
ethernetif_int_handler( void )
{
	portBASE_TYPE xWake = pdFALSE;

	//
	// A RTOS is being used.  Signal the Ethernet interrupt task.
	//
	xSemaphoreGiveFromISR(sem_lwip_interrupt, &xWake); // ??? or use queue as original


	/* Clear the Eth DMA Rx IT pending bits */
	ETH->DMASR = ETH_DMA_IT_NIS | ETH_DMA_IT_R;


	//
	// Potentially task switch as a result of the above queue write.
	//
	portEND_SWITCHING_ISR(xWake);
}

