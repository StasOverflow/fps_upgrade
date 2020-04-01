/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//#include <lib/types.h>
#include <lwiplib.h>
#include "httpd.h"
#include "network.h"

#include "property/net_property.h"

#if BOARD == 714153
void
network_init(void)
{
	lwiplib_init(
			net_mac_address_get(),
			net_ip_address_get(),
			net_netmask_get(),
			net_gateway_get(),
			net_mode_get()
			);

	//
	// Start httpd thread
	//
	sys_thread_new("httpd", httpd_thread, 0, configMINIMAL_STACK_SIZE * 3, tskIDLE_PRIORITY + 2);
}
#elif BOARD == 714200

#include "netif/stm32f2x7_eth.h"
#include "netif/ethernetif.h"

#define mainWEBSERVER_PRIORITY      ( tskIDLE_PRIORITY + 2 )

#define DEFAULT_IP_ADDR             0xc0a80033
#define DEFAULT_NET_MASK            0xffffff00
#define DEFAULT_GW_ADDR             0x00000000
#define DEFAULT_NET_MODE            IPADDR_USE_STATIC
#define PHY_ADDRESS                             0x00


void
network_init(void)
{
    static unsigned char  mac_address[6] =
    {
            0x00, 0x12, 0x37, 0x00, 0x00, 0x01
    };


    //
    // Enable Ethernet MAC clock
    //
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC |
                           RCC_AHB1Periph_ETH_MAC_Tx | RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);


    //
    // Reset Ethernet MAC peripheral
    //
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_ETH_MAC, ENABLE);

    SYSCFG->PMC |= SYSCFG_PMC_MII_RMII_SEL;

    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_ETH_MAC, DISABLE);

    lwiplib_init(mac_address, DEFAULT_IP_ADDR, DEFAULT_NET_MASK, DEFAULT_GW_ADDR, DEFAULT_NET_MODE);

    //
    // Start httpd thread
    //
    sys_thread_new("httpd", httpd_thread, 0, configMINIMAL_STACK_SIZE * 3, tskIDLE_PRIORITY + 2);
}
#else
    #error "Unknown Board (pls refer to makefile)"
#endif
