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

        URL *url;
        url = parse_url(argv[1]);

        int init_fd;
        init_fd = start_connection(url);

        int in;
        in = login(init_fd, url);
        assert(in == 0);

        destroy_url(url);
        end_connection(init_fd);

        return 0;
}

