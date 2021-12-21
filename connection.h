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

typedef struct url URL;


URL *
parse_url(const char *url);

void
destroy_url(URL *u);


int
start_connection(const URL *u);

void
end_connection(int sockfd);


int
login(int sockfd, const URL *u);


#endif /* _CONNECTION_H_ */

