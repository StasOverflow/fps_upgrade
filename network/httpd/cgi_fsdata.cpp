#include <lwip/init.h>
#include "fsdata.h"
#include "cgi.h"
#include "json.h"

#include "property/property.h"
#include "property/input_property.h"
#include "property/psu_property.h"
#include "property/ac_output_property.h"
#include "property/dc_output_property.h"
#include "property/ups_property.h"
#include "property/battery_property.h"
#include "property/inverter_property.h"
#include "property/net_property.h"


//static char cgi_out[2048];


#define PLAIN_HEADER                "Content-type: text/plain\r\n"             \
                                    "Cache-Control: no-cache, no-store\r\n"    \
									"Connection: close\r\n\r\n"

#define JSON_HEADER                 "Content-type: application/json\r\n"       \
                                    "Cache-Control: no-cache, no-store\r\n"    \
									"Connection: close\r\n\r\n"

#define HTML_HEADER                 "Content-type: text/html\r\n"              \
                                    "Cache-Control: no-cache, no-store\r\n"    \
									"Connection: close\r\n\r\n"

int
status_cgi_handler(char *out, int len, char *uri_params)
{
	char *p = out;
#if 0
	char  pcParam[32];
	char  pcValue[32];
	short sValue;
	static const char *relay_message_ru[] =
	{
			"Выкл.",
			"Вкл.",
	};
#endif

	//
	// Set http headers
	//
	p += usprintf(p, "%s", JSON_HEADER);

	// BEGIN
	p = json_begin(p);

#if 0
	if( UPSPropertyGet(SYSTEM_BYPASS_STATE) )
	{
		p = json_node_add(p, "bypass_status", relay_message_ru[1]);
	}
	else
	{
		p = json_node_add(p, "bypass_status", relay_message_ru[0]);
	}

	if( UPSPropertyGet(SYSTEM_STATUS) == UPS_STATUS_ALARM )
	{
		p = json_node_add(p, "relay_1_status", relay_message_ru[1]);
	}
	else
	{
		p = json_node_add(p, "relay_1_status", relay_message_ru[0]);
	}

	BatteryPropertyStringGet(0, BATTERY_CURRENT, pcValue);

	if( *pcValue == '-' )
	{
		p = json_node_add(p, "relay_2_status", relay_message_ru[1]);
	}
	else
	{
		p = json_node_add(p, "relay_2_status", relay_message_ru[0]);
	}

	sValue = BatteryPropertyGet(0, BATTERY_LEVEL);

	if( sValue <= 2 )			//2 for 3rd cell in battery level enum
	{
		p = json_node_add(p, "relay_3_status", relay_message_ru[1]);
	}
	else
	{
		p = json_node_add(p, "relay_3_status", relay_message_ru[0]);
	}

	// System section
	UPSPropertyStringGet(SYSTEM_TEMPERATURE_EXTERNAL, pcValue);
	p = json_node_add(p, "sys_external_temperature", pcValue);

	// Input section
	InputPhasePropertyStringGet(0, INPUT_VOLTAGE, pcValue);
	p = json_node_add(p, "input_voltage_a", pcValue);
	InputPhasePropertyStringGet(1, INPUT_VOLTAGE, pcValue);
	p = json_node_add(p, "input_voltage_b", pcValue);
	InputPhasePropertyStringGet(2, INPUT_VOLTAGE, pcValue);
	p = json_node_add(p, "input_voltage_c", pcValue);
	InputPhasePropertyStringGet(0, INPUT_CURRENT, pcValue);
	p = json_node_add(p, "input_current_a", pcValue);
	InputPhasePropertyStringGet(1, INPUT_CURRENT, pcValue);
	p = json_node_add(p, "input_current_b", pcValue);
	InputPhasePropertyStringGet(2, INPUT_CURRENT, pcValue);
	p = json_node_add(p, "input_current_c", pcValue);
	InputPhasePropertyStringGet(0, INPUT_POWER, pcValue);
	p = json_node_add(p, "input_power_a", pcValue);
	InputPhasePropertyStringGet(1, INPUT_POWER, pcValue);
	p = json_node_add(p, "input_power_b", pcValue);
	InputPhasePropertyStringGet(2, INPUT_POWER, pcValue);
	p = json_node_add(p, "input_power_c", pcValue);

#endif

#if ( PSU_COUNT > 0 )
	for( int i = PSU_GROUP_1_START_INDEX, iUnit = 1; i <= PSU_GROUP_1_END_INDEX; i++ )
	{
		if( PSUPropertyGet(i, PSU_STATUS) != PSU_STATUS_UNKNOWN )
		{
			usprintf(pcParam, "charger_%d_voltage", iUnit);
			PSUPropertyStringGet(i, PSU_OUTPUT_VOLTAGE, pcValue);
			p = json_node_add(p, pcParam, pcValue);
			usprintf(pcParam, "charger_%d_current", iUnit);
			PSUPropertyStringGet(i, PSU_OUTPUT_CURRENT, pcValue);
			p = json_node_add(p, pcParam, pcValue);
			usprintf(pcParam, "charger_%d_convtemp", iUnit);
			PSUPropertyStringGet(i, PSU_TEMPERATURE, pcValue);
			p = json_node_add(p, pcParam, pcValue);
			usprintf(pcParam, "charger_%d_status", iUnit);
			PSUPropertyStringGet(i, PSU_STATUS, pcValue);
			p = json_node_add(p, pcParam, pcValue);
		}
		else
		{
			usprintf(pcParam, "charger_%d_voltage", iUnit);
			p = json_node_add(p, pcParam, "-");
			usprintf(pcParam, "charger_%d_current", iUnit);
			p = json_node_add(p, pcParam, "-");
			usprintf(pcParam, "charger_%d_convtemp", iUnit);
			p = json_node_add(p, pcParam, "-");
			usprintf(pcParam, "charger_%d_status", iUnit);
			p = json_node_add(p, pcParam, "-");
		}

		iUnit++;
	}
#endif


#if 0
	// Battery section
	BatteryPropertyStringGet(0, BATTERY_VOLTAGE, pcValue);
	p = json_node_add(p, "battery_voltage", pcValue);
	BatteryPropertyStringGet(0, BATTERY_CURRENT, pcValue);
	p = json_node_add(p, "battery_current", pcValue);
	BatteryPropertyStringGet(0, BATTERY_USED_CAPACITY, pcValue);
	p = json_node_add(p, "battery_capacity_used", pcValue);
	BatteryPropertyStringGet(0, BATTERY_FULL_CAPACITY, pcValue);
	p = json_node_add(p, "battery_capacity_full", pcValue);
	BatteryPropertyStringGet(0, BATTERY_TEMPERATURE, pcValue);
	p = json_node_add(p, "battery_temperature", pcValue);
	BatteryPropertyStringGet(0, BATTERY_STATUS, pcValue);
	p = json_node_add(p, "battery_status", pcValue);
#endif


#if ( 0/*PSU_COUNT > 0*/ )
	for( int i = PSU_GROUP_2_START_INDEX, iUnit = 1; i <= PSU_GROUP_2_END_INDEX; i++ )
	{
		if( PSUPropertyGet(i, PSU_STATUS) != PSU_STATUS_UNKNOWN )
		{
			usprintf(pcParam, "psu_%d_voltage", iUnit);
			PSUPropertyStringGet(i, PSU_OUTPUT_VOLTAGE, pcValue);
			p = json_node_add(p, pcParam, pcValue);
			usprintf(pcParam, "psu_%d_current", iUnit);
			PSUPropertyStringGet(i, PSU_OUTPUT_CURRENT, pcValue);
			p = json_node_add(p, pcParam, pcValue);
			usprintf(pcParam, "psu_%d_convtemp", iUnit);
			PSUPropertyStringGet(i, PSU_TEMPERATURE, pcValue);
			p = json_node_add(p, pcParam, pcValue);
			usprintf(pcParam, "psu_%d_status", iUnit);
			PSUPropertyStringGet(i, PSU_STATUS, pcValue);
			p = json_node_add(p, pcParam, pcValue);
		}
		else
		{
			usprintf(pcParam, "psu_%d_voltage", iUnit);
			p = json_node_add(p, pcParam, "-");
			usprintf(pcParam, "psu_%d_current", iUnit);
			p = json_node_add(p, pcParam, "-");
			usprintf(pcParam, "psu_%d_convtemp", iUnit);
			p = json_node_add(p, pcParam, "-");
			usprintf(pcParam, "psu_%d_status", iUnit);
			p = json_node_add(p, pcParam, "-");
		}

		iUnit++;
	}
#endif

#if ( INVERTER_COUNT > 0 )
	for( int i = INVERTER_GROUP_1_START_INDEX, iUnit = 1; i <= INVERTER_GROUP_1_END_INDEX; i++ )
	{
		if( InverterPropertyGet(i, INVERTER_STATUS) != INVERTER_STATUS_UNKNOWN )
		{
			usprintf(pcParam, "inverter_%d_voltage", iUnit);
			InverterPropertyStringGet(i, INVERTER_OUTPUT_VOLTAGE, pcValue);
			p = json_node_add(p, pcParam, pcValue);
			usprintf(pcParam, "inverter_%d_current", iUnit);
			InverterPropertyStringGet(i, INVERTER_OUTPUT_CURRENT, pcValue);
			p = json_node_add(p, pcParam, pcValue);
			usprintf(pcParam, "inverter_%d_convtemp", iUnit);
			InverterPropertyStringGet(i, INVERTER_TEMPERATURE_CONV, pcValue);
			p = json_node_add(p, pcParam, pcValue);
			usprintf(pcParam, "inverter_%d_status", iUnit);
			InverterPropertyStringGet(i, INVERTER_STATUS, pcValue);
			p = json_node_add(p, pcParam, pcValue);
		}
		else
		{
			usprintf(pcParam, "inverter_%d_voltage", iUnit);
			p = json_node_add(p, pcParam, "-");
			usprintf(pcParam, "inverter_%d_current", iUnit);
			p = json_node_add(p, pcParam, "-");
			usprintf(pcParam, "inverter_%d_convtemp", iUnit);
			p = json_node_add(p, pcParam, "-");
			usprintf(pcParam, "inverter_%d_status", iUnit);
			p = json_node_add(p, pcParam, "-");
		}

		iUnit++;
	}
#endif

#if ( OUTPUT_DC_COUNT > 0 )
	for( int i = 0, iUnit = 1; i < OUTPUT_DC_COUNT; i++ )
	{
		usprintf(pcParam, "output_dc_%d_voltage", iUnit);
		DCOutputPropertyStringGet(i, OUTPUT_DC_VOLTAGE, pcValue);
		p = json_node_add(p, pcParam, pcValue);
		usprintf(pcParam, "output_dc_%d_current", iUnit);
		DCOutputPropertyStringGet(i, OUTPUT_DC_CURRENT, pcValue);
		p = json_node_add(p, pcParam, pcValue);
		usprintf(pcParam, "output_dc_%d_power", iUnit);
		DCOutputPropertyStringGet(i, OUTPUT_DC_POWER, pcValue);
		p = json_node_add(p, pcParam, pcValue);

		iUnit++;
	}
#endif

#if ( OUTPUT_AC_PHASE_COUNT > 0 )
	for( int i = 0, iUnit = 1; i < OUTPUT_AC_PHASE_COUNT; i++ )
	{
		usprintf(pcParam, "output_ac_%d_voltage", iUnit);
		ACOutputPhasePropertyStringGet(i, OUTPUT_AC_VOLTAGE, pcValue);
		p = json_node_add(p, pcParam, pcValue);
		usprintf(pcParam, "output_ac_%d_current", iUnit);
		ACOutputPhasePropertyStringGet(i, OUTPUT_AC_CURRENT, pcValue);
		p = json_node_add(p, pcParam, pcValue);
		usprintf(pcParam, "output_ac_%d_power", iUnit);
		ACOutputPhasePropertyStringGet(i, OUTPUT_AC_POWER, pcValue);
		p = json_node_add(p, pcParam, pcValue);

		iUnit++;
	}
#endif

	p = json_end(p);

	return p - out;
}


