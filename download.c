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

        int socka, sockb, auth, retr;
        URL *urla, *urlb;
        FILE *f = NULL;

        urla = get(argv[1]);
        socka = start(urla);
        assert(socka > 0);

        auth = login(socka, urla);
        assert(auth == 0);

        urlb = passive(socka, urla);
        assert(urlb != NULL);

        sockb = start(urlb);
        assert(sockb > 0);
        free(urlb);

        retr = retrieve(socka, sockb, urla, f);
        assert(retr == 0);
        free(urla);

        stop(socka);
        stop(sockb);

        return 0;
}

