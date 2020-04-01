
#include "net_property.h"

#if ( INCLUDE_NET == 1 )

#define DEFAULT_IP_ADDR             0xc0a80032
#define DEFAULT_NET_MASK            0xffffff00
#define DEFAULT_GW_ADDR             0x00000000
#define DEFAULT_NET_MODE            IPADDR_USE_STATIC


static const char *net_mode_message[] =
{
		"Static",
		"DHCP",
		"AutoIP"
};

#define NET_MODE_COUNT                 (sizeof(net_mode_message) / sizeof(net_mode_message[0]))

static unsigned char   net_mac_address[6] =
{
		0x00, 0x12, 0x37, 0x00, 0x00, 0x00
};

static unsigned long   net_ipaddr = DEFAULT_IP_ADDR;
static unsigned long   net_mask   = DEFAULT_NET_MASK;
static unsigned long   net_gwaddr = DEFAULT_GW_ADDR;
static unsigned long   net_mode   = DEFAULT_NET_MODE;

static tBoolean        is_load;

extern void ethernetif_mac_set( unsigned char * );

void net_addr_to_string( char *string, int iInsertLen, unsigned long ip )
{
	unsigned char group[4];

	group[3] = (ip >> 24) & 0xff;
	group[2] = (ip >> 16) & 0xff;
	group[1] = (ip >>  8) & 0xff;
	group[0] = (ip >>  0) & 0xff;

	usnprintf(string, iInsertLen, "%d.%d.%d.%d",
			group[0], group[1], group[2], group[3]);
}


tBoolean net_addr_check( unsigned long addr )
{
	tBoolean      is_ok;
	unsigned char group[4];

	is_ok    = true;
	group[0] = (addr >> 24) & 0xff;
	group[1] = (addr >> 16) & 0xff;
	group[2] = (addr >>  8) & 0xff;
	group[3] = (addr >>  0) & 0xff;

	if( (group[0] ==   0) || (group[3] ==   0) ||
		(group[0] == 127) ||
		(group[0] == 255) || (group[3] == 255) )
	{
		is_ok = false;
	}

	return is_ok;
}


tBoolean net_mask_check( unsigned long mask )
{
	unsigned long temp;

	temp  = (mask & -mask) -1;
	temp |= mask;
	temp  = ~temp;

	if( temp || (mask == 0xffffffff) )
		return (false);

	return (true);
}


tBoolean net_gwaddr_check( unsigned long addr )
{
	tBoolean      is_ok;
	unsigned char group[4];

	if( addr == 0 )
		return true;

	is_ok    = true;
	group[0] = (addr >> 24) & 0xff;
	group[1] = (addr >> 16) & 0xff;
	group[2] = (addr >>  8) & 0xff;
	group[3] = (addr >>  0) & 0xff;

	if( (group[0] ==   0) || (group[3] ==   0) ||
		(group[0] == 127) ||
		(group[0] == 255) || (group[3] == 255) )
	{
		is_ok = false;
	}

	return is_ok;
}


tBoolean net_property_set(
		unsigned long ip_addr,
		unsigned long mask,
		unsigned long gateway,
		unsigned long mode )
{
	tBoolean is_valid = false;

	if( net_addr_check(ip_addr) && net_mask_check(mask)  && net_gwaddr_check(gateway) && (mode < 3) )
	{
		net_ipaddr = ip_addr;
		net_mask   = mask;
		net_gwaddr = gateway;
		net_mode   = mode;

		lwiplib_network_config_change(net_ipaddr, net_mask, net_gwaddr, net_mode);

		net_settings_save();

		is_valid = true;
	}

	return is_valid;
}


void net_mac_address_string_get( char *pcString )
{
	usprintf(pcString, "%02x:%02x:%02x:%02x:%02x:%02x",
			net_mac_address[0], net_mac_address[1], net_mac_address[2],
			net_mac_address[3], net_mac_address[4], net_mac_address[5]);
}


unsigned char * net_mac_address_get( void )
{
	return net_mac_address;
}


void net_mac_address_set( unsigned char *mac_address )
{
	for( int i = 0; i < 6; i++ )
	{
		net_mac_address[i] = mac_address[i];
	}

	net_mac_address_save();
}


void net_ip_address_set( unsigned long ip_addr )
{

	net_property_set(ip_addr, net_mask, net_gwaddr, net_mode);

}


unsigned long  net_ip_address_get( void )
{
	return net_ipaddr;
}


void net_netmask_set( unsigned long mask )
{

	net_property_set(net_ipaddr, mask, net_gwaddr, net_mode);

}


unsigned long  net_netmask_get( void )
{
	return net_mask;
}


void net_gateway_set( u32 gw_addr )
{

	net_property_set(net_ipaddr, net_mask, gw_addr, net_mode);

}


unsigned long  net_gateway_get( void )
{
	return net_gwaddr;
}


