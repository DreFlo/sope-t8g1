CC = gcc

CFLAGS = -g -Wall

TARGET = xmod

all: xmod

xmod: $(TARGET)
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	$(RM) $(TARGET)