#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <deque>
#include <vector>

enum GameStatus
{
	Menu,
	InGame,
	// TODO Feature!!!!!!!!!!!
	Scoreboard
};

enum Direction
{
	Down,
	Up,
	Left,
	Right
};

struct SnakeNode
{
	int col, row;

	SnakeNode(int col, int row) : col(col),
	                              row(row)
	{ }
};

enum BoardCellType
{
	Empty = 0,
	Snake = 1,
	Food = 2
};

struct GameState
{
	std::deque<SnakeNode> snake;
	std::vector<std::vector<BoardCellType> > board;
	int food_row, food_col;
};

// Globals
int g_max_row, g_max_col;
GameStatus g_status;

int menu_mode();
void in_game_mode();
int in_game_core(std::deque<SnakeNode> &snake, std::vector<std::vector<BoardCellType> > &board,
	Direction &curr_direction);
int process_input(int inp, std::deque<SnakeNode> &snake,
	std::vector<std::vector<BoardCellType> > &board, Direction &curr_direction);
long diff_in_nanoseconds(timespec first, timespec second);
void print_board(std::deque<SnakeNode> &snake);
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

	// Initialize game g_status
	getmaxyx(stdscr, g_max_row, g_max_col);
	g_status = Menu;

	while (true)
	{
		switch (g_status)
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

				// Otherwise swap to whatever is the next GameStatus
				break;
			}
			case InGame:
				in_game_mode();

				// Transition to menu_mode()
				g_status = Menu;
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
			g_status = InGame;
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
	// TODO: Wrap this in a game state struct
	Direction curr_direction = Up;
	// We need an extra row/col because of how we deal with board edge detection
	std::vector<std::vector<BoardCellType> > board(
		g_max_row + 1, std::vector<BoardCellType>(g_max_col + 1, Empty));
	std::deque<SnakeNode> snake;
	snake.push_front(SnakeNode(g_max_col/2, g_max_row/2));
	// TODO: Temporarily hard-coded for testing
	int food_col = 30, food_row = 30;
	erase();
	attrset(A_BOLD);

	// This uses the same style as the ncurses window so (x, y) <-> [x][y] <-> (col, row)
	board[food_col][food_row] = Food;
	board[snake.front().col][snake.front().row] = Snake;

	while (true)
	{
		if (in_game_core(snake, food_col, food_row, board, curr_direction) < 0)
		{
			// Game ended
			return;
		}

		// Print some diagnostics
		mvprintw(0, 0, "col: %d, row: %d\n", snake.front().col, snake.front().row);
		mvprintw(1, 0, "curr_direction: %d\n", curr_direction);

		refresh();

		// Uncomment to view the diagnostics in this method
		//napms(100);
	}
}

int in_game_core(std::deque<SnakeNode> &snake, std::vector<std::vector<BoardCellType> > &board, 
	Direction &curr_direction)
{
	const int NANOSECONDS_PER_MILLISECOND = 1000000;
	// In nanoseconds
	long refresh_duration = 100 * NANOSECONDS_PER_MILLISECOND;

	// Process snake before the "round" starts (basically, check for collision with food)
	bool extend_snake = update_food(snake, board);

	// Next the "round" occurs which consists of reading user input until refresh_duration has
	// elapsed
	// We don't want getch() to be blocking here
	nodelay(stdscr, true);
	int inp, last_dir_inp = 0;
	struct timespec start, curr;
	clock_gettime(CLOCK_MONOTONIC, &start);
	do
	{
		inp = getch();
		if (inp == 'q')
		{
			return -1;
		}

		// Update last_dir_inp if they pressed a key corresponding to a direction
		switch (inp)
		{
			case KEY_DOWN:
			case KEY_UP:
			case KEY_LEFT:
			case KEY_RIGHT:
				last_dir_inp = inp;
				break;
			default:
				break;
		}

		// Print diagnostics
		clock_gettime(CLOCK_MONOTONIC, &curr);
		mvprintw(0, 0, "curr: %ld, start: %ld\n", curr.tv_nsec, start.tv_nsec);
		mvprintw(1, 0, "curr: %ld, start: %ld\n", curr.tv_sec, start.tv_sec);
		mvprintw(2, 0, "%ld\n", diff_in_nanoseconds(start, curr));
		mvprintw(3, 0, "%ld\n", refresh_duration);
		mvprintw(4, 0, "inp: %d\n", inp);
	}
	while (diff_in_nanoseconds(start, curr) < refresh_duration);
	// Reset getch() to be blocking
	nodelay(stdscr, false);

	// Now we're done with the main portion of "round", process the results of it
	if (process_input(last_dir_inp, snake, board, curr_direction) < 0)
	{
		return -1;
	}

	// Finally update the board
	print_board(snake);

	return 0;
}

// TODO: Obviously not the most robust thing
long diff_in_nanoseconds(timespec first, timespec second)
{
	const long NANOSECONDS_PER_SECOND = 1000000000;
	return (second.tv_sec - first.tv_sec) * NANOSECONDS_PER_SECOND + 
		(second.tv_nsec - first.tv_nsec);
}

bool update_food(std::deque<SnakeNode> &snake, std::vector

int process_input(int inp, std::deque<SnakeNode> &snake,
	std::vector<std::vector<BoardCellType> > &board, Direction &curr_direction)
{
	// Update curr_direction
	switch (inp)
	{
		case KEY_DOWN:
			curr_direction = Down;
			break;
		case KEY_UP:
			curr_direction = Up;
			break;
		case KEY_LEFT:
			curr_direction = Left;
			break;
		case KEY_RIGHT:
			curr_direction = Right;
			break;
		default:
			// No input was entered so the curr_direction stays the same
			break;
	}

	// Update snake
	// Tail first
	board[snake.back().col][snake.front().row] = Empty;
	snake.pop_back();

	// Now the head
	int curr_col = snake.front().col;
	int curr_row = snake.front().row;
	switch (curr_direction)
	{
		case Down:
			curr_row++;
			break;
		case Up:
			curr_row--;
			break;
		case Left:
			curr_col--;
			break;
		case Right:
			curr_col++;
			break;
		default:
			// Game broke
			return -1;
	}
	// Check if a collision occurred
	// Note the short circuit logic
	if ((curr_row < 0 || curr_row > g_max_row) ||
	    (curr_col < 0 || curr_col > g_max_col) ||
	    (board[curr_row][curr_col] == Snake))
	{
		// They lost
		return -1;
	}
	snake.push_front(SnakeNode(curr_col, curr_row));
	// Update the board
	board[snake.front().col][snake.front().row] = Snake;

	return 0;
}

void print_board(std::deque<SnakeNode> &snake)
{
	erase();
	for (std::deque<SnakeNode>::iterator it = snake.begin(); it != snake.end(); it++)
	{
		mvaddch((*it).row, (*it).col, ACS_BLOCK);
	}

	// TODO: Testing out food
	mvaddch(30, 30, ACS_PI);
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
