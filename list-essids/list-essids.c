
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iwlib.h>
#include <cdebconf/debconfclient.h>

#define MAX_LEN 1000

int show_essids()
{
    struct debconfclient *client = debconfclient_new();

    /* Selecting the interface :-" */
    char iface[] = "eth1";

    /* Some random file for testing. */
    FILE *f = fopen("test.out", "w");

    /* Le magic result. */
    char chosen_essid[MAX_LEN];

    debconf_x_loadtemplate(client, "list-essids.templates");

and_it_all_begins:

    /* Offering to scan interface. */
    debconf_reset(client, "netcfg/wireless_essid_offer_to_scan");
    debconf_capb(client, "");
    debconf_subst(client, "netcfg/wireless_essid_offer_to_scan",
                    "iface", iface);
    debconf_input(client, "high", "netcfg/wireless_essid_offer_to_scan");
    debconf_go(client);

    debconf_get(client, "netcfg/wireless_essid_offer_to_scan");

    /* Just to check: */
    fprintf(f, "%s\n", client->value);

    if (strcmp(client->value, "true") == 0) {
        int sockfd = iw_sockets_open();

        if (sockfd < 0) {
            /* Bad things happened. */
            return -1;
        }

        wireless_scan_head network_list;
        char buffer[MAX_LEN] = "";

        network_list.retry = 1;

        if (iw_process_scan(sockfd, iface, iw_get_kernel_we_version(),
                    &network_list) >= 0 ) {

            wireless_scan *network;

            for (network = network_list.result; network; network =
                    network->next) {

                /* And we have one essid. */
                fprintf(f, "%s\n", network->b.essid);

                strcat(buffer, network->b.essid);

                if (network->next) {
                    strcat(buffer, ", ");
                }
            }

            /* Print ALL the essids :) */
            fprintf(f, "%s\n", buffer);

            debconf_reset(client, "netcfg/wireless_show_essids");
            debconf_capb(client, "backup");
            debconf_subst(client, "netcfg/wireless_show_essids",
                            "essid_list", buffer);
            debconf_input(client, "high", "netcfg/wireless_show_essids");
            int ret = debconf_go(client);

            if (ret == 30) {
                goto and_it_all_begins;
            }

            debconf_get(client, "netcfg/wireless_show_essids");

            strcpy(chosen_essid, client->value);
        }
        else {
            /* Some other bad things happened. */
            return -1;
        }
    }
    else {
        /* Go to enter ESSID manually. */
    }

    fprintf(f, "And the chosen one is: %s\n", chosen_essid);

    fclose(f);

    return 0;
}

int main()
{
    int ret = show_essids();

    return ret;
}
