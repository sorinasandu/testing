
// TODO: remove "//" comments

#include "nm-conf.h"


/* Functions for printing informations in Network Manager format. */

void nm_write_connection(FILE *config_file, nm_connection connection)
{
    fprintf(config_file, "\n%s\n", NM_SETTINGS_CONNECTION);
    fprintf(config_file, "id=%s\n", connection.id);
    fprintf(config_file, "uuid=%s\n", connection.uuid);
    fprintf(config_file, "type=%s\n", (connection.type == WIFI) ?
            NM_DEFAULT_WIRELESS : NM_DEFAULT_WIRED);
}

#ifdef WIRELESS
void nm_write_wireless_specific_options(FILE *config_file,
        nm_wireless wireless)
{
    fprintf(config_file, "\n%s\n", NM_SETTINGS_WIRELESS);
    fprintf(config_file, "ssid=%s\n", wireless.ssid);
    fprintf(config_file, "mode=%s\n", (wireless.mode == AD_HOC) ?
            "adhoc" : "infrastructure");

    if (strcmp(wireless.mac_addr, "")) {
        fprintf(config_file, "mac=%s\n", wireless.mac_addr);
    }
    if (wireless.is_secured == TRUE) {
        fprintf(config_file, "security=%s\n", NM_DEFAULT_WIRELESS_SECURITY);
    }
}
#endif

void nm_write_wired_specific_options(FILE *config_file, nm_wired wired)
{
    fprintf(config_file, "\n%s\n", NM_SETTINGS_WIRED);

    if (strcmp(wired.mac_addr, "")) {
        fprintf(config_file, "mac=%s\n", wired.mac_addr);
    }
}

#ifdef WIRELESS
void nm_write_wireless_security(FILE *config_file, nm_wireless_security
        wireless_security)
{
    fprintf(config_file, "\n%s\n", NM_SETTINGS_WIRELESS_SECURITY);

    if (wireless_security.key_mgmt == WPA_PSK) {
        fprintf(config_file, "key-mgmt=%s\n", "wpa-psk");
        fprintf(config_file, "psk=%s\n", wireless_security.psk);
    }
    else {
        fprintf(config_file, "key-mgmt=%s\n", "none");
        fprintf(config_file, "auth-alg=%s\n",
                (wireless_security.auth_alg == OPEN) ? "open" : "shared");
        fprintf(config_file, "wep-key0=%s\n", wireless_security.wep_key0);
        fprintf(config_file, "wep-key-type=%d\n",
                wireless_security.wep_key_type);
    }
}
#endif

void nm_write_ipv4(FILE *config_file, nm_ipv4 ipv4)
{
    /* Don't write ipv4 settings if ipv4 wasn't used. */
    if (ipv4.used == 0) {
        return;
    }

    fprintf(config_file, "\n%s\n", NM_SETTINGS_IPV4);

    if (ipv4.method == AUTO) {
        fprintf(config_file, "method=%s\n", "auto");
    }
    else {
        char    buffer[NM_MAX_LEN_BUF], addr[NM_MAX_LEN_IPV4];
        int     i;

        fprintf(config_file, "method=%s\n", "manual");

        /* Get DNS in printable format. */
        memset(buffer, 0, NM_MAX_LEN_BUF);

        for (i = 0; !empty_str(ipv4.nameservers[i]); i++) {
            strcat(buffer, ipv4.nameservers[i]);
            strcat(buffer, ";");
        }

        if (strcmp(buffer, "")) {
            fprintf(config_file, "dns=%s\n", buffer);
        }

        /* Get addresses in printable format. */
        memset(buffer, 0, NM_MAX_LEN_BUF);

        /* Write IP address to the buffer. */
        strcat(buffer, ipv4.ip_address);
        strcat(buffer, ";");

        /* Write netmask to the buffer. */
        sprintf(addr, "%d", ipv4.masklen);
        strcat(buffer, addr);
        strcat(buffer, ";");

        /* Write gateway address to the buffer. */
        memset(addr, 0, NM_MAX_LEN_IPV4);
        if (!empty_str(ipv4.gateway)) {
            strncpy(addr, ipv4.gateway, NM_MAX_LEN_IPV4 - 1);
        }
        else {
            strcpy(addr, "0");
        }
        strcat(buffer, addr);
        strcat(buffer, ";");

        /* Write config to the configuration file. */
        fprintf(config_file, "addresses1=%s\n", buffer);
    }
}

