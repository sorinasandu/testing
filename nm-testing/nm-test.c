
/* Please remember, this is a testing repository. */

/* TODO: special care to see if WIRELESS is defined. */


/* Those will not really be necesary in netcfg, they are already there. */
#include <stdio.h>
#include <iwlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Well, this is a new one, this must be here. */
#include "uuid4.h"

/* Yep, we're testing and debuging, and this all. */
#define NM_TEST_CONFIG  1
#define NM_DEBUG        1

#if NM_TEST_CONFIG
/* Let's just assume that we already know anything netcfg does about the
 * connection :) */

typedef enum { DHCP, STATIC, DUNNO } method_t;
typedef enum { ADHOC = 1, MANAGED = 2 } wifimode_t;

char interface[] = "eth1";
int wfd;

/* IP address vars */
struct in_addr ipaddress = { 0 };
struct in_addr gateway = { 0 };
struct in_addr nameserver_array[4] = { { 0 }, };

/* Wireless mode */
wifimode_t mode = MANAGED;

/* Wireless config */
char* wepkey = NULL;
char* essid = NULL;

/* From wpa.c */
char *passphrase = NULL;  /* This is referenced in other places directly */

/* Netcfg uses this as static, but things can change :) */
method_t netcfg_method = DHCP;

#endif

/* Constants for maximum size for Network Manager config fields. */
#define NM_MAX_LEN_ID         128
#define NM_MAX_LEN_SSID       128
#define NM_MAX_LEN_MAC_ADDR   20   /* AA:BB:CC:DD:EE:FF format */
#define NM_MAX_LEN_WPA_PSK    65   /* 64 standard + NULL char */
#define NM_MAX_LEN_WEP_KEY    30   /* Rough estimation (should be 26) */
#define NM_MAX_COUNT_DNS      4


/* Some Network Manager default values for connection types. */
#define NM_DEFAULT_WIRED                "802-3-ethernet"
#define NM_DEFAULT_WIRELESS             "802-11-wireless"
#define NM_DEFAULT_WIRELESS_SECURITY    "802-11-wireless-security"


/* Minimalist structures for storing basic elements in order to write a Network
 * Manager format config file.
 *
 * See full specifications at:
 *
 * http://projects.gnome.org/NetworkManager/developers/settings-spec-08.html
 *
 */
typedef struct nm_connection
{
    char                    id[NM_MAX_LEN_ID];
    char                    uuid[UUID4_MAX_LEN];
    enum {WIRED, WIRELESS}  type;
}   nm_connection;

typedef struct nm_wireless
{
    char                        ssid[NM_MAX_LEN_SSID];
    char                        mac_addr[NM_MAX_LEN_MAC_ADDR];
    wifimode_t                  mode;
    enum {FALSE = 0, TRUE = 1}  is_secured; /* 1 = secured, 0 = unsecured */
}   nm_wireless;

typedef struct nm_wireless_security
{
    enum {WEP, WPA}         key_mgmt;

    union
    {
        char                psk[NM_MAX_LEN_WPA_PSK];
        struct
        {
            enum {HEX_ASCII = 1, PASSPHRASE = 2}  wep_key_type;
            enum {OPEN, SHARED}                   auth_alg;
            char                                  wep_key0[NM_MAX_LEN_WEP_KEY];
        };
    };
}   nm_wireless_security;

typedef struct nm_ipv4
{
    method_t            method;
    struct in_addr      ipaddress;
    struct in_addr      gateway;
    struct in_addr      nameserver_array[NM_MAX_COUNT_DNS];
}   nm_ipv4;

typedef struct nm_ipv6
{
    enum {AUTO, IGNORE} method;
}   nm_ipv6;


typedef struct nm_config_info
{
    nm_connection           connection;
    nm_wireless             wireless;
    nm_wireless_security    wireless_security;
    nm_ipv4                 ipv4;
    nm_ipv6                 ipv6;
}   nm_config_info;



/* Functions for writing Network Manager config file. */
void nm_write_config_file(struct nm_config_info nmconf)
{

}

/* Relies on global variables (y u no say? :) ) */
void nm_get_configuration(struct nm_config_info *nmconf)
{

}


#if NM_TEST_CONFIG


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

    char buf[30];

    uuid4_generate(buf);
    printf("%s\n", buf);

    wfd = iw_sockets_open();

#if NM_DEBUG
    show_wconf_info();
#endif


    return 0;
}
#endif
