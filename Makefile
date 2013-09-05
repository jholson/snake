CC = /usr/bin/g++
CFLAGS =
LIBS = -lncurses

.PHONY: all clean

all: snake

snake: snake.cc
	$(CC) $(CFLAGS) -o snake snake.cc $(LIBS)

clean:
	rm -f snake