int
setup_cgi_handler(char *out, int len, char *uri_params)
{
	struct CGIParam sCGIParams[8];
	int iParamCount;
//	char pcValue[32];
	long lValue;
	int  iError = 0;
	char *p = out;

	iParamCount = ParseUriParameters(uri_params, sCGIParams, 8);

	//
	// Fast Voltage
	//
	if(GetFixedDecimalCGIParam("setup_voltage_fast", sCGIParams, iParamCount, &lValue))
	{
		lValue *= 10;

//		iError |= !PSUGroupPropertyIsValid(0, PSU_GROUP_SETUP_VOLTAGE_FAST, lValue);

		if(iError == 0)
		{
//			PSUGroupPropertySet(0, PSU_GROUP_SETUP_VOLTAGE_FAST, lValue);
		}
	}

	//
	// Hold Voltage
	//
	if(GetFixedDecimalCGIParam("setup_voltage_hold", sCGIParams, iParamCount, &lValue))
	{
		lValue *= 10;

//		iError |= !PSUGroupPropertyIsValid(0, PSU_GROUP_SETUP_VOLTAGE_HOLD, lValue);

		if(iError == 0)
		{
//			PSUGroupPropertySet(0, PSU_GROUP_SETUP_VOLTAGE_HOLD, lValue);
		}
	}

	//
	// Battery Current Limit
	//
	if(GetFixedDecimalCGIParam("setup_current_limit", sCGIParams, iParamCount, &lValue))
	{
//		iError |= !BatteryPropertyIsValid(0, BATTERY_CURRENT_LIMIT, lValue);

		if(iError == 0)
		{
//			BatteryPropertySet(0, BATTERY_CURRENT_LIMIT, lValue);
		}
	}

	//
	// Temperature Compensation
	//
	if(GetCGIParam("setup_tcomp", sCGIParams, iParamCount, &lValue))
	{
//		iError |= !PSUGroupPropertyIsValid(0, PSU_GROUP_TEMPERATURE_COMPENSATION, lValue);

		if(iError == 0)
		{
//			PSUGroupPropertySet(0, PSU_GROUP_TEMPERATURE_COMPENSATION, lValue);
		}
	}

	//
	// Is it commit?
	//
	if(FindCGIParameter("commit", sCGIParams, iParamCount) != -1)
	{
		p += usprintf(p, "%s", PLAIN_HEADER);

		p += usprintf(p, "%s", iError ? "rejected" : "accepted");
	}
	else
	{
		p += usprintf(p, "%s", JSON_HEADER);

		p = json_begin(p);

#if 0
		PSUGroupPropertyStringGet(0, PSU_GROUP_SETUP_VOLTAGE_FAST, pcValue);
		p = json_node_add(p, "setup_voltage_fast", pcValue);

		PSUGroupPropertyStringGet(0, PSU_GROUP_SETUP_VOLTAGE_HOLD, pcValue);
		p = json_node_add(p, "setup_voltage_hold", pcValue);

		BatteryPropertyStringGet(0, BATTERY_CURRENT_LIMIT, pcValue);
		p = json_node_add(p, "setup_current_limit", pcValue);

		PSUGroupPropertyStringGet(0, PSU_GROUP_TEMPERATURE_COMPENSATION, pcValue);
		p = json_node_add(p, "setup_tcomp", pcValue);
#endif

		p = json_end(p);
	}

	return p - out;
}


