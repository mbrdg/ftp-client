/**
* connection.c
* ftp client
* Authors: Miguel Rodrigues & Nuno Castro
* RC @ L.EIC 2122
*/

#include "connection.h"

/* macros */
#define FTP_PORT 21
#define MAX_LINE_LEN 1024

/* enums */
typedef enum { USER, PASS, PASV, RETR, QUIT } CMD;
typedef enum { OPEN = 150, ACCEPT = 220, TRANSFER = 226, PASSIVE = 227, LOGIN = 230, PASS_SPEC = 331 } CODE;

/* structs */
struct url {
        char           user[32];
        char           pass[64];
        char           host[32];
        unsigned short port;
        char           path[512];
};

/* globals */
static const char cmds[][5] = { "USER", "PASS", "PASV", "RETR", "QUIT" };
static const char anon[] = "anonymous";


static URL *
parse(const char *url, const unsigned short port)
{
        URL *u;
        u = calloc(1, sizeof(URL));
        assert(u != NULL);

        char l[32];
        sscanf(url, "ftp://%32[^/]/%512s", l, u->path);

        strncpy(u->host, l, strlen(l));
        strncpy(u->user, anon, 10);
        strncpy(u->pass, "", 1);
        u->port = port;

        if (strchr(l, '@'))
                sscanf(l, "%32[^:]:%64[^@]@%32s", u->user, u->pass, u->host);

        return u;
}

static unsigned short
response(int sockfd, char *info, size_t infolen)
{
        unsigned short code;
        int s;
        FILE *fp;
        char line[MAX_LINE_LEN];

        s = dup(sockfd);
        fp = fdopen(s, "r");
        assert(fp != NULL);

        do {
		char *buffer;
		buffer = fgets(line, sizeof(line), fp);
		assert(buffer == line);
        } while(line[3] != ' ');

        sscanf(line, "%hu [^\r\n]\r\n", &code);
        if (info != NULL)
                strncpy(info, line, infolen);

        fclose(fp);
        return code;
}

static void
command(int sockfd, CMD cmd, const char *arg)
{
	    char fmt[strlen(arg)+8];
	    ssize_t wb;

        snprintf(fmt, sizeof(fmt), "%s %s\r\n", cmds[cmd], arg);
	    wb = send(sockfd, fmt, strlen(fmt), 0);
	    assert(wb >= 0);
}


URL *
geturl(const char *url)
{
        return parse(url, FTP_PORT);
}


int
start(const URL *u)
{
        struct addrinfo hints, *res, *r;
        int s, connection = 1;
        char port[6];

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        snprintf(port, sizeof(port), "%hu", u->port);

        getaddrinfo(u->host, port, &hints, &res);
        for (r = res; r != NULL; r = res->ai_next) {
                s = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
                if (s < 0)
                        continue;

                connection = connect(s, r->ai_addr, r->ai_addrlen);
                if (!connection)
                        break;
                close(s);
        }

        return !connection ? s : -1;
}

void
stop(int sockfd)
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

        return 0;
}

URL *
passive(int sockfd, const URL *u)
{
        char info[MAX_LINE_LEN], data[24], addr[INET_ADDRSTRLEN+6];
        unsigned char ip[6];
        URL *url;

        command(sockfd, PASV, "");
        if (response(sockfd, info, sizeof(info)) != PASSIVE)
                return NULL;

        sscanf(info, "%*[^(](%24[^)]).\r\n", data);
        sscanf(data, "%hhu,%hhu,%hhu,%hhu,%hhu,%hhu", &ip[0], &ip[1], &ip[2], &ip[3], &ip[4], &ip[5]);
        snprintf(addr, sizeof(addr), "ftp://%hhu.%hhu.%hhu.%hhu/", ip[0], ip[1], ip[2], ip[3]);

        url = parse(addr, ip[4] * 256 + ip[5]);
        if (strncmp(u->user, anon, strlen(anon)) != 0) {
                strncpy(url->user, u->user, strlen(u->user));
                strncpy(url->pass, u->pass, strlen(u->pass));
        }

        return url;
}

int
retrieve(int sockfd_auth, int sockfd_retr, const URL *u, FILE *fp)
{
        char *filename, fragment[1024], info[MAX_LINE_LEN];
        size_t filesize, iter, i, rb;

        filename = strrchr(u->path, '/') + 1;
        fp = fopen(filename, "wb");
        assert(fp != NULL);

        command(sockfd_auth, RETR, u->path);
        if (response(sockfd_auth, info, sizeof(info)) != OPEN) {
                fclose(fp);
                return -OPEN;
        }

        sscanf(info, "%*[^(](%zu bytes).\r\n", &filesize);

        iter = filesize / sizeof(fragment);
        iter += filesize % sizeof(fragment) != 0;
        for (i = 0; i < iter; i++) {
                rb = recv(sockfd_retr, fragment, sizeof(fragment), 0);
                assert(rb >= 0);
                fwrite(fragment, 1, rb, fp);
        }

        fclose(fp);
        if (response(sockfd_auth, NULL, 0) != TRANSFER)
                return -TRANSFER;

        return 0;
}

