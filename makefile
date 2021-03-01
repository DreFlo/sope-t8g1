CC = gcc

CFLAGS = -g -Wall

TARGET = xmod

all: ret

ret: main.o xmod_utils.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o xmod_utils.o

main.o: main.c xmod_utils.h
	gcc -c main.c

xmod_utils.o: xmod_utils.c xmod_utils.h
	gcc -c xmod_utils.c

clean:
	$(RM) $(TARGET)