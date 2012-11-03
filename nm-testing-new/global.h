#ifndef _GLOBAL_H
#define _GLOBAL_H

/* Let's just assume that we already know anything netcfg does about the
 * connection :) */
#include "netcfg.h"


extern char *interface;
extern int wfd;

/* IP address vars */
extern struct in_addr ipaddress;
extern struct in_addr gateway;
extern struct in_addr *nameserver_array;
extern struct in_addr netmask;

/* Wireless mode */
extern wifimode_t mode;

/* Wireless config */
extern char* wepkey;
extern char* essid;

/* From wpa.c */
extern char *passphrase;  /* This is referenced in other places directly */

/* Yep, this is a wireless interface. */
extern int is_wireless_iface(char *inface);

/* TODO: those are not actually in netcfg, but could be easily added. */

/* Netcfg uses this as static, but things can change :) */
extern method_t netcfg_method;

extern response_t wifi_security;


#endif
