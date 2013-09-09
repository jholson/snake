CC = /usr/bin/g++
CFLAGS =
LIBS = -lncurses -lrt

.PHONY: all clean

all: snake

snake: snake.cc
	$(CC) $(CFLAGS) -o snake snake.cc $(LIBS)

ncurses_tutorial: ncurses_tutorial.cc
	$(CC) $(CFLAGS) -o ncurses_tutorial ncurses_tutorial.cc $(LIBS)

clean:
	rm -f snake ncurses_tutorial
