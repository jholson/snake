#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <vector>

enum GameState
{
	Menu,
	InGame,
	// TODO Feature!!!!!!!!!!!
	Scoreboard
};

enum Direction
{
	Up,
	Down,
	Left,
	Right
};

int g_max_row, g_max_col;
GameState g_state;

int menu_mode();
void in_game_mode();
void in_game_mode_core(Direction *curr_direction);
long diff_in_nanoseconds(timespec first, timespec second);
void configure_stdscr();
void configure_colors();

int main()
{
	// Initialize ncurses stuff
	initscr();
	start_color();
	curs_set(0);
	configure_stdscr();
	configure_colors();

	// Initialize game g_state
	getmaxyx(stdscr, g_max_row, g_max_col);
	g_state = Menu;

	while (true)
	{
		switch (g_state)
		{
			case Menu:
			{	
				int ret = menu_mode();
				if (ret < 0)
				{
					// End curses mode
					endwin();

					return 0;
				}

				// Otherwise swap to whatever is the next GameState
				break;
			}
			case InGame:
				in_game_mode();
				break;
			case Scoreboard:
				break;
			default:
				// Something broke
				exit(-1);
		}
	}
}

int menu_mode()
{
	const std::string title_text = "SNAKE";
	const std::string play_text = "Play (p)";
	const std::string exit_text = "Exit (q)";

	// Erase whatever may have been printed on the screen
	erase();

	// Generate and print menu
	mvprintw(g_max_row/2, (g_max_col - title_text.length())/2, "%s", title_text.c_str());
	// Rest of lines should be centered below the title, but left-aligned
	mvprintw(g_max_row/2 + 1, (g_max_col - play_text.length())/2, "%s", play_text.c_str());
	mvprintw(g_max_row/2 + 2, (g_max_col - play_text.length())/2, "%s", exit_text.c_str());

	while (true)
	{
		char c = getch();
		if (c == 'p')
		{
			g_state = InGame;
			return 0;
		}
		else if (c == 'q')
		{
			return -1;
		}

		// Else wait for them to press a valid key
	}
}

void in_game_mode()
{
	// Init game settings
	int curr_row = 0, curr_col = 0;
	Direction curr_direction = Up;
	erase();
	attrset(A_BOLD);

	// They start in the center
	mvaddch(g_max_row/2, g_max_col/2, ACS_BLOCK);

	while (true)
	{
		in_game_mode_core(&curr_direction);
		/* TODO: Move this to in_game_mode_core()
		char c = getch();
		switch (c)
		{
			case UpKeyCode:
				curr_direction = Up; 
				break;
			case DownKeyCode:
				curr_direction = Down; 
				break;
			case LeftKeyCode:
				curr_direction = Left; 
				break;
			case RightKeyCode:
				curr_direction = Right; 
				break;
			default:
				// Wait for them to press a valid key
				break;
		}

		mvprintw(0, 0, "%c", c);

		napms(300);
		*/

		erase();
		mvaddstr(0,0, "napping");
		mvaddstr(g_max_row/2 + 1, g_max_col/2, "curr direction: ");
		switch (curr_direction)
		{
			case Up:
				addstr("Up");
				break;
			case Down:
				addstr("Down");
				break;
			case Left:
				addstr("Left");
				break;
			case Right:
				addstr("Right");
				break;
			default:
				// Shit broke
				return;
		}
		refresh();
		napms(500);
	}
}

void in_game_mode_core(Direction *curr_direction)
{
	const int NANOSECONDS_PER_MILLISECOND = 1000000;
	// In nanoseconds
	long refresh_span = 500 * NANOSECONDS_PER_MILLISECOND;

	// We don't want getch() to be blocking here
	nodelay(stdscr, true);

	int inp;
	struct timespec start, curr;
	clock_gettime(CLOCK_MONOTONIC, &start);
	do
	{
		inp = getch();
		switch (inp)
		{
			// Notice that we set the direction to whatever was the last user input
			// before a "refresh"
			// TODO: Alternatively, have a separate thread for processing user input with 
			// a producer consumer model? Probably overkill
			case KEY_UP:
				*curr_direction = Up; 
				break;
			case KEY_DOWN:
				*curr_direction = Down; 
				break;
			case KEY_LEFT:
				*curr_direction = Left; 
				break;
			case KEY_RIGHT:
				*curr_direction = Right; 
				break;
			default:
				// Wait for them to press a valid key
				break;
		}

		// TODO: Delete most of this
		erase();
		clock_gettime(CLOCK_MONOTONIC, &curr);
		mvprintw(0, 0, "curr: %ld, start: %ld", curr.tv_nsec, start.tv_nsec);
		mvprintw(1, 0, "curr: %ld, start: %ld", curr.tv_sec, start.tv_sec);
		mvprintw(2, 0, "%ld", diff_in_nanoseconds(start, curr));
		mvprintw(3, 0, "%ld", refresh_span);
		if (inp != ERR)
		{
			mvprintw(4, 0, "inp: %c", inp);
		}
		else
		{
			mvaddstr(4, 0, "ERR");
		}
		mvprintw(5, 0, "curr_direction: %d", (int) *curr_direction);
	}
	while (diff_in_nanoseconds(start, curr) < refresh_span);

	// Reset getch() to be blocking
	nodelay(stdscr, false);
}

// TODO: Obviously not the most robust thing
long diff_in_nanoseconds(timespec first, timespec second)
{
	const long NANOSECONDS_PER_SECOND = 1000000000;
	return (second.tv_sec - first.tv_sec) * NANOSECONDS_PER_SECOND + 
		(second.tv_nsec - first.tv_nsec);
}

void configure_stdscr()
{
	// Disable line buffering, but characters are still echo'd to the terminal like 
	// ctrl-z
	cbreak();

	// Don't echo user input back to the screen. We'll print it if we need to
	noecho();

	// Enables  function keys (e.g. F1) and arrow keys to be treated as normal chars instead
	// of special chars that need "escaping"
	keypad(stdscr, TRUE);
}

void configure_colors()
{
	// TODO: Hm the first argument needs to be bound by COLOR_PAIRS. The latter need to be bound 
	// COLORS if we decide not to use the 8 built in color macros
	init_pair(1, COLOR_RED, COLOR_GREEN);
}
