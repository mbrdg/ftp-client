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

        int sockfd_a, sockfd_b;
        URL *url_a, *url_b;
        FILE *f = NULL;

        url_a = parse_url(argv[1], FTP_DEFAULT_PORT);
        sockfd_a = start_connection(url_a);
        assert(sockfd_a > 0);

        int in;
        in = login(sockfd_a, url_a);
        assert(!in);

        url_b = passive(sockfd_a, url_a);
        sockfd_b = start_connection(url_b);
        assert(sockfd_b > 0);

        destroy_url(url_b);

        int retr;
        retr = retrieve(sockfd_a, sockfd_b, url_a, f);
        assert(!retr);

        destroy_url(url_a);

        end_connection(sockfd_b);
        end_connection(sockfd_a);

        return 0;
}

