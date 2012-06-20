
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iwlib.h>
#include <cdebconf/debconfclient.h>

#define MAX_LEN 1000
#define GO_BACK 30

int wfd;
char enter_manually[] = "Enter ESSID manually";


int netcfg_wireless_show_essids(struct debconfclient *client, char *iface, char *essid)
{
    wireless_scan_head network_list;
    char buffer[MAX_LEN] = "";

    network_list.retry = 1;

    if (iw_process_scan(wfd, iface, iw_get_kernel_we_version(),
                &network_list) >= 0 ) {
        wireless_scan *network, *old;

        for (network = network_list.result; network; ) {
            strcat(buffer, network->b.essid);

            if (network->next) {
                strcat(buffer, ", ");
            }

            old = network;
            network = network->next;

            free(old);
        }

        debconf_reset(client, "netcfg/wireless_show_essids");
        debconf_capb(client, "backup");
        debconf_subst(client, "netcfg/wireless_show_essids",
                        "essid_list", buffer);
        debconf_input(client, "high", "netcfg/wireless_show_essids");
        int ret = debconf_go(client);

        if (ret == 30) {
            return GO_BACK;
        }
        debconf_get(client, "netcfg/wireless_show_essids");

        snprintf(essid, IW_ESSID_MAX_SIZE + 1, "%s", essid);
    }

    return 0;
}

int netcfg_wireless_choose_essid_manually(struct debconfclient *client, char *iface, char *priority)
{

    return 0;
}

int netcfg_wireless_set_essid(struct debconfclient *client, char *iface, char *priority)
{
    wireless_config wconf;
    char *ptr = wconf.essid;
    char essid[IW_ESSID_MAX_SIZE];

    iw_get_basic_config(wfd, iface, &wconf);


    /* Something like:
     * - choose essid
     * - if manual, go to manual (use what is already there)
     *      - if manual = go_back, go back to choose essid
     * - set config
     */

    fprintf(stderr, "%s\n%d\n%d\n", wconf.essid, wconf.has_essid,
            wconf.essid_on);


    return 0;
}


int main()
{
    char iface[] = "eth1", essid[MAX_LEN];
    struct debconfclient *client = debconfclient_new();
    debconf_x_loadtemplate(client, "set-essid.templates");

    wfd = iw_sockets_open();

    netcfg_wireless_set_essid(client, iface, "high");

    int ret = netcfg_wireless_show_essids(client, iface, essid);

    if (ret == GO_BACK) {
        /* Just go back... */
    }

    return 0;
}
