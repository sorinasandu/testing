
/* Please remember, this is a testing repository. */

/* TODO: special care to see if WIRELESS is defined. */

#include "nm-conf.h"

#if NM_TEST_CONFIG
#include "global.h"
#endif


/* Functions for printing informations in Network Manager format. */

void nm_write_connection(FILE *config_file, nm_connection connection)
{
    fprintf(config_file, "\n%s\n", NM_SETTINGS_CONNECTION);
    fprintf(config_file, "id=%s\n", connection.id);
    fprintf(config_file, "uuid=%s\n", connection.uuid);
    fprintf(config_file, "type=%s\n", (connection.type == WIRELESS) ?
            NM_DEFAULT_WIRELESS : NM_DEFAULT_WIRED);
}

/* Write Network Manager config file. */
void nm_write_config_file(struct nm_config_info nmconf)
{
    FILE *config_file = fopen(nmconf.connection.id, "w");

    if (config_file == NULL) {
        /* TODO: replace with di_warning */
        fprintf(stderr, "Unable to open file for writting configurations, "
                        "connection id might not be set or set to unproper "
                        "value. Current value: %s\n", nmconf.connection.id);
    }

    nm_write_connection(config_file, nmconf.connection);


}

/* Functions for extracting information from netcfg variables. */

/* Get info for the connection setting. */
void nm_get_wireless_connection(nm_connection *connection)
{
    snprintf(connection->id, NM_MAX_LEN_ID, "Auto %s", essid);
    uuid4_generate(connection->uuid);
    connection->type = WIRELESS;
}

/* Extract all configs for a wireless interface, from both global netcfg
 * values and other resources. */
void nm_get_wireless_config(struct nm_config_info *nmconf)
{
    nm_get_wireless_connection(&(nmconf->connection));

}

/* Relies on global variables (y u no say? :) ) */
void nm_get_configuration(struct nm_config_info *nmconf)
{
    /* TODO: test if wireless interface */
    nm_get_wireless_config(nmconf);

}
