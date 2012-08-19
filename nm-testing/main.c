
/* Yep, we're testing and debuging, and this all. */
#define NM_TEST_CONFIG  1
#define NM_DEBUG        0

#if NM_TEST_CONFIG

#include "global.h"
#include "nm-conf.h"

#include <stdlib.h>

/* Let's just assume that we already know anything netcfg does about the
 * connection :) */

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

/* Yep, this is a wireless interface. */
/* LE: or maybe sometimes it isn't :D */
int is_wireless_iface(char *inface)
{
    return 1;
}

/* Netcfg uses this as static, but things can change :) */
method_t netcfg_method = STATIC;
response_t wifi_security;

/* Checks if an address is a valid netmask, "^1*0*$", returns 1 for a valid
 * netmask, 0 otherwise. */
int netcfg_check_netmask(struct in_addr netmask)
{
    unsigned char mask, byte;
    int flag = 0;
    int byte_count, bit_count;

    for (byte_count = 0; byte_count < 4; byte_count++) {
        /* Get each byte of the netmask, network byte order. */
        byte = *(((unsigned char *) &(netmask.s_addr)) + byte_count);

        for (bit_count = 7; bit_count >= 0; bit_count--) {
            mask = 1 << bit_count;

            /* 1 bit. */
            if (mask & byte) {
                /* There's an 1 bit where there should be only 0s. */
                if (flag == 1) {
                    return 0;
                }
            }
            /* 0 bit. */
            else {
                /* The initial sequence of 1 bits is over. */
                flag = 1;
            }
        }
    }

    return 1;
}


void set_global_variables()
{
    wfd = iw_sockets_open();
    interface = "eth1";
    netcfg_method = STATIC;

    essid = "sorina";
    wifi_security = REPLY_WPA;
    passphrase = "noubliezjamais";
    wepkey = "s:noubliezjamai";

    inet_pton(AF_INET, "192.168.10.123", &ipaddress.s_addr);
    inet_pton(AF_INET, "192.168.10.1", &gateway.s_addr);
    inet_pton(AF_INET, "255.254.0.0", &netmask.s_addr);

    nameserver_array = malloc(4 * sizeof(struct in_addr));

    inet_pton(AF_INET, "8.8.8.8", &nameserver_array[0].s_addr);
    inet_pton(AF_INET, "4.4.4.4", &nameserver_array[1].s_addr);
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

    printf("Netmask result: %d\n", netcfg_check_netmask(netmask));

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
