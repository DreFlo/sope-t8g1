CC = gcc

CFLAGS = -g -Wall

TARGET = xmod

all: ret

ret: $(TARGET)
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	$(RM) $(TARGET)