int
lan_cgi_handler(char *out, int len, char *uri_params)
{
	struct CGIParam sCGIParams[8];
	int  iParamCount;
	char *p = out;
	char  pcValue[32];
	int  iError = 0;
	unsigned long ip_addr;
	unsigned long ip_mask;
	unsigned long ip_gateway;
	unsigned long net_mode;
	unsigned char mac_addr[6];

	iParamCount = ParseUriParameters(uri_params, sCGIParams, 8);

	if(iParamCount != 0)
	{
		//
		// Net mode
		//
		int iParam = FindCGIParameter("lan_mode", sCGIParams, iParamCount);

		if(iParam != -1)
		{
			net_mode_string_set(sCGIParams[iParam].pcValue);
		}

		net_mode = net_mode_get();

		iError |= !GetIPCGIParam("lan_ip", sCGIParams, iParamCount, (long *)&ip_addr);
		iError |= !GetIPCGIParam("lan_mask", sCGIParams, iParamCount, (long *)&ip_mask);
		iError |= !GetIPCGIParam("lan_gateway", sCGIParams, iParamCount, (long *)&ip_gateway);

		if(iError == 0)
		{
			iError |= !net_property_set(ip_addr, ip_mask, ip_gateway, net_mode);
		}

		if(GetMacCGIParam("lan_mac", sCGIParams, iParamCount, mac_addr))
		{
			iError = 0;
			net_mac_address_set(mac_addr);
		}

		//
		// Make response
		//
		p += usprintf(p, "%s", PLAIN_HEADER);

		p += usprintf(p, "%s", iError ? "rejected" : "accepted");
	}
	else
	{
		//
		// Set http headers
		//
		p += usprintf(p, "%s", JSON_HEADER);

		p = json_begin(p);

		net_mode_string_get(pcValue);
		p = json_node_add(p, "lan_mode", pcValue);

		net_addr_to_string(pcValue, 24, lwiplib_local_ipaddr_get());
		p = json_node_add(p, "lan_ip", pcValue);

		net_addr_to_string(pcValue, 24, lwiplib_local_netmask_get());
		p = json_node_add(p, "lan_mask", pcValue);

		net_addr_to_string(pcValue, 24, lwiplib_local_gwaddr_get());
		p = json_node_add(p, "lan_gateway", pcValue);

		net_mac_address_string_get(pcValue);
		p = json_node_add(p, "lan_mac", pcValue);

		p = json_end(p);
	}

	return p - out;
}


