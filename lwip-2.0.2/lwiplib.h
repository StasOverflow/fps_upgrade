
#ifndef __LWIPLIB_H__
#define __LWIPLIB_H__

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

#include <stdint.h>

//*****************************************************************************
//
// lwIP Options
//
//*****************************************************************************
#include <lwip/opt.h>


//*****************************************************************************
//
// Ensure that AUTOIP COOP option is configured correctly.
//
//*****************************************************************************
#undef LWIP_DHCP_AUTOIP_COOP
#define LWIP_DHCP_AUTOIP_COOP   ((LWIP_DHCP) && (LWIP_AUTOIP))

//*****************************************************************************
//
// lwIP API Header Files
//
//*****************************************************************************
#include "lwip/api.h"
#include "lwip/netifapi.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

#include "lwip/apps/snmp.h"

//*****************************************************************************
//
// IP Address Acquisition Modes
//
//*****************************************************************************
#define IPADDR_USE_STATIC       0
#define IPADDR_USE_DHCP         1
#define IPADDR_USE_AUTOIP       2

//*****************************************************************************
//
// lwIP Abstraction Layer API
//
//*****************************************************************************
uint32_t lwiplib_local_ipaddr_get( void );
uint32_t lwiplib_local_netmask_get( void );
uint32_t lwiplib_local_gwaddr_get( void );
void     lwiplib_local_mac_get( uint8_t *mac );
void     lwiplib_network_config_change( uint32_t ipaddr, uint32_t netmask,
                                        uint32_t gwaddr, uint32_t ipmode);

void     lwiplib_init( const uint8_t *mac, uint32_t ipaddr, uint32_t netmask,
                                           uint32_t gwaddr, uint32_t ipmode);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __LWIPLIB_H__
