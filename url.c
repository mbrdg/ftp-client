/**
 * url.c
 * Authors: Miguel Rodrigues & NUno Castro
 * RC @ L.EIC 2122
 */

#include "url.h"


struct url
parse_url(char *url_str)
{
        struct url url;
        char tmp[256];

        sscanf(url_str, "ftp://%[^/]%s", tmp, url.path);
        const char *usr = strchr(tmp, '@');

        if (usr) {
                char utmp[256];
                sscanf(tmp, "%[^@]@%s", utmp, url.host);
                sscanf(utmp, "%[^:]:%s", url.usr, url.pwd);
        } else {
                strncpy(url.host, tmp, 32);
                strncpy(url.usr, "anonymous", 10);
                strncpy(url.pwd, "", 1);
        }

        return url;
}