int
debug_cgi_handler(char *out, int len, char *uri_params)
{
	char *p = out;

	//
	// Set http headers
	//
	p += usprintf(p, "%s", JSON_HEADER);

	p = json_begin(p);

	p = json_end(p);

	return p - out;
}


int
error_cgi_handler(char *out, int len, char *uri_params)
{
	struct CGIParam sCGIParams[1];
	int iParamCount;
	const char *pcMsg;
	long lCode;
	char *p = out;

	//
	// Set http headers
	//
	p += usprintf(p, "%s", HTML_HEADER);

	iParamCount = ParseUriParameters(uri_params, sCGIParams, 1);

	if(GetCGIParam("code", sCGIParams, iParamCount, &lCode))
	{
		switch(lCode)
		{
		case 400:
			pcMsg = "Bad Request";
			break;

		case 403:
			pcMsg = "Forbidden";
			break;

		case 404:
			pcMsg = "Not Found";
			break;

		case 500:
			pcMsg = "Internal Server Error";
			break;

		default:
			pcMsg = "Unknown Error";
			break;
		}

		p += usprintf(p, "<center>"
						 "<h1>%d %s</h1>"
						 "<hr>lwIP/%s</hr>"
						 "</center>",
						 lCode, pcMsg,
						 LWIP_VERSION_STRING);
	}

	return p - out;
}


