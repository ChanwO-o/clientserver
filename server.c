#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>


#define MAXLINE 200 /* MAX NUM OF LINES */
#define LISTENQ 100 /* SIZE OF QUEUE */


void echo(int connfd)
{
	size_t n;
	char buf[MAXLINE];
	while((n = read(connfd, buf, MAXLINE)) != 0) {
		printf("server received %d bytes\n", (int) n);
		write(connfd, buf, n);
	}
}


int main(int argc, char * argv[])
{
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr; /* Enough room for any addr*/
	char client_hostname[MAXLINE], client_port[MAXLINE];
	listenfd = open_listenfd(argv[1]);
	
	while(1) {
		clientlen = sizeof(struct sockaddr_storage); /* Important! */
		connfd = accept(listenfd, (struct sockaddr_storage *)&clientaddr, &clientlen);
		getnameinfo((struct sockaddr_storage *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
		printf("Connected to (%s, %s)\n", client_hostname, client_port);
		echo(connfd);
		close(connfd);
	}
	exit(0);
}



/*  
 * open_listenfd - Open and return a listening socket on port. This
 *	 function is reentrant and protocol-independent.
 *
 *	 On error, returns: 
 *	   -2 for getaddrinfo error
 *	   -1 with errno set for other errors.
 */
/* $begin open_listenfd */
int open_listenfd(char *port)
{
	struct addrinfo hints, *listp, *p;
	int listenfd, rc, optval=1;

	/* Get a list of potential server addresses */
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;			 /* Accept connections */
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
	hints.ai_flags |= AI_NUMERICSERV;			/* ... using port number */
	if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
		fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(rc));
		return -2;
	}

	/* Walk the list for one that we can bind to */
	for (p = listp; p; p = p->ai_next) {
		/* Create a socket descriptor */
		if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
			continue;  /* Socket failed, try the next */

		/* Eliminates "Address already in use" error from bind */
		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,	//line:netp:csapp:setsockopt
					(const void *)&optval , sizeof(int));

		/* Bind the descriptor to the address */
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
			break; /* Success */
		if (close(listenfd) < 0) { /* Bind failed, try the next */
			fprintf(stderr, "open_listenfd close failed: %s\n", strerror(errno));
			return -1;
		}
	}

	/* Clean up */
	freeaddrinfo(listp);
	if (!p) /* No address worked */
		return -1;

	/* Make it a listening socket ready to accept connection requests */
	if (listen(listenfd, LISTENQ) < 0) {
		close(listenfd);
		return -1;
	}
	return listenfd;
}
