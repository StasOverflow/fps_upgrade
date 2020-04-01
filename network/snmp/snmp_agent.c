
#include <lib/types.h>

#include "lwiplib.h"
#include "lwip/apps/snmp_mib2.h"
#include "snmp_pulse.h"


void
snmp_agent_init( void )
{
	static const struct snmp_obj_id enterprise_oid =
	{
		SNMP_DEVICE_ENTERPRISE_OID_LEN,
		{1, 3, 6, 1, 4, 1, 2}
	};

	static const struct snmp_mib* mibs[] =
	{
		&mib2,
		&pulse
	};

//	snmp_mib2_set_syscontact();
//	snmp_mib2_set_syslocation();
//	snmp_mib2_set_sysdescr();
//	snmp_mib2_set_sysname();
	snmp_set_mibs(mibs, LWIP_ARRAYSIZE(mibs));
	snmp_set_device_enterprise_oid(&enterprise_oid);
	snmp_init();
}
