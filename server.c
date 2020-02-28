#include <stdio.h>
#include <string.h>

#define MAX_LINE_NUM 200

int main(int argc, char * argv[])
{
    printf("server started\n");
    
    char * fname = argv[1];
    char * portnum = atoi(argv[2]);

    printf("%s\n", fname);
    printf("%d\n", portnum);

    return 0;
}
