/**
 * url.h
 * Authors: Miguel Rodrigues & Nuno Castro
 * RC @ L.EIC 2122
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct url {
        char usr[32];
        char pwd[32];
        char host[32];
        char path[256];
};

struct url
parse_url(char *url);

