/**
* download.c
* ftp client
* Authors: Miguel Rodrigues & Nuno Castro
* RC @ L.EIC 2122
*/

#include <sys/socket.h>
#include <sys/types.h>

#include <stdio.h>
#include <unistd.h>

#include "connection.h"


int
main (int argc, char **argv)
{
        if (argc != 2) {
                fprintf(stderr, "usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
                return 1;
        }

        URL url;
        url = get_url(argv[1]);

        int sockfd[2];
        sockfd[0] = start(&url);
        get_response(sockfd[0]);

        command(sockfd[0], USER, url.user);
        get_response(sockfd[0]);

        command(sockfd[0], PASS, url.pass);
        get_response(sockfd[0]);

        command(sockfd[0], PASV, "");
        get_response(sockfd[0]);

        stop(sockfd[0]);

        return 0;
}
