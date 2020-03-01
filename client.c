// Chan Woo Park 26984415 Min Sung Cha 85408485
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdint.h>
#include <netdb.h>
#include <stdlib.h>


#define MAXLINE 200


int open_clientfd(char * hostname, char * port)
{

	int clientfd;
	struct addrinfo hints, *listp, *p;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_NUMERICSERV;
	hints.ai_flags |= AI_ADDRCONFIG;

	getaddrinfo(hostname, port, &hints, &listp);

	for (p=listp; p; p = p->ai_next)
	{
		if ((clientfd = socket(p->ai_family, p->ai_socktype, 
								p->ai_protocol)) < 0)
			continue;

		if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
			break;

		close(clientfd);
	}

	freeaddrinfo(listp);
	if (!p)
		return -1;
	else
		return clientfd;
}

void start(char * host, char * port)
{
	char buffer[MAXLINE];
	int clientfd;
	clientfd = open_clientfd(host, port);

	while (1)
	{
		printf("> ");
		if (fgets(buffer, MAXLINE, stdin) != NULL)
		{
			strtok(buffer, "\n"); // remove newline
			if (strcmp(buffer, "quit") == 0)
			{
				break;
			}
			else if (
				(strlen(buffer)-1) >= 0 && (strlen(buffer)-1) <= 256)
			{
				char info[256];
				char bitesize = strlen(buffer);

				info[0] = bitesize;
				strcpy(info+1, buffer);

				write(clientfd, info, strlen(info));
				read(clientfd, buffer, MAXLINE);
				
				int responsesz = buffer[0];
				char response[responsesz+1];
				strncpy(response, buffer+1, responsesz);
				response[responsesz] = '\0';				
			
				fputs(response, stdout);
				printf("\n");
			} 			
		}
	}

	close(clientfd);
	exit(0);
}

int main(int argc, char * argv[])
{
    char * domainName = argv[1];
    char * portnum = argv[2];

    start(domainName, portnum);

    return 0;
}
