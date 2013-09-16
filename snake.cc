#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <deque>

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

struct SnakeNode
{
	int x, y;

	SnakeNode(int row, int col) : x(row),
	                              y(col)
	{ }
};

int g_max_row, g_max_col;
GameState g_state;

int menu_mode();
void in_game_mode();
int in_game_mode_core(Direction *curr_direction);
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

				// Transition to menu_mode()
				g_state = Menu;
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
	int curr_row = g_max_row/2, curr_col = g_max_col/2;
	Direction curr_direction = Up;
	std::deque<SnakeNode> snake;
	snake.push_front (SnakeNode(curr_row, curr_col));
	erase();
	attrset(A_BOLD);

	// They start in the center
	mvaddch(g_max_row/2, g_max_col/2, ACS_BLOCK);

	while (true)
	{
		if (in_game_mode_core(&curr_direction) < 0)
		{
			return;
		}

		// Update snake
		snake.pop_back();
		switch (curr_direction)
		{
			case Up:
				curr_row--;
				break;
			case Down:
				curr_row++;
				break;
			case Left:
				curr_col--;
				break;
			case Right:
				curr_col++;
				break;
			default:
				// Shit broke
				return;
		}
		snake.push_front(SnakeNode(curr_row, curr_col));

		erase();

		// Print the snake
		mvaddch(snake.front().x, snake.front().y, ACS_BLOCK);

		// Print some diagnostics
		mvaddstr(0, 0, "napping");
		mvaddstr(1, 0, "curr direction: ");
		switch (curr_direction)
		{
			case Up:
				addstr("Up\n");
				break;
			case Down:
				addstr("Down\n");
				break;
			case Left:
				addstr("Left\n");
				break;
			case Right:
				addstr("Right\n");
				break;
			default:
				// Shit broke
				return;
		}
		mvprintw(2, 0, "col: %d, row: %d\n", curr_col, curr_row);

		refresh();

		// Uncomment to view the diagnostics in this method
		//napms(100);
	}
}

int in_game_mode_core(Direction *curr_direction)
{
	const int NANOSECONDS_PER_MILLISECOND = 1000000;
	// In nanoseconds
	long refresh_span = 100 * NANOSECONDS_PER_MILLISECOND;

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
			case 'q':
				return -1;
			default:
				// Wait for them to press a valid key
				break;
		}

		// TODO: Delete most of this
		clock_gettime(CLOCK_MONOTONIC, &curr);
		// Note that a newline in these format strings seems to automatically call clrtoeol()
		mvprintw(0, 0, "curr: %ld, start: %ld\n", curr.tv_nsec, start.tv_nsec);
		mvprintw(1, 0, "curr: %ld, start: %ld\n", curr.tv_sec, start.tv_sec);
		mvprintw(2, 0, "%ld\n", diff_in_nanoseconds(start, curr));
		mvprintw(3, 0, "%ld\n", refresh_span);
		mvprintw(4, 0, "curr_direction: %d\n", (int) *curr_direction);
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
