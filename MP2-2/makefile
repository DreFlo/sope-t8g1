all: c s

s: server.o lib.o delay.c delay.h server_signals.c server_signals.h server_utils.o
	gcc -Wall -DDELAY=100 -o s delay.c lib.o server.o server_signals.c server_utils.o -pthread

c: client.c common.h
	gcc -Wall -o c client.c -pthread

server_utils.o: server_utils.c server_utils.h common.h
	gcc -Wall -c -o server_utils.o server_utils.c

server.o: server.c common.h
	gcc -Wall -c -o server.o server.c

lib.o: lib.c lib.h delay.h
	gcc -Wall -c -o lib.o lib.c

clean:
	rm -f s c
