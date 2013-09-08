#include <ncurses.h>
#include <string>
#include <vector>

const std::string title = "SNAKE";
const std::string test = "WHOA";

void configure_stdscr();
void configure_colors();

int main()
{
	char c;
	std::string s;
	std::vector<char> writable_s(s.size() + 1);
	// Calling C libaries from C++ is insane. I'm unconvinced that it could be any more
	// complicated to convert a string to a writeable char *
	std::copy(s.begin(), s.end(), writable_s.begin());

	int row, col;

	// Start curses mode with color
	initscr();
	start_color();

	// Configure the screen we initialized above
	configure_stdscr();
	configure_colors();

	// Store the row and col of stdscr
	getmaxyx(stdscr, row, col);

	while (1)
	{
		erase();
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

		refresh();
		// Sleep for 500ms
		napms(500);
		// Get rid of the title before printing our next string. Note that it doesn't
		// "redraw the screen from scratch" so there's no blinking (i.e. what you'd see with
		// clear())
		erase();

		// Now print something else to test addstr()
		mvaddstr(row/2, (col - test.length())/2, test.c_str());

		refresh();
		napms(500);

		// Color the title line
		mvchgat(row/2, 0, -1, A_NORMAL, 1, NULL);
		// Augment the title itself
		mvchgat(row/2, (col - test.length())/2, test.length(), 
			A_BLINK | A_BOLD | A_UNDERLINE, 1, NULL);

		// Test chgat()
		mvchgat(0, 0, -1, A_BLINK, 1, NULL);
		mvchgat(row-1, 0, -1, A_BLINK, 1, NULL);

		// Note that we don't need refresh() if we fetch user input, interestingly...
		c = getch();
		if (c == 'q')
		{
			break;
		}

		erase();
	}

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
	// TODO: Hm the first argument needs to be bound by COLOR_PAIRS. The latter need to be bound 
	// COLORS if we decide not to use the 8 built in color macros
	init_pair(1, COLOR_RED, COLOR_GREEN);
}
