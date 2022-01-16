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


/* macros */
#define FTP_PORT 21

/* typedefs */
typedef struct url URL;


/** 
 * geturl - Parses the given url and port filling a URL structure
 * @url: an url string
 * @port: a port in which the connection should be made
 * 
 * The main function should call this with port 21 aka FTP_PORT and
 * should call free on the returned pointer after using it
 * Return: URL*
 */
URL *geturl(const char *url, const unsigned short port);


/**
 * start - Starts the connection with the ftp server. 
 * @u: a URL* for a struct given by geturl()
 *
 * Return: file descriptor for the opened socket
 */
int start(const URL *u);

/**
 * stop - Stops the connection with the ftp server. 
 * @sockfd: file descriptor of the socket hosting the connection
 * 
 * Must be called in order to finish a connection successfully
 */
void stop(int sockfd);


/**
 * login - Logins in the ftp server 
 * @sockfd: file descriptor of the socket hosting the connection
 * @u: a URL* for a struct given by geturl()
 * 
 * Return: 0 if successful <0 value otherwise 
 */
int login(int sockfd, const URL *u);

/**
 * passive - Enters in passive mode
 * @sockfd: file descriptor of the socket hosting the connection
 * @u: a URL* for a struct given by geturl()
 * 
 * The main function should call free on the returned pointer after using it
 * Return: a new URL* to allow open the retrieve connection of ftp (RFC 959)
 */
URL *passive(int sockfd, const URL *u);

/**
 * retrieve - Retrieves the desired file from the ftp server  
 * @sockfd_auth: file descriptor of the socket hosting the connection
 * @sockfd_retr: file descriptor of the socket hosting the connection
 * @u: a URL* for a struct given by geturl()
 * @fp: file to be downloaded
 * 
 * Return: 0 if successful <0 value otherwise 
 */
int retrieve(int sockfd_auth, int sockfd_retr, const URL *u, FILE *fp);

#endif /* _CONNECTION_H_ */

