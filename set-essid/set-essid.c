
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iwlib.h>
#include <cdebconf/debconfclient.h>


#include <sys/types.h>
#include <arpa/inet.h>
#include <assert.h>


#define MAX_LEN 1000
#define GO_BACK 30
#define ENTER_MANUALLY 10

#define empty_str(s) (s != NULL && *s == '\0')

char enter_manually[] = "Enter ESSID manually";


/* Global variables which are found in netcfg. */
int wfd, skfd;
char *essid;
typedef enum { ADHOC = 1, MANAGED = 2 } wifimode_t;
wifimode_t mode = MANAGED;
int netcfg_progress_displayed = 0;


void open_sockets (void)
{
    wfd = iw_sockets_open();
    skfd = socket (AF_INET, SOCK_DGRAM, 0);
}


void interface_up (char* iface)
{
    struct ifreq ifr;

    strncpy(ifr.ifr_name, iface, IFNAMSIZ);

    if (skfd && ioctl(skfd, SIOCGIFFLAGS, &ifr) >= 0) {
        strncpy(ifr.ifr_name, iface, IFNAMSIZ);
        ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
        ioctl(skfd, SIOCSIFFLAGS, &ifr);
    }
}

void interface_down (char* iface)
{
    struct ifreq ifr;

    strncpy(ifr.ifr_name, iface, IFNAMSIZ);

    if (skfd && ioctl(skfd, SIOCGIFFLAGS, &ifr) >= 0) {
        strncpy(ifr.ifr_name, iface, IFNAMSIZ);
        ifr.ifr_flags &= ~IFF_UP;
        ioctl(skfd, SIOCSIFFLAGS, &ifr);
    }
}

int netcfg_wireless_auto_connect(struct debconfclient *client, char *iface,
        wireless_config *wconf, int *couldnt_associate)
{
    int i, success = 0;

    /* Default to any AP */
    wconf->essid[0] = '\0';
    wconf->essid_on = 0;

    iw_set_basic_config (wfd, iface, wconf);

    /* Wait for association.. (MAX_SECS seconds)*/
#ifndef MAX_SECS
#define MAX_SECS 3
#endif

    debconf_capb(client, "backup progresscancel");
    debconf_progress_start(client, 0, MAX_SECS, "netcfg/wifi_progress_title");

    if (debconf_progress_info(client, "netcfg/wifi_progress_info") == 30)
        goto stop;
    netcfg_progress_displayed = 1;

    for (i = 0; i <= MAX_SECS; i++) {
        int progress_ret;

        interface_up(iface);
        sleep (1);
        iw_get_basic_config (wfd, iface, wconf);

        if (!empty_str(wconf->essid)) {
            /* Save for later */
            debconf_set(client, "netcfg/wireless_essid", wconf->essid);
            debconf_progress_set(client, MAX_SECS);
            success = 1;
            break;
        }

        progress_ret = debconf_progress_step(client, 1);
        interface_down(iface);
        if (progress_ret == 30)
            break;
    }

stop:
    debconf_progress_stop(client);
    debconf_capb(client, "backup");
    netcfg_progress_displayed = 0;

    if (success)
        return 0;

    *couldnt_associate = 1;

    return *couldnt_associate;
}

