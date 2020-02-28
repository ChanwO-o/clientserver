#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdint.h>
#include <netdb.h>
#include <stdlib.h>

//MACROS
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
	char serverbuf[MAXLINE];

	int clientfd;

	clientfd = open_clientfd(host, port);

	while (1)
	{
		printf("> ");
		if (fgets(buffer, MAXLINE, stdin) != NULL)
		{
			if (strcmp(buffer, "quit\n") == 0)
			{
				exit(0);
			}
			else if (
				(strlen(buffer)-1) >= 0 && (strlen(buffer)-1) <= 256)
			{
				char info[256];
				char bitesize = strlen(buffer)-1;

				info[0] = bitesize;
				strcpy(info+1, buffer);

				write(clientfd, info, strlen(info));
				read(clientfd, serverbuf, MAXLINE);
				fputs(serverbuf, stdout);
			} 			
			// write(clientfd, buffer, strlen(buffer));
			// read(clientfd, buffer, MAXLINE);
			// fputs(buffer, stdout);
		}
	}

	// while (fgets(buffer, MAXLINE, stdin) != NULL)
	// {
	// 	printf("> ");
	// 	write(clientfd, buffer, strlen(buffer));
	// 	read(clientfd, buffer, MAXLINE);
	// 	fputs(buffer, stdout);
	// }

	close(clientfd);
	exit(0);

	// while(1)
	// {
	// 	printf("> ");
	// 	fgets(buffer, MAXLINE, stdin);

	// 	char * game_id;
	// 	char * field;

	// 	char * cmd = strtok(buffer, " \t\n");
	// 	if (strcmp(cmd, "quit") == 0)
	// 	{
	// 		break;
	// 	}
	// 	else if (cmd != NULL)
	// 	{
	// 		game_id = atoi(cmd);
	// 		cmd = strtok(NULL, " ");
	// 		if (cmd != NULL)
	// 		{
	// 			field = cmd;
	// 		}
	// 	}
	// }
}

int main(int argc, char * argv[])
{
    char * domainName = argv[1];
    char * portnum = argv[2];

    start(domainName, portnum);

    return 0;
}
