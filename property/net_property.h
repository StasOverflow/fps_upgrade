
#ifndef LAN_PROPERTY_H_
#define LAN_PROPERTY_H_

#include <lib/types.h>
#include <lwiplib.h>
#include <netif/ethernetif.h>
#include <property.h>

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

void net_addr_to_string( char *string, int iInsertLen, unsigned long ip );

tBoolean net_property_set(
		unsigned long ip_addr,
		unsigned long mask,
		unsigned long gateway,
		unsigned long mode );

void           net_ip_address_set( unsigned long ip_addr );
unsigned long  net_ip_address_get( void );

void           net_netmask_set( unsigned long net_mask );
unsigned long  net_netmask_get( void );

void           net_gateway_set( unsigned long gw_addr );
unsigned long  net_gateway_get( void );

tBoolean       net_mode_string_set( char *string );
void           net_mode_string_get( char *string );
tBoolean       net_mode_set( long mode );
long           net_mode_get( void );

void           net_mac_address_string_get( char *pcString );
unsigned char* net_mac_address_get( void );
void           net_mac_address_set( unsigned char *mac_address );
void           net_mac_address_save( void );

tBoolean       net_settings_default_set( void );
void           net_settings_save( void );
void           net_settings_load( void );

tBoolean       net_property_is_load( void );
void           net_property_init( void );

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* LAN_PROPERTY_H_ */
