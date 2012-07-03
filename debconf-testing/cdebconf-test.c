#include <cdebconf/debconfclient.h>
#include <stdio.h>

#define empty_str(s) (s != NULL && *s == '\0')

void test1(struct debconfclient *client)
{
    /* Showing the question the first time. */
    debconf_reset(client, "foo/bar");  /* ask every time */
    debconf_input(client, "low", "foo/bar");
    debconf_go(client);

    debconf_get(client, "foo/bar");

    /* Something with multiple choise. */
    debconf_reset(client, "foo/choice");
    debconf_subst(client, "foo/choice", "choices", "Whatever, Something more");
    debconf_input(client, "high", "foo/choice");
    debconf_go(client);

    debconf_get(client, "foo/choice");

and_again:
    /* Setting the value for foo/bar. */
    debconf_set(client, "foo/bar", "stuff");
    debconf_input(client, "low", "foo/bar");
    debconf_go(client);

    debconf_get(client, "foo/bar");

    /* Asking a boolean question. */
    debconf_reset(client, "foo/my_question");
    debconf_input(client, "high", "foo/my_question");
    debconf_go(client);

    debconf_get(client, "foo/my_question");

    /* Playing with Go Back button. */
    debconf_capb(client, "backup");

    debconf_reset(client, "foo/bar");  /* ask every time */
    debconf_input(client, "high", "foo/bar");

    int ret = debconf_go(client);

    if (ret == 30) {
        goto and_again;
    }

    debconf_get(client, "foo/bar");

}

void test2(struct debconfclient *client)
{

}

int main(int argc, char **argv)
{
    struct debconfclient *client = debconfclient_new();

    debconf_x_loadtemplate(client, "cdebconf-test.templates");

    test1(client);

    debconfclient_delete(client);

    return 0;
}
