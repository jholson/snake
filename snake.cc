#include <ncurses.h>
#include <string>
#include <vector>

enum GameState
{
	Menu,
	InGame,
	// TODO Feature!!!!!!!!!!!
	Scoreboard
};

int g_row, g_col;
GameState g_state;

void menu_mode();
void configure_stdscr();
void configure_colors();

int main()
{
	// Initialize ncurses stuff
	initscr();
	start_color();
	configure_stdscr();
	configure_colors();

	// Initialize game g_state
	getmaxyx(stdscr, g_row, g_col);
	g_state = Menu;

	menu_mode();

	// End curses mode
	endwin();

	return 0;
}

void menu_mode()
{
	const std::string title_text = "SNAKE";
	const std::string play_text = "Play (p)";
	const std::string exit_text = "Exit (q)";

	// Erase whatever may have been printed on the screen
	erase();

	// Generate and print menu
	mvprintw(g_row/2, (g_col - title_text.length())/2, "%s", title_text.c_str());
	// Rest of lines should be centered below the title, but left-aligned
	mvprintw(g_row/2 + 1, (g_col - play_text.length())/2, "%s", play_text.c_str());
	mvprintw(g_row/2 + 2, (g_col - play_text.length())/2, "%s", exit_text.c_str());

	while (true)
	{
		char c = getch();
		if (c == 'p')
		{
			// TODO: Implement in_game_mode()
			return;
		}
		else if (c == 'q')
		{
			return;
		}

		// Else wait for them to press a valid key
	}
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
