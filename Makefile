CC=gcc
CFLAGS= -Wall -Werror -pthread -I.
all: server client

#%.o: server.c client.c $(DEPS)
#	$(CC) -c -o $@ $< $(CFLAGS)

server: server.o
	$(CC) $(CFLAGS) -o server server.c

client: client.o
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm server client server.o client.o