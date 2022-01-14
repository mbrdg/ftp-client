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

        int sock_auth, sock_retr, auth, retr;
        URL *url_auth, *url_retr;
        FILE *f = NULL;

        url_auth = geturl(argv[1]);
        sock_auth = start(url_auth);
        assert(sock_auth > 0);

        auth = login(sock_auth, url_auth);
        assert(auth == 0);

        url_retr = passive(sock_auth, url_auth);
        assert(url_retr != NULL);

        sock_retr = start(url_retr);
        assert(sock_retr > 0);
        free(url_retr);

        retr = retrieve(sock_auth, sock_retr, url_auth, f);
        assert(retr == 0);
        free(url_auth);

        stop(sock_auth);
        stop(sock_retr);

        return 0;
}