void nm_write_ipv6(FILE *config_file, nm_ipv6 ipv6)
{
    if (ipv6.used == 0) {
        return
    }

    fprintf(config_file, "\n%s\n", NM_SETTINGS_IPV6);

    if (ipv6.method == IGNORE) {
        fprintf(config_file, "method=%s\n", "ignore");
    }
}

/* Write Network Manager config file. */
void nm_write_configuration(struct nm_config_info nmconf)
{
    FILE    *config_file;
    char    buffer[NM_MAX_LEN_BUF];

    /* Create the directory for the config file and clear any possible
     * previous files found there. */
    sprintf(buffer, "mkdir -p %s", NM_CONFIG_FILE_PATH);
//    di_exec_shell(buffer);

    /* If the directory exist mkdir will do nothing, so just remove every file
     * there. Rely on the fact that for now netcfg only does config for one
     * interface. */
    sprintf(buffer, "rm %s/*", NM_CONFIG_FILE_PATH);
//    di_exec_shell(buffer);

    /* Open file using its full path. */
    sprintf(buffer, "%s/%s", NM_CONFIG_FILE_PATH, nmconf.connection.id);
//    config_file = fopen(buffer, "w");
    config_file = fopen(nmconf.connection.id, "w");

    if (config_file == NULL) {
//        di_info("Unable to open file for writting configurations, "
//                "connection id might not be set or set to unproper "
//                "value. Current value: %s\n", nmconf.connection.id);
        printf("Unable to open file\n");
        return;
    }

    nm_write_connection(config_file, nmconf.connection);

    if (nmconf.connection.type == WIRED) {
        nm_write_wired_specific_options(config_file, nmconf.wired);
    }
#ifdef WIRELESS
    else {
        nm_write_wireless_specific_options(config_file, nmconf.wireless);
        if (nmconf.wireless.is_secured) {
            nm_write_wireless_security(config_file, nmconf.wireless_security);
        }
    }
#endif

    nm_write_ipv4(config_file, nmconf.ipv4);
    nm_write_ipv6(config_file, nmconf.ipv6);

    fclose(config_file);
}

/* Functions for extracting information from netcfg variables. */

/* Get info for the connection setting for wireless networks. */

#ifdef WIRELESS
void nm_get_wireless_connection(struct netcfg_interface *niface, nm_connection *connection)
{
    uuid_t uuid;

    /* Use the wireless network name for connection id. */
    snprintf(connection->id, NM_MAX_LEN_ID, "Auto %s", niface->essid);

    /* Generate uuid. */
    uuid_generate(uuid);
    uuid_unparse(uuid, connection->uuid);

    connection->type = WIFI;
}
#endif

/* Get info for the connection setting for wired networks. */
void nm_get_wired_connection(nm_connection *connection)
{
    uuid_t uuid;

    /* This is the first wired connection. */
    snprintf(connection->id, NM_MAX_LEN_ID, NM_DEFAULT_WIRED_NAME);

    /* Generate uuid. */
    uuid_generate(uuid);
    uuid_unparse(uuid, connection->uuid);

    connection->type = WIRED;
}

/* Get MAC address from default file. */
void nm_get_mac_address(char *interface, char *mac_addr)
{
    char    file_name[NM_MAX_LEN_PATH];
    FILE    *file;

    snprintf(file_name, NM_MAX_LEN_PATH, NM_DEFAULT_PATH_FOR_MAC, interface);
    file = fopen(file_name, "r");

    if (file == NULL) {
        mac_addr[0] = '\0';   /* Empty string means don't write MAC. */
    }
    else {
        int i;

        fscanf(file, "%s\n", mac_addr);

        /* Should be upper case. */
        for (i = 0; mac_addr[i]; i++) {
            mac_addr[i] = toupper(mac_addr[i]);
        }
    }
}

