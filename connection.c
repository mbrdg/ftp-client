/**
* connection.c
* ftp client
* Authors: Miguel Rodrigues & Nuno Castro
* RC @ L.EIC 2122
*/

#include "connection.h"


/* enums */
typedef enum { USER, PASS, PASV, RETR, QUIT } CMD;
typedef enum { ACCEPT = 220, PASS_SPEC = 331, LOGIN = 230, PASSIVE = 227 } CODE;

/* structs */
struct url {
        char user[32];
        char pass[64];
        char host[32];
        unsigned short port;
        char path[512];
};

/* globals */
static const char
cmds[][5] = { "USER", "PASS", "PASV", "RETR", "QUIT" };



static unsigned short
response(int sockfd, char *info, size_t info_len)
{
        unsigned short code;
        int tmp_sockfd;
        FILE *fp;

        char* line = NULL;
        size_t len = 0;

        tmp_sockfd = dup(sockfd);
        fp = fdopen(tmp_sockfd, "r");
        assert(fp != NULL);

        do {
                assert(getline(&line, &len, fp) >= 0);
        } while(line[3] != ' ');

        sscanf(line, "%hu [^\r\n]\r\n", &code);
        if (info != NULL)
                strncpy(info, line, info_len);

        fclose(fp);
        return code;
}

static void
command(int sockfd, CMD cmd, const char *arg)
{
        char fmt[strlen(arg) + 4 + 4];
        snprintf(fmt, sizeof(fmt), "%s %s\r\n", cmds[cmd], arg);

        size_t wb;
        wb = send(sockfd, fmt, strlen(fmt), 0);
        assert(wb >= 0);
}


URL *
parse_url(const char *url, const unsigned short port)
{
        URL *u;
        u = calloc(1, sizeof(URL));
        assert(u != NULL);

        char l[128];
        sscanf(url, "ftp://%128[^/]/%512s", l, u->path);

        strncpy(u->host, l, strlen(l));
        strncpy(u->user, "anonymous", 10);
        strncpy(u->pass, "anonymous", 10);
        u->port = port;

        if (strchr(l, '@'))
                sscanf(l, "%32[^:]:%64[^@]@%32s", u->user, u->pass, u->host);

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

        char ip[3 * 4 + 4 + 1];
        snprintf(ip, sizeof(ip), inet_ntoa(*((struct in_addr *) h->h_addr)));

        int sockfd;
        struct sockaddr_in server_addr;

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(ip);
        server_addr.sin_port = htons(u->port);

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        assert(sockfd > 0);

        int cnct;
        cnct = connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
        assert(cnct == 0);

        fprintf(stderr, "[INFO] connection established with '%s'.\n", ip);
        return sockfd;
}

void
end_connection(int sockfd)
{
        command(sockfd, QUIT, "");
        close(sockfd);
}



int
login(int sockfd, const URL *u) {
        if (response(sockfd, NULL, 0) != ACCEPT)
                return -ACCEPT;

        command(sockfd, USER, u->user);
        if (response(sockfd, NULL, 0) != PASS_SPEC)
                return -PASS_SPEC;

        command(sockfd, PASS, u->pass);
        if (response(sockfd, NULL, 0) != LOGIN)
                return -LOGIN;

        fprintf(stderr, "[INFO] login completed successfully\n");
        return 0;
}

URL *
passive(int sockfd, const URL *u)
{
        char info[1024];

        command(sockfd, PASV, "");
        if (response(sockfd, info, sizeof(info)) != PASSIVE)
                return NULL;

        char data[24];
        unsigned char ip[6];

        sscanf(info, "%*[^(](%[^)]).\r\n", data);
        sscanf(data, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &ip[0], &ip[1], &ip[2], &ip[3], &ip[4], &ip[5]);

        char addr[6 + 32 + 64 + 17 + 1];
        if (strncmp(u->user, "anonymous", 10) != 0)
                snprintf(addr, sizeof(addr), "ftp://%s:%s@%hhu.%hhu.%hhu.%hhu", u->user, u->pass, ip[0], ip[1], ip[2], ip[3]);
        else
                snprintf(addr, sizeof(addr), "ftp://%hhu.%hhu.%hhu.%hhu", ip[0], ip[1], ip[2], ip[3]);

        return parse_url(addr, ip[4] * 256 + ip[5]);
}

