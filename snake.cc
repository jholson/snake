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
	Direction curr_direction;
	int food_row, food_col;
};

// Globals
int g_max_row, g_max_col;
GameStatus g_status;

int menu_mode();
void in_game_mode();
int in_game_core(GameState &game_state);
int process_input(int inp, GameState &game_state);
long diff_in_nanoseconds(timespec first, timespec second);
void generate_food(GameState &game_state);
void print_game(GameState &game_state);
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
	GameState game_state =
	{
		std::deque<SnakeNode>(), // snake
		std::vector<std::vector<BoardCellType> >(
			g_max_row,
			std::vector<BoardCellType>(g_max_col, Empty)), // board
		Up, // curr_direction
		// TODO: Temporarily hard-coded for testing
		30, // food_col
		30 // food_row
	};
	// Start the snake of size 1 in the middle of the board
	game_state.snake.push_front(SnakeNode(g_max_col/2, g_max_row/2));

	erase();
	attrset(A_BOLD);

	// This uses the same style as the ncurses window so (x, y) <-> [x][y] <-> (col, row)
	game_state.board[game_state.food_col][game_state.food_row] = Food;
	game_state.board[game_state.snake.front().col][game_state.snake.front().row] = Snake;

	while (true)
	{
		if (in_game_core(game_state) < 0)
		{
			// Game ended
			return;
		}

		// Print some diagnostics
		mvprintw(0, 0, "col: %d, row: %d\n", game_state.snake.front().col, 
			game_state.snake.front().row);
		mvprintw(1, 0, "curr_direction: %d\n", game_state.curr_direction);

		refresh();

		// Uncomment to view the diagnostics in this method
		//napms(100);
	}
}

int in_game_core(GameState &game_state)
{
	const int NANOSECONDS_PER_MILLISECOND = 1000000;
	// In nanoseconds
	long refresh_duration = 100 * NANOSECONDS_PER_MILLISECOND;

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
	if (process_input(last_dir_inp, game_state) < 0)
	{
		return -1;
	}

	// Finally draw the game
	print_game(game_state);

	return 0;
}

// TODO: Obviously not the most robust thing
long diff_in_nanoseconds(timespec first, timespec second)
{
	const long NANOSECONDS_PER_SECOND = 1000000000;
	return (second.tv_sec - first.tv_sec) * NANOSECONDS_PER_SECOND + 
		(second.tv_nsec - first.tv_nsec);
}

bool update_food(GameState &game_state)
{
	// TODO:
	return true;
}

int process_input(int inp, GameState &game_state)
{
	// Map input to a direction
	switch (inp)
	{
		case KEY_DOWN:
			game_state.curr_direction = Down;
			break;
		case KEY_UP:
			game_state.curr_direction = Up;
			break;
		case KEY_LEFT:
			game_state.curr_direction = Left;
			break;
		case KEY_RIGHT:
			game_state.curr_direction = Right;
			break;
		default:
			// No input was entered so the curr_direction stays the same
			break;
	}

	// Move the snake
	int curr_col = game_state.snake.front().col;
	int curr_row = game_state.snake.front().row;
	switch (game_state.curr_direction)
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
	    // Short circuit is critical here due to index out of bounds
	    (game_state.board[curr_row][curr_col] == Snake))
	{
		// They lost
		return -1;
	}
	game_state.snake.push_front(SnakeNode(curr_col, curr_row));
	game_state.board[game_state.snake.front().col][game_state.snake.front().row] = Snake;

	// Update the tail based on whether the snake ate food
	if (game_state.snake.front().col == game_state.food_col &&
		game_state.snake.front().row == game_state.food_row)
	{
		// Then we ate food, so we "grow" the snake by... not making it shorter
		
		// Now generate new food
		generate_food(game_state);
	}
	else
	{
		game_state.board[game_state.snake.back().col][game_state.snake.back().row] = Empty;
		game_state.snake.pop_back();
	}
	
	return 0;
}

void generate_food(GameState &game_state)
{
	// TODO: Since the board is small we can actually enumerate the entire thing. Since the
	// snake could theoretically cover (almost) the entire board it doesn't seem asymptotically
	// possible to improve the time. It could be feasible to store the empty spots in a list
	// or perhaps store the number of empty cells in the rectangle from (0,0) to a coordinate
	// (x,y) and then use some sort of binary decision tree to pick an arbitrary empty spot
	std::vector<std::pair<int, int> > empty_cells;
	for (int col = 0; col < g_max_col; col++)
	{
		for (int row = 0; row < g_max_row; row++)
		{
			if (game_state.board[col][row] == Empty)
			{
				empty_cells.push_back(std::pair<int, int>(col, row));
			}
		}
	}
}

void print_game(GameState &game_state)
{
	erase();
	for (std::deque<SnakeNode>::iterator it = game_state.snake.begin();
		it != game_state.snake.end(); it++)
	{
		mvaddch((*it).row, (*it).col, ACS_BLOCK);
	}

	mvaddch(game_state.food_col, game_state.food_row, ACS_PI);
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
