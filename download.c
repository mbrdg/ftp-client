/**
* download.c
* ftp client
* Authors: Miguel Rodrigues & Nuno Castro
* RC @ L.EIC 2122
*/

#include <sys/socket.h>
#include <sys/types.h>

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "connection.h"

/* globals */
static const char
usage[] = "usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n";

int
main (int argc, char **argv)
{
        if (argc != 2) {
                fprintf(stderr, usage, argv[0]);
                return 1;
        }

        URL *init, *retr;
        int initfd, retrfd;

        init = parse_url(argv[1], FTP_DEFAULT_PORT);
        initfd = start_connection(init);

        int in;
        in = login(initfd, init);
        assert(in == 0);

        retr = passive(initfd, init);
        retrfd = start_connection(retr);

        destroy_url(retr);
        end_connection(retrfd);

        destroy_url(init);
        end_connection(initfd);

        return 0;
}

