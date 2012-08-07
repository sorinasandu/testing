/*
 * Generate a UUID v4 (random).
 *
 * Copyright © 2012 Per Andersson <avtobiff@gmail.com>
 * This is GPLv3 (or later at your option) licensed code.
 *
 * Compile and run me
 *
 *     gcc -Wall -o uuid4 uuid4.c
 *     ./uuid4
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define DEBUG 0


char* uuid4_generate(char *uuid)
{
    pid_t               pid;
    struct timeval      tp;
    struct timezone     tzp;
    unsigned            rnd0, rnd1, rnd2, rnd3;
    unsigned long long  uuid_hi, uuid_lo;


    /* seed rng with time xor pid */
    pid = getpid();
    gettimeofday(&tp, &tzp);
    srandom(tp.tv_sec ^ pid);

    /* generate random numbers (note they are only 31 bits) */
    rnd0 = random();
    rnd1 = random();
    rnd2 = random();
    rnd3 = random();

    /* compile uuid v4 parts */
    /* rnd0 and rnd1 are 31 bits, compensate for that */
    uuid_hi = rnd0;
    uuid_hi = (uuid_hi << 34)
            + ((rnd1 & 0xffff8000) << 2)
            + 0x00004000                    /* version = 4 */
            + ((rnd1 & 0x00007ff) >> 2);

    /* rnd2 and rnd3 are 31 bits, compensate for that */
    uuid_lo = 0x80000000                    /* variant 1 0 */
            + (rnd2 >> 2);
    uuid_lo = (uuid_lo << 32)
            + rnd3;

#if DEBUG
    printf("DEBUG: pid = %d\n", pid);
    printf("DEBUG: tp.tv_sec = %li\nDEBUG: tp.tv_usec = %li\n", tp.tv_sec,
                                                               tp.tv_usec);
    printf("DEBUG: tp.tv_sec ^ pid = %li\n", tp.tv_sec ^ pid);
    printf("DEBUG: RAND_MAX = %d\n", RAND_MAX);
    printf("DEBUG: uuidX = %d %d %d %d\n", rnd0, rnd1, rnd2, rnd3);
    printf("uuid_hi = %016llx\nuuid_lo = %016llx\n", uuid_hi, uuid_lo);
#endif

    /* format the resulting uuid */
    sprintf(uuid, "%08llx-%04llx-%04llx-%04llx-%012llx",
                (uuid_hi & 0xffffffff00000000) >> 32, /*  8 nibbles */
                (uuid_hi & 0x00000000ffff0000) >> 16, /*  4 nibbles */
                uuid_hi & 0x000000000000ffff,         /*  4 nibbles */
                (uuid_lo & 0xffff000000000000) >> 48, /*  4 nibbles */
                uuid_lo & 0x0000ffffffffffff          /* 12 nibbles */
                );

    return uuid;
}


/* example usage */
int main(int argc, char **argv)
{
    char uuid[25];

    uuid4_generate(uuid);
    printf("%s\n", uuid);

    return 0;
}
