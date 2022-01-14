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


/* Fills an internal struct to allow connection to the ftp server. */
URL *geturl(const char *url);

/* Starts the connection with the ftp server. */
int start(const URL *u);
/* Stops the connection with the ftp server. */
void stop(int sockfd);

/* Logins in the ftp server */
int login(int sockfd, const URL *u);
/* Enters in passive mode */
URL *passive(int sockfd, const URL *u);
/* Retrieves the desired file from the ftp server */
int retrieve(int sockfd_auth, int sockfd_retr, const URL *u, FILE *fp);

#endif /* _CONNECTION_H_ */

