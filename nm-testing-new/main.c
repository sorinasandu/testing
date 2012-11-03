
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "nm-conf.h"


int wfd;


/* Yep, this is a wireless interface. */
/* LE: or maybe sometimes it isn't :D */
int is_wireless_iface(char *inface)
{
    return 0;
}

void init_globals()
{
    wfd = iw_sockets_open();
}

void free_globals()
{
}

void init_netcfg_interface(struct netcfg_interface *ni)
{
    int i;

    ni->name = "eth0";

    strcpy(ni->ipaddress, "192.168.2.2");
    strcpy(ni->gateway, "");
    ni->dhcp = 1;
    ni->address_family = AF_INET;

    for (i = 0; i < NETCFG_NAMESERVERS_MAX; i ++) {
        strcpy(ni->nameservers[i], "");
    }
    strcpy(ni->nameservers[0], "8.8.8.8");

    ni->wifi_security = REPLY_WEP;
    ni->passphrase = NULL;
    ni->wepkey = "s:somerandomkey";
    ni->essid = "essid";
    ni->mode = ADHOC;

    ni->masklen = 24;
}

int main()
{
    nm_config_info nmconf;
    struct netcfg_interface ni;

    init_globals();
    init_netcfg_interface(&ni);

    nm_get_configuration(&ni, &nmconf);
    nm_write_configuration(nmconf);

    free_globals();

    return 0;
}
