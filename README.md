# SudokuSolver
A basic, slightly optimised brute force Sudoku solver.

## Building:

I'm yet to make a proper makefile, but for the meantime the code can be compiled simply with

`gcc --ansi -Wall -O3 main.c -o sudoku.o`

## Running

Either run the application with `./sudoku.o` and type in the game board, or pipe in a board and pipe out the output, eg:

`cat hard.txt | ./sudoku.o > hard.output.txt`
