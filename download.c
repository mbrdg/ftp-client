/**
* download.c
* ftp client
* Authors: Miguel Rodrigues & Nuno Castro
* RC @ L.EIC 2122
*/

#include "url.h"

int
main (int argc, char **argv)
{
        if (argc != 2) {
                fprintf(stderr, "usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
                return 1;
        }

        struct url url;
        url = parse_url(argv[1]);

        printf("user: %s\n", url.usr);
        printf("password: %s\n", url.pwd);
        printf("host: %s\n", url.host);
        printf("path: %s\n", url.path);

        return 0;
}
