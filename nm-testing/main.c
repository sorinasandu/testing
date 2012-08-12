
/* Yep, we're testing and debuging, and this all. */
#define NM_TEST_CONFIG  1
#define NM_DEBUG        1

#if NM_TEST_CONFIG

#include "global.h"
#include "nm-conf.h"

#include <stdlib.h>

/* Let's just assume that we already know anything netcfg does about the
 * connection :) */

char *interface = "eth1";
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

/* Yep, this is a wireless interface. */
int is_wireless_iface(char *inface)
{
    return 1;
}

/* Netcfg uses this as static, but things can change :) */
method_t netcfg_method = STATIC;
wifisec_t wifi_security = WEP;


void set_global_variables()
{
    wfd = iw_sockets_open();
    interface = "eth1";
    essid = "sorina";
    wifi_security = WPA;
    passphrase = "noubliezjamais";
    wepkey = "s:noubliezjamai";

    ipaddress.s_addr = 0;
    gateway.s_addr = 0;


    inet_pton(AF_INET, "255.255.255.0", &netmask.s_addr);

    nameserver_array = malloc(4 * sizeof(struct in_addr));

    inet_pton(AF_INET, "8.8.8.8", &nameserver_array[0].s_addr);
    nameserver_array[1].s_addr = 0;
    nameserver_array[2].s_addr = 0;
    nameserver_array[3].s_addr = 0;
}

#if NM_DEBUG
/* Print some random stuff, to see what we can get. */
void show_wconf_info()
{
    wireless_config wconf;

    iw_get_basic_config(wfd, interface, &wconf);

    fprintf(stderr, "DEBUG: Wireless/protocol name = %s\n", wconf.name);
    fprintf(stderr, "DEBUG: Key = %s\n", wconf.key);
    fprintf(stderr, "DEBUG: Key size = %d\n", wconf.key_size);

    fprintf(stderr, "DEBUG: ESSID = %s\n", wconf.essid);

}
#endif

int main()
{

#if TEST_UUID
    char buf[30];

    uuid4_generate(buf);
    printf("%s\n", buf);
#endif

    set_global_variables();

#if NM_DEBUG
    show_wconf_info();

    fprintf(stderr, "DEBUG: Mask = %u\n", nm_count_one_bits(netmask));
#endif

    nm_config_info nmconf;

    nm_get_configuration(&nmconf);

    nm_write_config_file(nmconf);


    return 0;
}
#endif
