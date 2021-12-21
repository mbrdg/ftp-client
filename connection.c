/**
* connection.c
* ftp client
* Authors: Miguel Rodrigues & Nuno Castro
* RC @ L.EIC 2122
*/

#include "connection.h"

/* macros */
#define FTP_DEFAULT_PORT 21

/* enums */
typedef enum { USER, PASS, PASV, RETR } CMD;
typedef enum { ACCEPT = 220, PASS_SPEC = 331, LOGIN = 230 } FTP_CODE;

/* structs */
struct url {
        char user[32];
        char pass[32];
        char host[32];
        char path[256];
};

/* globals */
static const char
cmds[][5] = { "USER", "PASS", "PASV", "RETR" };



URL *
parse_url(const char *url)
{
        URL *u;
        char tmp[256];

        u = malloc(sizeof(URL));
        assert(u != NULL);
        sscanf(url, "ftp://%[^/]%s", tmp, u->path);

        const char *usr = strchr(tmp, '@');
        if (usr) {
                char utmp[256];
                sscanf(tmp, "%[^@]@%s", utmp, u->host);
                sscanf(utmp, "%[^:]:%s", u->user, u->pass);
        } else {
                strncpy(u->host, tmp, 32);
                strncpy(u->user, "anonymous", 10);
                strncpy(u->pass, "anonymous", 10);
        }

        return u;
}

void
destroy_url(URL *u)
{
        free(u);
}



int
start_connection(const URL *u)
{
        struct hostent *h;
        h = gethostbyname2(u->host, AF_INET);
        assert(h != NULL);

        char ip[17];
        snprintf(ip, 17, inet_ntoa(*((struct in_addr *) h->h_addr)));

        int sockfd;
        struct sockaddr_in server_addr;

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ip);
        server_addr.sin_port = htons(FTP_DEFAULT_PORT);

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        assert(sockfd > 0);

        int cnct;
        cnct = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
        assert(cnct >= 0);
        fprintf(stderr, "[INFO] connection established with '%s'.\n", ip);

        return sockfd;
}

void
end_connection(int sockfd)
{
        close(sockfd);
}



static unsigned short
read_resp(int sockfd)
{
        unsigned short code;
        int tmp_sockfd;
        FILE *fp;

        tmp_sockfd = dup(sockfd);
        fp = fdopen(tmp_sockfd, "r");
        assert(fp != NULL);

        char line[128];
        while (1) {
                fgets(line, sizeof(line), fp);
                fprintf(stderr, "%s", line);
                if (line[3] == ' ')
                        goto parse_code;
        }

parse_code:
        sscanf(line, "%hu [^\r\n]\r\n", &code);

        fclose(fp);
        return code;
}

static void
send_cmd(int sockfd, CMD cmd, const char *content)
{
        char fmt[strlen(content) + 4 + 4];
        snprintf(fmt, sizeof(fmt), "%s %s\r\n", cmds[cmd], content);

        size_t wb;
        wb = send(sockfd, fmt, strlen(fmt), 0);
        assert(wb >= 0);
}


int
login(int sockfd, const URL *u) {
        if (read_resp(sockfd) != ACCEPT)
                return -1;

        send_cmd(sockfd, USER, u->user);
        if (read_resp(sockfd) != PASS_SPEC)
                return -1;

        send_cmd(sockfd, PASS, u->pass);
        if (read_resp(sockfd) != LOGIN)
                return -1;

        fprintf(stderr, "[INFO] login completed successfully\n");
        return 0;
}

