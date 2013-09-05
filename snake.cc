#include <ncurses.h>
#include <string>

const std::string title = "SNAKE";

void configure_stdscr();

int main()
{
	// TODO: For testin the keypad() behavior
	char c;

	int row, col;

	// Start curses mode
	initscr();

	// Configure the screen we initialized above
	configure_stdscr();

	// Store the row and col of stdscr
	getmaxyx(stdscr, row, col);

	// Print title screen in the center
	mvprintw(row/2, (col - title.length())/2, "%s", title.c_str());

	c = getch();

	// Get rid of the title before printing our next string. Note that it doesn't
	// "redraw the screen from scratch" so there's no blinking (i.e. what you'd see with
	// clear())
	erase();
	// Now print something else to test addstr()
	mvaddstr(row/2, (col - title.length())/2, "WHOA");

	refresh();

	// Wait for user input before terminating
	c = getch();

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
