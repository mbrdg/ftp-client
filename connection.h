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


/* typedefs */
typedef struct url URL;


URL *get(const char *url);

int start(const URL *u);
void stop(int sockfd);

int login(int sockfd_a, const URL *u);
URL *passive(int sockfd_a, const URL *u);
int retrieve(int sockfd_a, int sockfd_b, const URL *u, FILE *fp);

#endif /* _CONNECTION_H_ */

