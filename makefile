CC = gcc

CFLAGS = -g -Wall

TARGET = xmod

all: ret

ret: main.o xmod_utils.o xmod_sig_handlers.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o xmod_utils.o xmod_sig_handlers.o

main.o: main.c xmod_utils.h xmod_sig_handlers.h xmod_macros.h
	gcc -c main.c

xmod_utils.o: xmod_utils.c xmod_utils.h xmod_macros.h
	gcc -c xmod_utils.c

xmod_sig_handlers.o: xmod_sig_handlers.c xmod_sig_handlers.h xmod_macros.h
	gcc -c xmod_sig_handlers.c

clean:
	$(RM) $(TARGET)