#ifdef WIRELESS
void nm_get_wireless_specific_options(struct netcfg_interface *niface, nm_wireless *wireless)
{
    strncpy(wireless->ssid, niface->essid, NM_MAX_LEN_SSID);

    nm_get_mac_address(niface->name, wireless->mac_addr);

    /* Decide mode. */
    if (niface->mode == ADHOC) {
        wireless->mode = AD_HOC;
    }
    else {
        wireless->mode = INFRASTRUCTURE;
    }

    /* In netcfg, you have to chose WEP and leave the key empty for an
     * unsecure connection. */
    if (niface->wifi_security == REPLY_WEP && niface->wepkey == NULL) {
        wireless->is_secured = FALSE;
    }
    else {
        wireless->is_secured = TRUE;
    }
}
#endif

/* Only set MAC address, the others have good defaults in NM. */
void nm_get_wired_specific_options(struct netcfg_interface *niface, nm_wired *wired)
{
    nm_get_mac_address(niface->name, wired->mac_addr);
}

/* Security type for wireless networks. */
#ifdef WIRELESS
void nm_get_wireless_security(struct netcfg_interface *niface, nm_wireless_security *wireless_security)
{
    if (niface->wifi_security == REPLY_WPA) {
        wireless_security->key_mgmt = WPA_PSK;
        memset(wireless_security->psk, 0, NM_MAX_LEN_WPA_PSK);
        strncpy(wireless_security->psk, niface->passphrase, NM_MAX_LEN_WPA_PSK - 1);
    }
    else {
        wireless_security->key_mgmt = WEP_KEY;
        memset(wireless_security->wep_key0, 0, NM_MAX_LEN_WEP_KEY);
        iw_in_key(niface->wepkey, wireless_security->wep_key0);

        /* Only options supported by netcfg for now. */
        wireless_security->wep_key_type = HEX_ASCII;
        wireless_security->auth_alg = OPEN;
    }
}
#endif

/* Save IPv4 settings. */
void nm_get_ipv4(struct netcfg_interface *niface, nm_ipv4 *ipv4)
{
    if (niface->address_family != AF_INET) {
        ipv4->used = 0;
        return;
    }

    ipv4->used = 1;
    if (niface->dhcp == 1) {
        ipv4->method = AUTO;
    }
    else {
        int i;

        ipv4->method = MANUAL;
        ipv4->ip_address = niface->ipaddress;
        ipv4->gateway = niface->gateway;
        ipv4->masklen = niface->masklen;

        for (i = 0; i < NM_MAX_COUNT_DNS; i++) {
            ipv4->nameservers[i] = niface->nameservers[i];
        }
    }
}

/* For the moment, just set it to ignore. */
void nm_get_ipv6(struct netcfg_interface *niface, nm_ipv6 *ipv6)
{
    if (niface->address_family != AF_INET6) {
        ipv6->used = 0;
        return;
    }

    ipv6->used = 1;
    ipv6->method = IGNORE;
}

/* Extract all configs for a wireless interface, from both global netcfg
 * values and other resources. */
#ifdef WIRELESS
void nm_get_wireless_config(struct netcfg_interface *niface, struct nm_config_info *nmconf)
{
    nm_get_wireless_connection(niface, &(nmconf->connection));
    nm_get_wireless_specific_options(niface, &(nmconf->wireless));

    if (nmconf->wireless.is_secured == TRUE) {
        nm_get_wireless_security(niface, &(nmconf->wireless_security));
    }

    nm_get_ipv4(niface, &(nmconf->ipv4));
    nm_get_ipv6(niface, &(nmconf->ipv6));
}
#endif

/* Extract all configs for a wired interface. */
void nm_get_wired_config(struct netcfg_interface *niface, struct nm_config_info *nmconf)
{
    nm_get_wired_connection(&(nmconf->connection));
    nm_get_wired_specific_options(niface, &(nmconf->wired));
    nm_get_ipv4(niface, &(nmconf->ipv4));
    nm_get_ipv6(niface, &(nmconf->ipv6));
}

/* Getting configurations for NM relies on netcfrg global variables. */
void nm_get_configuration(struct netcfg_interface *niface, struct nm_config_info *nmconf)
{
    /* Decide if wireless configuration is needed. */
    if (!is_wireless_iface(niface->name)) {
        nm_get_wired_config(niface, nmconf);
    }
#ifdef WIRELESS
    else {
        nm_get_wireless_config(niface, nmconf);
    }
#endif
}
