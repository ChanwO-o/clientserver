CC=gcc

#Compile with: 
#-g = debug
#-m32 = 32bit x86 executable

all: client server

client: client.o
	$(CC) client.o -o client

client.o: client.c
	$(CC) -c client.c

server: server.o
	$(CC) server.o -o server

server.o: server.c
	$(CC) -c server.c

clean:
	rm *.o client
	rm *.o server


