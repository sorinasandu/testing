#include <cdebconf/debconfclient.h>
#include <stdio.h>
#include <string.h>

#define empty_str(s) (s != NULL && *s == '\0')

int test1(struct debconfclient *client)
{
    int rv = 0;

    debconf_capb(client, "");
    debconf_subst(client, "netcfg/write_wireless_config", "iface", "eht1");
    debconf_subst(client, "netcfg/write_wireless_config", "interfaces_file",
            "some_file");
    debconf_input(client, "medium", "netcfg/write_wireless_config");
    debconf_go(client);
    debconf_get(client, "netcfg/write_wireless_config");

    fprintf(stderr, "%s\n", client->value);

    if (!empty_str(client->value) && strcmp(client->value, "true") == 0) {
        rv = 1;
    }

    debconf_capb(client, "backup");

    return rv;
}

int test2(struct debconfclient *client)
{
    debconf_input(client, "high", "netcfg/base_system_config");
    debconf_go(client);
    debconf_get(client, "netcfg/base_system_config");

    return 0;
}


int main(int argc, char **argv)
{
    struct debconfclient *client = debconfclient_new();

    debconf_x_loadtemplate(client,
            "netcfg_templates_test.templates");

    fprintf(stderr, "%d\n", test2(client));


    debconfclient_delete(client);

    return 0;
}
