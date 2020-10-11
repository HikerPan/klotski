IDIR = headers
CC = gcc
CFLAGS=-I $(IDIR) -Wall

VPATH = source;headers

DEPS = position.h queue.h
OBJ = main.o position.o queue.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

klotski: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	del	klotski* $(OBJ)
