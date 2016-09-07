/* Recursive Sudoku Solver (v2)
 * Written by Ryan Crosby - August 2013
 * Improvements upon last version:
 * + New bitwise logic only loops over possible values
 * + Added empty cell lookup table to remove excessive looping
 * + Added precheck to determine if board is unsolvable or already solved
 * + Solves ~10 times faster than old version
 *
 * Also yes, this is ANSI C89 complaint.
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_SOLUTIONS	10

#define EMPTY_CELL_INPUT_VAL	0

#define BOARD_WIDTH		9
#define BOARD_HEIGHT	9
#define SQUARE_WIDTH	3
#define SQUARE_HEIGHT	3

#define FALSE			0
#define TRUE			1

typedef struct {
	int x;
	int y;
} point_t;

typedef struct {
	int board_array[BOARD_HEIGHT][BOARD_WIDTH]; /* [y][x] */
} board_t;

typedef struct {
	point_t board_array[BOARD_HEIGHT][BOARD_WIDTH];
} point_board_t;

int solve_board(board_t*);
void solve_cell(board_t*, point_board_t*, point_t, int*);
point_board_t find_all_empty_cells(board_t*);
point_t find_next_empty_cell(point_board_t*, point_t);
int int_to_bit_rep(int);
int bit_rep_to_int(int);
int find_possible_values(board_t*, point_t);
void print_board(board_t*);
int check_board(board_t*);


int main() {
	board_t game_board;
	/* get the board to be solved */
	int board_ok = FALSE;
	int i;
	int j;
	int solutions_found;

	printf("\nPlease enter Sudoku board now, left to right, top to bottom.\n"
		"Separate each cell with a space.\n"
		"Use %d for empty/unknown cells\n\n", EMPTY_CELL_INPUT_VAL);

	while(board_ok != TRUE) {
		board_ok = TRUE;
		for(j = 0; j < BOARD_HEIGHT; j++) {
			for(i = 0; i < BOARD_WIDTH; i++) {
				int val = -1;
				int vals_read = scanf("%d", &val);
				/* internally force empty cells to zero */
				if(val == EMPTY_CELL_INPUT_VAL) val = 0;
				if((vals_read == 1)
					&& ((val >= 0) && (val <= 9))) {
						game_board.board_array[j][i] = int_to_bit_rep(val);
				}
				else {
					board_ok = FALSE;
					break;
				}
			}
			if(board_ok != TRUE) break;
		}
		if(board_ok != TRUE) {
			printf("\nInvalid game board, please check all cells"
				" and try again:\n\n");
		}
	}

	printf("\nBoard OK!");
	printf("\nInput:\n\n");
	print_board(&game_board);
	printf("\nSolving ...\n");
	/* solve_board prints out all solutions internally */
	solutions_found = solve_board(&game_board);

	printf("\nDone, %d solutions found.\n", solutions_found);

	return EXIT_SUCCESS;
}

int solve_board(board_t* board) {
	/* strategy: find all possible values for this cell
	plug in first possible value, call next cell
	after next cell returns, plug in next value
	if no values remain, return
	if no values to start with, return */
	point_t origin_cell =  {0, 0};
	point_t first_cell;
	/* each empty cell has a point which describes the next empty cell along */
	point_board_t empty_cell_map;
	int solutions_found = 0;
	int board_check_result;

	empty_cell_map = find_all_empty_cells(board);
	if(board->board_array[0][0] == 0) {
		first_cell.x = 0;
		first_cell.y = 0;
	}
	else {
		first_cell = find_next_empty_cell(&empty_cell_map, origin_cell);
	}

	/* check if this board is already solved,
	or if it is unsolvable */
	board_check_result = check_board(board);

	if(board_check_result == 1) {
		/* board is already complete */
		solutions_found = 1;
		printf("\nSolution %d:\n\n", solutions_found);
		print_board(board);
		return solutions_found;
	}
	else if(board_check_result == 2) {
		/* board cannot be solved */
		return solutions_found;
	}
	/* board check returned 0, board can be solved
	and hasn't been*/

	/* start the recursive solve */
	solve_cell(board, &empty_cell_map, first_cell, &solutions_found);

	return solutions_found;
}

void solve_cell(board_t* board, point_board_t* empty_cell_map, point_t current_cell, int* solutions_found) {
	int possible_vals = find_possible_values(board, current_cell);
	point_t next_cell = find_next_empty_cell(empty_cell_map, current_cell);
	int current_search;

	for(current_search = 0x0001; current_search <= 0x0100; current_search = current_search << 1) {
		if(possible_vals & current_search) {
			/* this current_search is a possible val,
			lock it into the board and call the next cell */
			board->board_array[current_cell.y][current_cell.x] = current_search;

			/* no more cells, this must be the last one */
			if((next_cell.x == -1) && (next_cell.y == -1)) {
					/* solution found */
					(*solutions_found)++;
					printf("\nSolution %d:\n\n", *solutions_found);
					print_board(board);
					if(*solutions_found >= MAX_SOLUTIONS) {
						printf("\nSolve stopped after %d solutions\n", *solutions_found);
						exit(EXIT_SUCCESS);
					}
			}
			else {
				solve_cell(board, empty_cell_map, next_cell, solutions_found);
			}
		}
	}

	/*set this cell back to zero and return */
	board->board_array[current_cell.y][current_cell.x] = 0x0000;
	return;
}

