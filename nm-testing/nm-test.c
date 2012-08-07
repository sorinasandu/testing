
#include <stdio.h>
#include <iwlib.h>
#include "uuid4.h"


#define TEST_NM_CONFIG 1

/* Minimalist structure for storing basic elements in order to write a Network
 * Manager format config file. */
struct nm_config_info
{

};

void nm_write_config_file(struct nm_config_info nmconf)
{

}


#if TEST_NM_CONFIG

char iface[] = "eth1";
int wfd;


void show_wconf_info()
{
    wireless_config wconf;

    iw_get_basic_config(wfd, iface, &wconf);

    fprintf(stderr, "DEBUG: Wireless/protocol name = %s\n", wconf.name);
    fprintf(stderr, "DEBUG: Key = %s\n", wconf.key);
    fprintf(stderr, "DEBUG: Key size = %d\n", wconf.key_size);

    fprintf(stderr, "DEBUG: ESSID = %s\n", wconf.essid);

}

int main()
{

    char buf[30];

    uuid4_generate(buf);
    printf("%s\n", buf);

    wfd = iw_sockets_open();
    show_wconf_info();


    return 0;
}
#endif
