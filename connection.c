/**
* connection.c
* ftp client
* Authors: Miguel Rodrigues & Nuno Castro
* RC @ L.EIC 2122
*/

#include "connection.h"

#define FTP_PORT 21

static const char
str_cmd[][5] = { "USER", "PASS", "PASV", "RETR" };


URL
get_url(const char *url)
{
        URL surl;
        char tmp[256];

        sscanf(url, "ftp://%[^/]%s", tmp, surl.path);
        const char *got_user = strchr(tmp, '@');

        if (got_user) {
                char utmp[256];
                sscanf(tmp, "%[^@]@%s", utmp, surl.host);
                sscanf(utmp, "%[^:]:%s", surl.user, surl.pass);
        } else {
                strncpy(surl.host, tmp, 32);
                strncpy(surl.user, "anonymous", 10);
                strncpy(surl.pass, "", 1);
        }

        return surl;
}


int
start(const URL *url)
{
        struct hostent *h;
        h = gethostbyname2(url->host, AF_INET);
        assert(h != NULL);

        char ip[17];
        snprintf(ip, 17, inet_ntoa(*((struct in_addr *) h->h_addr)));

        int sockfd;
        struct sockaddr_in server_addr;

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ip);
        server_addr.sin_port = htons(FTP_PORT);

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        assert(sockfd > 0);

        int connection;
        connection = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
        assert(connection >= 0);

        fprintf(stderr, "[INFO] connection established with %s.\n", ip);
        return sockfd;
}

void
stop(int sockfd)
{
        close(sockfd);
}


// FIXME - why has this trick needed?
unsigned short
get_response(int sockfd)
{
        unsigned short code;
        FILE *fp = fdopen(sockfd, "r");

        char *tmp;
        size_t rb;
        while (getline(&tmp, &rb, fp) > 0) {
                fprintf(stdout, "%s", tmp);

                if (tmp[3] == ' ') {
                        sscanf(tmp, "%hu", &code);
                        break;
                }
        }

        fclose(fp);
        return code;
}

// Segfaults...
void
command(int sockfd, CMD cmd, const char *content)
{
        char fmt[strlen(content) + 6];
        snprintf(fmt, sizeof(fmt), "%s %s\r\n", str_cmd[cmd], content);

        size_t wb;
        wb = send(sockfd, fmt, sizeof(fmt), 0);
        assert(wb >= 0);

        fprintf(stderr, "[INFO] command '%s' sent.\n", fmt);
}

