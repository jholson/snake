#include <ncurses.h>

int main()
{
	// Start curses mode
	initscr();

	printw("Hello World!");

	refresh();

	// Wait for user input
	getch();

	// End curses mode
	endwin();

	return 0;
}
