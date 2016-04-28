FLAGS = -Wall -O1 -std=c99 -D_POSIX_SOURCE=200112L

all: server client

server: server.c sharedFunc.h sharedFunc.c
	gcc ${FLAGS} -o server server.c sharedFunc.c

client: client.c sharedFunc.h sharedFunc.c
	gcc ${FLAGS} -o client client.c sharedFunc.c

clean:
	rm -f server
	rm -f client
	rm -f core