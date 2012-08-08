
#ifndef _NM_CONF_H
#define _NM_CONF_H

/* Testing: */
#define NM_TEST_CONFIG 1

/* Those will not really be necesary in netcfg, they are already there. */
#include <stdio.h>
#include <iwlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Well, this is a new one, this must be here. */
#include "uuid4.h"

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

#define NM_SETTINGS_CONNECTION          "[connection]"
#define NM_SETTINGS_WIRELESS            "["NM_DEFAULT_WIRELESS"]"
#define NM_SETTINGS_WIRED               "["NM_DEFAULT_WIRED"]"
#define NM_SETTINGS_WIRELESS_SECURITY   "["NM_DEFAULT_WIRELESS_SECURITY"]"
#define NM_SETTINGS_IPV4                "[ipv4]"
#define NM_SETTINGS_IPv6                "[ipv6]"


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
    char                            ssid[NM_MAX_LEN_SSID];
    char                            mac_addr[NM_MAX_LEN_MAC_ADDR];
    enum {AD_HOC, INFRASTRUCTURE}   mode;
    enum {FALSE = 0, TRUE = 1}      is_secured; /* 1 = secure, 0 = unsecure */
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
    enum {AUTO, MANUAL}     method;
    struct in_addr          ipaddress;
    struct in_addr          gateway;
    struct in_addr          nameserver_array[NM_MAX_COUNT_DNS];
}   nm_ipv4;

typedef struct nm_ipv6
{
    enum {AUTO6, IGNORE} method;
}   nm_ipv6;


typedef struct nm_config_info
{
    nm_connection           connection;
    nm_wireless             wireless;
    nm_wireless_security    wireless_security;
    nm_ipv4                 ipv4;
    nm_ipv6                 ipv6;
}   nm_config_info;

/* Here come functions: */

void nm_write_connection(FILE *config_file, nm_connection connection);
void nm_write_config_file(struct nm_config_info nmconf);

void nm_get_wireless_connection(nm_connection *connection);
void nm_get_wireless_config(struct nm_config_info *nmconf);
void nm_get_configuration(struct nm_config_info *nmconf);

#endif

