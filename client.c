#include <stdio.h>
#include <string.h>

//MACROS
#define MAX_LINE_NUM 200

void start()
{
	char buffer[100];

	while(1)
	{
		printf("> ");
		fgets(buffer, 100, stdin);

		char * game_id;
		char * field;

		char * cmd = strtok(buffer, " \t\n");
		if (strcmp(cmd, "quit") == 0)
		{
			break;
		}
		else if (cmd != NULL)
		{
			game_id = atoi(cmd);
			cmd = strtok(NULL, " ");
			if (cmd != NULL)
			{
				field = cmd;
			}

			if (game_id == NULL || field == NULL)
			{
				printf("unknown\n");
				game_id = NULL;
				field = NULL;
			}

			printf("%d\n", game_id);
			printf("%s\n", field);
		}
	}
}

int main(int argc, char * argv[])
{
    char * domainName = argv[1];
    char * portnum = atoi(argv[2]);

    start();

    return 0;
}