int netcfg_wireless_show_essids(struct debconfclient *client, char *iface, char *priority)
{
    wireless_scan_head network_list;
    wireless_config wconf;
    char buffer[MAX_LEN] = "";
    int couldnt_associate = 0;

    iw_get_basic_config (wfd, iface, &wconf);
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
        if (!empty_str(wconf.essid) || empty_str(client->value)) {
            /* TODO Go to automatic... */
            if (netcfg_wireless_auto_connect(client, iface, &wconf,
                        &couldnt_associate) == 0) {
                return 0;
            }
            return couldnt_associate;
        }

        /* User wants to enter an ESSID manually. */
        if (strcmp(client->value, enter_manually) == 0) {
            return ENTER_MANUALLY;
        }

        /* User has chosen a network from the list, need to find which one and
         * get its cofiguration. */
        for (network = network_list.result; network; network = network->next) {
            if (strcmp(network->b.essid, client->value) == 0) {
                wconf = network->b;
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

    iw_set_basic_config(wfd, iface, &wconf);

    return 0;
}

int netcfg_wireless_choose_essid_manually(struct debconfclient *client, char *iface)
{
    /* Priority here should be high, since user had already chosen to
     * enter an ESSID, he wants to see this question. */

    int ret, couldnt_associate = 0;
    wireless_config wconf;
    char* tf = NULL, *user_essid = NULL, *ptr = wconf.essid;

    iw_get_basic_config (wfd, iface, &wconf);

    debconf_reset(client, "netcfg/wireless_essid");
    debconf_reset(client, "netcfg/wireless_essid_again");

    debconf_subst(client, "netcfg/wireless_essid", "iface", iface);
    debconf_subst(client, "netcfg/wireless_essid_again", "iface", iface);
    debconf_subst(client, "netcfg/wireless_adhoc_managed", "iface", iface);

    debconf_input(client, "low", "netcfg/wireless_adhoc_managed");

    if (debconf_go(client) == 30)
        return GO_BACK;

    debconf_get(client, "netcfg/wireless_adhoc_managed");

    if (!strcmp(client->value, "Ad-hoc network (Peer to peer)"))
        mode = ADHOC;

    wconf.has_mode = 1;
    wconf.mode = mode;

    debconf_input(client, "high", "netcfg/wireless_essid");

    if (debconf_go(client) == 30)
        return GO_BACK;

    debconf_get(client, "netcfg/wireless_essid");
    tf = strdup(client->value);

automatic:
    /* User doesn't care or we're successfully associated. */
    if (!empty_str(wconf.essid) || empty_str(client->value)) {
        if (netcfg_wireless_auto_connect(client, iface, &wconf,
                &couldnt_associate) == 0) {
            return 0;
        }
    }

    /* Yes, wants to set an essid by himself. */

    if (strlen(tf) <= IW_ESSID_MAX_SIZE) /* looks ok, let's use it */
        user_essid = tf;

    while (!user_essid || empty_str(user_essid) ||
           strlen(user_essid) > IW_ESSID_MAX_SIZE) {
        /* Misnomer of a check. Basically, if we went through autodetection,
         * we want to enter this loop, but we want to suppress anything that
         * relied on the checking of tf/user_essid (i.e. "", in most cases.) */
        if (!couldnt_associate) {
            debconf_subst(client, "netcfg/invalid_essid", "essid", user_essid);
            debconf_input(client, "high", "netcfg/invalid_essid");
            debconf_go(client);
        }

        if (couldnt_associate)
            ret = debconf_input(client, "critical", "netcfg/wireless_essid_again");
        else
            ret = debconf_input(client, "low", "netcfg/wireless_essid");

        /* we asked the question once, why can't we ask it again? */
        if (ret == 30)
            /* maybe netcfg/wireless_essid was preseeded; if so, give up */
            break;

        if (debconf_go(client) == 30) /* well, we did, but he wants to go back */
            return GO_BACK;

        if (couldnt_associate)
            debconf_get(client, "netcfg/wireless_essid_again");
        else
            debconf_get(client, "netcfg/wireless_essid");

        if (empty_str(client->value)) {
            if (couldnt_associate)
                /* we've already tried the empty string here, so give up */
                break;
            else
                goto automatic;
        }

        /* But now we'd not like to suppress any MORE errors */
        couldnt_associate = 0;

        free(user_essid);
        user_essid = strdup(client->value);
    }

    essid = user_essid;

    memset(ptr, 0, IW_ESSID_MAX_SIZE + 1);
    snprintf(wconf.essid, IW_ESSID_MAX_SIZE + 1, "%s", essid);
    wconf.has_essid = 1;
    wconf.essid_on = 1;

    iw_set_basic_config(wfd, iface, &wconf);

    return 0;
}

int netcfg_wireless_set_essid(struct debconfclient *client, char *iface, char *priority)
{
    wireless_config wconf;
    int choose_ret;

    iw_get_basic_config(wfd, iface, &wconf);

select_essid:
    choose_ret = netcfg_wireless_show_essids(client, iface, priority);

    if (choose_ret == GO_BACK) {
        return GO_BACK;
    }

    if (choose_ret == ENTER_MANUALLY) {
        int manually_ret = netcfg_wireless_choose_essid_manually(client, iface);

        if (manually_ret == GO_BACK) {
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

    fprintf(stderr, "%s\n", essid);


    return 0;
}


int main()
{
    char iface[] = "eth1";
    struct debconfclient *client = debconfclient_new();
    debconf_x_loadtemplate(client, "set-essid.templates");

    open_sockets();

    int ret = netcfg_wireless_set_essid(client, iface, "high");

    if (ret == GO_BACK) {
        /* Just go back... */
        fprintf(stderr, "GO BACK\n");
    }

    return 0;
}
