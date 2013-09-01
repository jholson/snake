CC = /usr/bin/g++
CFLAGS = -lncurses

all: snake

.PHONY: clean

snake: snake.cc
	$(CC) $(CFLAGS) -o snake snake.cc

clean:
	rm -f snake
