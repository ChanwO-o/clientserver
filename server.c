// Chan Woo Park 26984415 Min Sung Cha 85408485
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>


#define MAXLINE 200 /* MAX NUM OF LINES */
#define LISTENQ 100 /* SIZE OF QUEUE */


struct Match {
	char type[5], home_team[4], away_team[4], week[4], season[5], home_score[4], away_score[4];
	int game_id;
};

struct Match matches[MAXLINE];

void echo(int connfd)
{
	size_t n;
	char buf[MAXLINE];
	while((n = read(connfd, buf, MAXLINE)) != 0) {
		printf("server received %d bytes\n", (int) n);
		write(connfd, buf, n);
	}
}

char* searchdb(char* gameidstr, char* query) {
	int gameid = atoi(gameidstr);
	int i;
	for (i = 0; i < MAXLINE; ++i) {
		if (matches[i].game_id == gameid) {
			if (strcmp(query, "type") == 0) {
				printf("%s %s\n", gameidstr, query);
				return matches[i].type;
			}
			else if (strcmp(query, "game_id") == 0) {
				printf("%s %s\n", gameidstr, query);
				return gameidstr;
			}
			else if (strcmp(query, "home_team") == 0) {
				printf("%s %s\n", gameidstr, query);
				return matches[i].home_team;
			}
			else if (strcmp(query, "away_team") == 0) {
				printf("%s %s\n", gameidstr, query);
				return matches[i].away_team;
			}
			else if (strcmp(query, "week") == 0) {
				printf("%s %s\n", gameidstr, query);
				return matches[i].week;
			}
			else if (strcmp(query, "season") == 0) {
				printf("%s %s\n", gameidstr, query);
				return matches[i].season;
			}
			else if (strcmp(query, "home_score") == 0) {
				printf("%s %s\n", gameidstr, query);
				return matches[i].home_score;
			}
			else if (strcmp(query, "away_score") == 0) {
				printf("%s %s\n", gameidstr, query);
				return matches[i].away_score;
			}
		}
	}
	return "unknown";
}


void respond(int connfd) {
	size_t n;
	char buf[MAXLINE];
	while((n = read(connfd, buf, MAXLINE)) != 0) {
		// printf("%d %s#\n", n, buf);
		int bytesize = buf[0];
		char input[bytesize+1];
		strncpy(input, buf+1, bytesize);
		input[bytesize] = '\0';
		// printf("input: %s, bytesize: %d\n", input, bytesize);
		
		char* token = strtok(input, " ");
		// printf("gameid: %s  atoi: %d\n", to, atoi(gameid));
		if (atoi(token) == 0) { // first arg is not integer
			char unknownres[256];
			char unknownsz = strlen("unknown");
			unknownres[0] = unknownsz;
			strcpy(unknownres+1, "unknown");
			
			write(connfd, unknownres, n);
		}
		else {
			if (token != NULL)
			{
				char* gameidstr = token;
				token = strtok(NULL, " ");
				
				if (token != NULL)
				{
					char * field = token;
					char* response = searchdb(gameidstr, field);
					
					char info[256];
					char bytesize = strlen(response);
					info[0] = bytesize;
					strcpy(info+1, response);
					
					write(connfd, info, strlen(info));
				}
			}
		}
	}
}

void loadfile(char* filename) {
	FILE* dbfile;
	char* line = NULL;
	size_t len = 0;
	ssize_t read;
	int linenum = 1;
	
	dbfile = fopen(filename, "r");
	if (dbfile == NULL)
		return;
	while ((read = getline(&line, &len, dbfile)) != -1) {
		if (linenum == 1) {
			linenum++;
			continue;
		}
		
		char* type = strtok(line, ","); // extract nextRooms
		int game_id = atoi(strtok(NULL, ","));
		char* home_team = strtok(NULL, ",");
		char* away_team = strtok(NULL, ",");
		char* week = strtok(NULL, ",");
		char* season = strtok(NULL, ",");
		char* home_score = strtok(NULL, ",");
		char* away_score = strtok(NULL, ",\n");
		
		// printf("%d: %s %d %s %s %d %d %d %d \n", linenum, type, game_id, home_team, away_team, week, season, home_score, away_score);
		
		struct Match match;
		match.game_id = game_id;
		strcpy(match.type, type);
		strcpy(match.home_team, home_team);
		strcpy(match.away_team, away_team);
		strcpy(match.week, week);
		strcpy(match.season, season);
		strcpy(match.home_score, home_score);
		strcpy(match.away_score, away_score);
		
		matches[linenum - 2] = match;
		
		if (linenum == 200)
			break;
		linenum++;
	}
	// printf("%d\n", matches[198].game_id); // last element in array
	free(line);
}


int main(int argc, char * argv[])
{
	char* dbfilename = argv[1];
	loadfile(dbfilename); // load database
	
	// printf(searchdb("2018090600", "away_team"));
	// printf(searchdb("2018090600", "away_score"));
	
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr; /* Enough room for any addr*/
	char client_hostname[MAXLINE], client_port[MAXLINE];
	
	listenfd = open_listenfd(argv[2]);
	
	printf("server started\n");
	
	while(1) {
		clientlen = sizeof(struct sockaddr_storage); /* Important! */
		connfd = accept(listenfd, (struct sockaddr_storage *)&clientaddr, &clientlen);
		getnameinfo((struct sockaddr_storage *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
		// printf("Connected to (%s, %s)\n", client_hostname, client_port);
		//echo(connfd);
		respond(connfd);
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