tBoolean
net_mode_string_set( char *string )
{
	tBoolean is_valid = false;
	size_t   len;


	for( int i = 0; i < NET_MODE_COUNT; i++ )
	{
		len = strlen(net_mode_message[i]);

		if( strncmp(net_mode_message[i], string, len) == 0 )
		{
			is_valid = net_mode_set(i);
			break;
		}
	}

	return is_valid;
}


void net_mode_string_get( char *string )
{
	strcpy(string, net_mode_message[net_mode_get()]);
}


tBoolean net_mode_set( long mode )
{

	return net_property_set(net_ipaddr, net_mask, net_gwaddr, mode);

}


long net_mode_get( void )
{
	return net_mode;
}


tBoolean net_settings_default_set( void )
{
	tBoolean bChangedToDefault = false;

	if( net_ipaddr != DEFAULT_IP_ADDR )
	{
		net_ipaddr = DEFAULT_IP_ADDR;
		bChangedToDefault = true;
	}
	if( net_mask != DEFAULT_NET_MASK )
	{
		net_mask = DEFAULT_NET_MASK;
		bChangedToDefault = true;
	}
	if( net_gwaddr != DEFAULT_GW_ADDR )
	{
		net_gwaddr = DEFAULT_GW_ADDR;
		bChangedToDefault = true;
	}
	if( net_mode != DEFAULT_NET_MODE )
	{
		net_mode = DEFAULT_NET_MODE;
		bChangedToDefault = true;
	}

	if( bChangedToDefault )
	{
		net_property_set(net_ipaddr, net_mask, net_gwaddr, net_mode);
	}

	return bChangedToDefault;
}


void net_mac_address_save( void )
{
	unsigned short usDataBuffer[3];

	usDataBuffer[0]  = (unsigned short) net_mac_address[0] << 8;
	usDataBuffer[0] |= (unsigned short) net_mac_address[1];
	usDataBuffer[1]  = (unsigned short) net_mac_address[2] << 8;
	usDataBuffer[1] |= (unsigned short) net_mac_address[3];
	usDataBuffer[2]  = (unsigned short) net_mac_address[4] << 8;
	usDataBuffer[2] |= (unsigned short) net_mac_address[5];

	InterCommDataSet(NET_MAC_ADDR_0, usDataBuffer[0]);
	InterCommDataSet(NET_MAC_ADDR_1, usDataBuffer[1]);
	InterCommDataSet(NET_MAC_ADDR_2, usDataBuffer[2]);
}


void net_settings_save( void )
{
	unsigned short usDataBuffer[7];

	usDataBuffer[0] = net_mode;

	usDataBuffer[1] = (net_ipaddr >> 16);
	usDataBuffer[2] = (net_ipaddr & 0xFFFF);

	usDataBuffer[3] = (net_mask >> 16);
	usDataBuffer[4] = (net_mask & 0xFFFF);

	usDataBuffer[5] = (net_gwaddr >> 16);
	usDataBuffer[6] = (net_gwaddr & 0xFFFF);

	InterCommDataSet(NET_MODE,      usDataBuffer[0]);
	InterCommDataSet(NET_IP_0,      usDataBuffer[1]);
	InterCommDataSet(NET_IP_1,      usDataBuffer[2]);
	InterCommDataSet(NET_MASK_0,    usDataBuffer[3]);
	InterCommDataSet(NET_MASK_1,    usDataBuffer[4]);
	InterCommDataSet(NET_GATEWAY_0, usDataBuffer[5]);
	InterCommDataSet(NET_GATEWAY_1, usDataBuffer[6]);
}


void net_settings_load( void )
{
	net_ipaddr   = (unsigned long) InterCommDataGet(NET_IP_0) << 16;
	net_ipaddr  |= (unsigned long) InterCommDataGet(NET_IP_1) & 0xFFFF;

	net_mask     = (unsigned long) InterCommDataGet(NET_MASK_0) << 16;
	net_mask    |= (unsigned long) InterCommDataGet(NET_MASK_1) & 0xFFFF;

	net_gwaddr   = (unsigned long) InterCommDataGet(NET_GATEWAY_0) << 16;
	net_gwaddr  |= (unsigned long) InterCommDataGet(NET_GATEWAY_1) & 0xFFFF;

	net_mode     = InterCommDataGet(NET_MODE);

	net_mac_address[0] = InterCommDataGet(NET_MAC_ADDR_0) >> 8;
	net_mac_address[1] = InterCommDataGet(NET_MAC_ADDR_0) & 0xFF;
	net_mac_address[2] = InterCommDataGet(NET_MAC_ADDR_1) >> 8;
	net_mac_address[3] = InterCommDataGet(NET_MAC_ADDR_1) & 0xFF;
	net_mac_address[4] = InterCommDataGet(NET_MAC_ADDR_2) >> 8;
	net_mac_address[5] = InterCommDataGet(NET_MAC_ADDR_2) & 0xFF;

	ethernetif_mac_set(net_mac_address);

	net_property_set(net_ipaddr, net_mask, net_gwaddr, net_mode);

	is_load = true;
}


tBoolean net_property_is_load( void )
{
	return is_load;
}


void net_property_init( void )
{

}

#endif /* INCLUDE_NET == 1 */
