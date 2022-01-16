/**
* connection.c
* ftp client
* Authors: Miguel Rodrigues & Nuno Castro
* RC @ L.EIC 2122
*/

#include "connection.h"

/* macros */
#define MAX_LINE_LEN 1024

/* enums */
typedef enum { 
        USER, 
        PASS,
        PASV,
        RETR,
        QUIT 
} CMD;

typedef enum { 
        OPEN        = 150, 
        ACCEPT      = 220, 
        TRANSFER    = 226, 
        PASSIVE     = 227, 
        LOGIN       = 230, 
        PASS_SPEC   = 331
} CODE;

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


URL *
geturl(const char *url, const unsigned short port)
{
        URL *u;
        u = calloc(1, sizeof(URL));
        assert(u != NULL);

        char l[128];
        sscanf(url, "ftp://%128[^/]/%512s", l, u->path);

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

/*
 *  Here we really need to kkep track of the file descriptor open
 *  by the connect(). This means that is needs to be duplicated here
 *  just because of the response, otherwise when this function is done
 *  and calls fclose() the file descriptor will be closed as well.
 *  
 *  More info is available in fclose() man page:
 *  The fclose() function shall perform the equivalent of a close() on
 *  the file descriptor that is associated with the stream pointed to 
 *  by stream.  
 */
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
        unsigned short resp;

        resp = response(sockfd, NULL, 0);
        if (resp != ACCEPT)
                return -ACCEPT;

        command(sockfd, USER, u->user);
        resp = response(sockfd, NULL, 0);
        if (resp == LOGIN)
                return 0;
        if (resp != PASS_SPEC)
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

        url = geturl(addr, ip[4] * 256 + ip[5]);
        if (strncmp(u->user, anon, strlen(anon)) != 0) {
                strncpy(url->user, u->user, strlen(u->user));
                strncpy(url->pass, u->pass, strlen(u->pass));
        }

        return url;
}

int
retrieve(int sockfd_auth, int sockfd_retr, const URL *u, FILE *fp)
{
        char *filename, info[MAX_LINE_LEN];
        unsigned char fragment[1024];

        ssize_t rb;
        size_t fsize;

        filename = strrchr(u->path, '/') + 1;
        fp = fopen(filename, "wb");
        assert(fp != NULL);

        command(sockfd_auth, RETR, u->path);
        if (response(sockfd_auth, info, sizeof(info)) != OPEN) {
                fclose(fp);
                return -OPEN;
        }

        sscanf(info, "%*[^(](%zu bytes).\r\n", &fsize);
        
        do {
                rb = recv(sockfd_retr, fragment, sizeof(fragment), 0);
                fwrite(fragment, 1, rb, fp);
        } while (rb > 0);  

        fclose(fp);
        if (response(sockfd_auth, NULL, 0) != TRANSFER)
                return -TRANSFER;

        return 0;
}

