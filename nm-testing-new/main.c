
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "nm-conf.h"


char *interface;
int wfd;

/* IP address vars */
struct in_addr ipaddress;
struct in_addr gateway;
struct in_addr *nameserver_array;
struct in_addr netmask;

/* Wireless mode */
wifimode_t mode = MANAGED;

/* Wireless config */
char* wepkey = NULL;
char* essid = NULL;

/* From wpa.c */
char *passphrase = NULL;  /* This is referenced in other places directly */

/* Netcfg uses this as static, but things can change :) */
method_t netcfg_method = STATIC;
response_t wifi_security;



/* Yep, this is a wireless interface. */
/* LE: or maybe sometimes it isn't :D */
int is_wireless_iface(char *inface)
{
    return 0;
}

void init_globals()
{
    wfd = iw_sockets_open();

    interface = "eth0";
    netcfg_method = STATIC;

    essid = "sorina";
    wifi_security = REPLY_WPA;
    passphrase = "noubliezjamais";
    wepkey = "s:noubliezjamai";

    inet_pton(AF_INET, "192.168.10.123", &ipaddress.s_addr);
    inet_pton(AF_INET, "192.168.10.1", &gateway.s_addr);
    inet_pton(AF_INET, "255.255.255.255", &netmask.s_addr);

    nameserver_array = malloc(4 * sizeof(struct in_addr));

    inet_pton(AF_INET, "8.8.8.8", &nameserver_array[0].s_addr);
    inet_pton(AF_INET, "4.4.4.4", &nameserver_array[1].s_addr);
    nameserver_array[2].s_addr = 0;
    nameserver_array[3].s_addr = 0;

}

void free_globals()
{
    free(nameserver_array);
}

void init_netcfg_interface(struct netcfg_interface *ni)
{
    int i;

    ni->name = "eth0";

    strcpy(ni->ipaddress, "192.168.2.2");
    strcpy(ni->gateway, "");
    ni->dhcp = 0;
    ni->address_family = AF_INET;

    for (i = 0; i < NETCFG_NAMESERVERS_MAX; i ++) {
        strcpy(ni->nameservers[i], "");
    }
    strcpy(ni->nameservers[0], "8.8.8.8");

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
