
#include <stdio.h>
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
    return 1;
}

int main()
{

    return 0;
}
