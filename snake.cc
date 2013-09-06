#include <ncurses.h>
#include <string>

const std::string title = "SNAKE";
const std::string test = "WHOA";

void configure_stdscr();
void configure_colors();

int main()
{
	// TODO: For testin the keypad() behavior
	char c;

	int row, col;

	// Start curses mode with color
	initscr();
	start_color();

	// Configure the screen we initialized above
	configure_stdscr();
	configure_colors();

	// Store the row and col of stdscr
	getmaxyx(stdscr, row, col);

	// Print title screen in the center
	mvprintw(row/2, (col - title.length())/2, "%s", title.c_str());
	// Test A_STANDOUT
	attrset(A_STANDOUT);
	mvprintw(row/2 + 1, (col - title.length())/2, "%s", title.c_str());
	// Test A_UNDERLINE
	attrset(A_UNDERLINE);
	mvprintw(row/2 + 2, (col - title.length())/2, "%s", title.c_str());
	// Test A_REVERSE
	attrset(A_REVERSE);
	mvprintw(row/2 + 3, (col - title.length())/2, "%s", title.c_str());
	// Test A_BLINK
	attrset(A_BLINK);
	mvprintw(row/2 + 4, (col - title.length())/2, "%s", title.c_str());
	// Test A_BOLD
	attrset(A_BOLD);
	mvprintw(row/2 + 5, (col - title.length())/2, "%s", title.c_str());
	// Test A_UNDERLINE | A_BOLD
	attrset(A_UNDERLINE | A_BOLD);
	mvprintw(row/2 + 6, (col - title.length())/2, "%s", title.c_str());

	// Clear the attributes
	standend();

	// Pause our game, wait for user input
	c = getch();

	// Now print something else to test addstr()
	// Get rid of the title before printing our next string. Note that it doesn't
	// "redraw the screen from scratch" so there's no blinking (i.e. what you'd see with
	// clear())
	erase();
	mvaddstr(row/2, (col - test.length())/2, test.c_str());

	// Pause our game, wait for user input
	c = getch();
	
	// Color the title line
	mvchgat(row/2, 0, -1, A_NORMAL, 1, NULL);
	// Augment the title itself
	mvchgat(row/2, (col - test.length())/2, test.length(), A_BLINK | A_BOLD | A_UNDERLINE, 1, NULL);

	// TODO: Odd, I don't seem to need this anywhere
	//refresh();

	// Pause our game, wait for user input
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

void configure_colors()
{
	// TODO: Hm these need to be bound by COLOR_PAIRS and COLORS
	init_pair(1, COLOR_RED, COLOR_GREEN);
}
