#ifndef _GLOBAL_H
#define _GLOBAL_H

/* Let's just assume that we already know anything netcfg does about the
 * connection :) */

typedef enum { DHCP, STATIC, DUNNO } method_t;
typedef enum { ADHOC = 1, MANAGED = 2 } wifimode_t;

extern char *interface;
extern int wfd;

/* IP address vars */
extern struct in_addr ipaddress;
extern struct in_addr gateway;
extern struct in_addr *nameserver_array;

/* Wireless mode */
extern wifimode_t mode;

/* Wireless config */
extern char* wepkey;
extern char* essid;

/* From wpa.c */
extern char *passphrase;  /* This is referenced in other places directly */

/* Netcfg uses this as static, but things can change :) */
extern method_t netcfg_method;

/* Yep, this is a wireless interface. */
extern int is_wireless_iface(char *inface);

#endif
