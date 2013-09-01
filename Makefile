CC = /usr/bin/g++
CFLAGS =
LIBS = -lncurses

all: snake

.PHONY: clean

snake: snake.cc
	$(CC) $(CFLAGS) -o snake snake.cc $(LIBS)

clean:
	rm -f snake
