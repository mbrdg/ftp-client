/**
* connection.h
* ftp client
* Authors: Miguel Rodrigues & Nuno Castro
* RC @ L.EIC 2122
*/

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#include <assert.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum {
       USER,
       PASS,
       PASV,
       RETR
} CMD;

typedef struct {
        char user[32];
        char pass[32];
        char host[32];
        char path[256];
} URL;

URL
get_url(const char *url);

int
start(const URL *url);

void
stop(int sockfd);

unsigned short
get_response(int sockfd);

void
command(int sockfd, CMD cmd, const char *content);

#endif /* _CONNECTION_H_ */

