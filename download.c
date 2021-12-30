/**
* download.c
* ftp client
* Authors: Miguel Rodrigues & Nuno Castro
* RC @ L.EIC 2122
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "connection.h"


int
main (int argc, char **argv)
{
        if (argc != 2) {
                fprintf(stderr, "usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
                return 1;
        }

        int sock_a, sock_b, in, retr;
        URL *url_a, *url_b;
        FILE *f = NULL;

        url_a = get(argv[1]);
        sock_a = start(url_a);
        assert(sock_a > 0);

        in = login(sock_a, url_a);
        assert(in == 0);

        url_b = passive(sock_a, url_a);
        assert(ub != NULL);

        sock_b = start(url_b);
        assert(sock_b > 0);
        free(url_b);

        retr = retrieve(sock_a, sock_b, url_a, f);
        assert(retr == 0);
        free(url_a);

        stop(sock_a);
        stop(sock_b);

        return 0;
}

