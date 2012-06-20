
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iwlib.h>
#include <cdebconf/debconfclient.h>

#define MAX_LEN 1000
#define GO_BACK 30
#define ENTER_MANUALLY 10

#define empty_str(s) (s != NULL && *s == '\0')

int wfd;
char enter_manually[] = "Enter ESSID manually";
char *essid;


int netcfg_wireless_show_essids(struct debconfclient *client, char *iface,
        wireless_config *wconf, char *priority)
{
    wireless_scan_head network_list;
    char buffer[MAX_LEN] = "";

    network_list.retry = 1;

    if (iw_process_scan(wfd, iface, iw_get_kernel_we_version(),
                &network_list) >= 0 ) {
        wireless_scan *network, *old;

        /* Create list of available ESSIDs. */
        for (network = network_list.result; network; network = network->next) {
            strcat(buffer, network->b.essid);
            if (network->next) {
                strcat(buffer, ", ");
            }
        }

        /* Asking the user. */
        debconf_reset(client, "netcfg/wireless_show_essids");
        debconf_capb(client, "backup");
        debconf_subst(client, "netcfg/wireless_show_essids", "essid_list", buffer);
        debconf_input(client, priority ? priority : "high", "netcfg/wireless_show_essids");
        int ret = debconf_go(client);

        if (ret == 30) {
            return GO_BACK;
        }

        debconf_get(client, "netcfg/wireless_show_essids");

        /* Question not asked or we're succesfully associated. */
        if (!empty_str(wconf->essid) || empty_str(client->value)) {
            /* TODO Go to automatic... */

            return -1;
        }

        /* User wants to enter an ESSID manually. */
        if (strcmp(client->value, enter_manually) == 0) {
            return ENTER_MANUALLY;
        }

        /* User has chosen a network from the list, need to find which one and
         * get its cofiguration. */
        for (network = network_list.result; network; network = network->next) {
            if (strcmp(network->b.essid, client->value) == 0) {
                *wconf = network->b;
                essid = strdup(network->b.essid);
                break;
            }
        }

        /* Free the network list. */
        for (network = network_list.result; network; ) {
            old = network;
            network = network->next;
            free(old);
        }

    }

    return 0;
}

int netcfg_wireless_choose_essid_manually(struct debconfclient *client, char *iface)
{
    /* Priority here should be high, since user had already chosen to
     * enter an ESSID, he wants to see this question. */

    return 0;
}

int netcfg_wireless_set_essid(struct debconfclient *client, char *iface, char *priority)
{
    wireless_config wconf;
    int ret;

    iw_get_basic_config(wfd, iface, &wconf);


select_essid:
    ret = netcfg_wireless_show_essids(client, iface, &wconf, priority);

    if (ret == GO_BACK) {
        return GO_BACK;
    }

    if (ret == ENTER_MANUALLY) {
        int ret1 = netcfg_wireless_choose_essid_manually(client, iface);

        if (ret1 == GO_BACK) {
            goto select_essid;
        }
    }


    /* Something like:
     * - choose essid
     * - if manual, go to manual (use what is already there)
     *      - if manual = go_back, go back to choose essid
     * - set config
     */

    fprintf(stderr, "%d\n", IW_ESSID_MAX_SIZE);

    fprintf(stderr, "%s\n%d\n%d\n", wconf.essid, wconf.has_essid,
            wconf.essid_on);


    return 0;
}


int main()
{
    char iface[] = "eth1";
    struct debconfclient *client = debconfclient_new();
    debconf_x_loadtemplate(client, "set-essid.templates");

    wfd = iw_sockets_open();

    int ret = netcfg_wireless_set_essid(client, iface, "high");

    if (ret == GO_BACK) {
        /* Just go back... */
        fprintf(stderr, "GO BACK\n");
    }

    return 0;
}
