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

        int sa, sb, in, retr;
        URL *ua, *ub;
        FILE *f = NULL;

        ua = get(argv[1]);
        sa = start(ua);
        assert(sa > 0);

        in = login(sa, ua);
        assert(in == 0);

        ub = passive(sa, ua);
        assert(ub != NULL);
        sb = start(ub);
        assert(sb > 0);

        free(ub);

        retr = retrieve(sa, sb, ua, f);
        free(ua);
        assert(retr == 0);


        stop(sa);
        stop(sb);

        return 0;
}

