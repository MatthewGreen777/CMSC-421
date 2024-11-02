CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -lpthread -lrt

all: main app

main: main.c buffer.o
	$(CC) $(CFLAGS) -o main main.c buffer.o $(LDFLAGS)

app: app.o
	$(CC) $(CFLAGS) -o app app.o buffer.o $(LDFLAGS)
	
app.o: app.c app.h
	$(CC) $(CFLAGS) -c app.c -o app.o

buffer.o: buffer.c
	$(CC) $(CFLAGS) -c buffer.c -o buffer.o

clean:
	rm -f *.o main app

.PHONY: all clean

