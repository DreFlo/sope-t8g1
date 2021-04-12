CC = gcc

CFLAGS = -g -Wall -Wextra

LIBS = -pthread

all: s c

s: server.o lib.o delay.c delay.h
	gcc -Wall -DDELAY=0 -o s delay.c lib.o server.o -pthread

c: client.o client_utils.o
	$(CC) $(CFLAGS) -o c client.o client_utils.o $(LIBS)

client.o: client.c client_utils.h
	$(CC) $(CFLAGS) -c client.c

client_utils.o: client_utils.c client_utils.h
	$(CC) $(CFLAGS) -c client_utils.c

# server.o: server.c
# 	gcc -Wall -c -o server.o server.c

# lib.o: lib.c lib.h
# 	gcc -Wall -c -o lib.o lib.c

clean:
	rm -f s c client.o client_utils.o
