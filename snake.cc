#include <ncurses.h>

void configure_stdscr();

int main()
{
	// TODO: For testin the keypad() behavior
	char c;

	// Start curses mode
	initscr();

	// Configure the screen we initialized above
	configure_stdscr();

	printw("Hello World!");

	refresh();

	// Wait for user input
	c = getch();
	printw("%c", c);
	c = getch();
	printw("%c", c);
	c = getch();
	printw("%c", c);
	c = getch();
	printw("%c", c);
	c = getch();
	printw("%c", c);

	// End curses mode
	endwin();

	return 0;
}

void configure_stdscr()
{
	// Disable line buffering, but characters are still echo'd to the terminal like 
	// ctrl-z
	cbreak();

	// Don't echo user input back to the screen. We'll print it if we need to
	noecho();

	// Enables  function keys (e.g. F1) and arrow keys
	// TODO: Hm if this isn't set then the arrow keys are still processed by
	// get char but I think multiple chars are sent per arrow key (or function key)
	keypad(stdscr, TRUE);
}
