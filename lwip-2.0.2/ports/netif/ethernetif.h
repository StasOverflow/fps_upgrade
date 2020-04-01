#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

struct netif *ethernetif_register(void);

err_t   ethernetif_input(struct netif *netif);
int     ethernetif_poll(void);
err_t   ethernetif_init(struct netif *netif);

#endif 
