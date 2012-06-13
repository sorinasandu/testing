
#include <stdio.h>
#include <stdlib.h>

#include <iwlib.h>


int main()
{
    int sock;

    sock = iw_sockets_open();

    printf("%d\n", sock);

    char wl[] = "eth1";

    wireless_scan_head *context = calloc(1, sizeof(wireless_scan_head));

    printf("%d\n", iw_get_kernel_we_version());

    /* It would seem that retry must be greater than 0. */
    context->retry = 1;

    int ret = iw_process_scan(sock, wl, iw_get_kernel_we_version(), context);

    printf("%d\n", ret);

    wireless_scan *it;

    if (context->result == NULL) {
        printf("mumu\n");
    }

    /* And this works :X */
    for (it = context->result; it; it = it->next) {
        printf("%d\n", it->has_ap_addr);
        printf("%d\n", it->b.has_essid);
        printf("%s\n", it->b.essid);

        printf("\n");
    }

    return 0;
}