point_board_t find_all_empty_cells(board_t* value_board) {
	point_board_t cell_link_board;
	int i;
	int j;
	point_t last_empty_cell = {-1, -1};
	/* search backwards through board */
	for(j = BOARD_HEIGHT - 1; j >= 0; j--) {
		for(i = BOARD_WIDTH - 1; i >= 0; i--) {
			if(value_board->board_array[j][i] == 0) {
				cell_link_board.board_array[j][i] = last_empty_cell;
				last_empty_cell.x = i;
				last_empty_cell.y = j;
			}
			else {
				cell_link_board.board_array[j][i] = last_empty_cell;
			}
		}
	}
	return cell_link_board;
}

point_t find_next_empty_cell(point_board_t* empty_link_board, point_t start) {
	return empty_link_board->board_array[start.y][start.x];
}

void print_board(board_t* board) {
	point_t this_point;
	for(this_point.y = 0; this_point.y < BOARD_HEIGHT; this_point.y++) {
		for(this_point.x = 0; this_point.x < BOARD_WIDTH; this_point.x++) {
			int this_cell_val = 
				bit_rep_to_int(board->board_array[this_point.y][this_point.x]);
			printf("%d ", this_cell_val);
		}
		printf("\n");
	}
}

int int_to_bit_rep(int input) {
	if(input) {
		return 1 << (input -1);
	}
	return 0;
}

int bit_rep_to_int(int input) {
	int output = 0;
	while(input >= 1) {
		output++;
		input = input >> 1;
	}
	return output;
}

/* returns 0 for not solved, 1 for solved, 2 for unsolvable */
int check_board(board_t* board) {
	int board_solved = TRUE;
	/* check if a solved board is valid or not */
	point_t this_point;
	for(this_point.y = 0; this_point.y < BOARD_HEIGHT; this_point.y++) {
		for(this_point.x = 0; this_point.x < BOARD_WIDTH; this_point.x++) {
			int possible_vals = find_possible_values(board, this_point);
			if(possible_vals == 0) {
				/* no number is valid in this cell,
				therefore board cannot be solved */
				return 2;
			}
			else if(board->board_array[this_point.y][this_point.x] == 0) {
				/* cell is empty therefore board is not solved yet */
				board_solved = FALSE;
			}
			else if((board->board_array[this_point.y][this_point.x] & possible_vals) == 0) {
				/* specified value cannot work in current location, board cannot be solved */
				return 2;
			}
		}
	}
	if(board_solved == TRUE) {
		return 1;
	}
	else {
		return 0;
	}
}

int find_possible_values(board_t* board, point_t cell_loc) {
	/* each bit represents a possible value for the cell
	with the least sig bit representing 1 and the 9th
	least sig bit representing 9.
	eg. 100001010 represents 9, 4, and 2. */

	int possible_vals = 0x01FF; /* 0000000111111111, ie 1-9*/

	point_t square_loc; /* top left position of current "square" */
	/* used for loops */
	int x_search;
	int y_search;

	/* find square_loc */
	square_loc.x = cell_loc.x;
	square_loc.y = cell_loc.y;
	while((square_loc.x % SQUARE_WIDTH) != 0) square_loc.x--;
	while((square_loc.y % SQUARE_HEIGHT) != 0) square_loc.y--;

	for(y_search = square_loc.y;
		y_search < (square_loc.y + SQUARE_HEIGHT);
		y_search++) {
			for(x_search = square_loc.x;
				x_search < (square_loc.x + SQUARE_WIDTH);
				x_search++) {
					if((x_search == cell_loc.x) && (y_search == cell_loc.y)) {
						continue;
					}
					possible_vals = possible_vals & ~board->board_array[y_search][x_search];
			}
	}

	/* move down the column eliminating all values seen */
	for(y_search = 0; y_search < BOARD_HEIGHT; y_search++) {
		if(y_search == cell_loc.y) {
			continue;
		}
		possible_vals = possible_vals & ~board->board_array[y_search][cell_loc.x];
	}

	/* move across the row eliminating all values seen */
	for(x_search = 0; x_search < BOARD_WIDTH; x_search++) {
		if(x_search == cell_loc.x) {
			continue;
		}
		possible_vals = possible_vals & ~board->board_array[cell_loc.y][x_search];
	}

	return possible_vals;
}
