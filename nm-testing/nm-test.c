
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
#define NM_TEST_CONFIG 1
#define NM_DEBUG 1


/* Minimalist structure for storing basic elements in order to write a Network
 * Manager format config file. */
struct nm_config_info
{

};

void nm_write_config_file(struct nm_config_info nmconf)
{

}


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
