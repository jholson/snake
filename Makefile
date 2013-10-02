CC = /usr/bin/g++
CFLAGS =
LIBS = -lncurses -lrt

.PHONY: all debug clean

all: snake

debug: CFLAGS += -DDEBUG
debug: all

snake: snake.cc
	$(CC) $(CFLAGS) -o snake snake.cc $(LIBS)

ncurses_tutorial: ncurses_tutorial.cc
	$(CC) $(CFLAGS) -o ncurses_tutorial ncurses_tutorial.cc $(LIBS)

clean:
	rm -f snake ncurses_tutorial