//extern const struct fs_data rom_fsdata_root[];
/*
static const struct fs_data cgi_fsdata_root[] =
{
	{
		.next = cgi_fsdata_root + 1,
		.name = "/status.cgi",
		.func = status_cgi_handler,
		.data = cgi_out,
		.size = 2048,
		.flgs = FS_FILE_HAS_HTTP_HDR
	},

	{
		.next = cgi_fsdata_root + 2,
		.name = "/setup.cgi",
		.func = setup_cgi_handler,
		.data = cgi_out,
		.size = 2048,
		.flgs = FS_FILE_HAS_HTTP_HDR
	},

	{
		.next = cgi_fsdata_root + 3,
		.name = "/lan.cgi",
		.func = lan_cgi_handler,
		.data = cgi_out,
		.size = 2048,
		.flgs = FS_FILE_HAS_HTTP_HDR
	},

	{
		.next = cgi_fsdata_root + 4,
		.name = "/error.cgi",
		.func = error_cgi_handler,
		.data = cgi_out,
		.size = 2048,
		.flgs = FS_FILE_HAS_HTTP_HDR
	},

	{
		.next = cgi_fsdata_root + 5,
		.name = "/flash.bin",
		.func = 0,
		.data = (void *)0x08000000,
		.size = 1024 * 1024,
		.flgs = 0
	},

	{
		.next = rom_fsdata_root,
		.name = "/debug.cgi",
		.func = debug_cgi_handler,
		.data = cgi_out,
		.size = 2048,
		.flgs = FS_FILE_HAS_HTTP_HDR
	},
};
*/
//const struct fs_data *fs_root = cgi_fsdata_root;

