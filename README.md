# Monster Sudoku Generator and Solver
This is a re-write of the final project of CS 171: Intro to Artificial Intelligence at UCI. MSS is written in C++11.

This will generate puzzles randomly, or from text files, and solve puzzles. This will also log and keep track of statistics- time taken by individual algorithms, nodes searched, dead ends, etc.

This also has a number of heuristic algorithms that can be enabled or disabled. This is purely to experiment- see what combination of heuristics performs the best over different sized puzzles. The heuristics are as follows:

  - Minimum remaining values
  
  - Least constraining value
  
  - Max degrees
  
  - Regular constraint propagation
  
  - Arc consistency check
  
  - Forward check

## This is a two-part program: a sudoku puzzle generator and solver.

### Generator

The generator generates a random sudoku puzzle based on four integers: m, n, p, q. The puzzle is not guaranteed to be solvable, nor to have a unique solution. It only guarantees that the initially solved cells are all legal.
 
  - m: m is the number of clues, or initially solved cells. m should be at least 1 (else the puzzle is easily solvable and is of no use), and should be less than n^2 (else the puzzle is already solved and is of no use). Be careful with m, as having it too low compared to n^2 will cause the puzzle to have multiple solutions, and having it too high will cause the generator to potentially timeout. For the standard 9x9 puzzle, m = 17 is the minimum number of clues for a puzzle to have a unique solution.
  
  - n: n is the size of the puzzle. The standard size for a sudoku puzzle is n = 9. Note that larger n's will generate larger puzzles, and thus, will take much longer to solve.
  
  - p: p is the height of each block. The product p * q must equal n. This is a requirement of a sudoku puzzle.
  
  - q: q is the width of each block.

### Solver

The solver will use backtracking search and the enabled heuristics to solve the given puzzle.

### Input File
The input file is expected to be in a specific format:

  - The first line must have four numbers, delimited by a single space. These are the m, n, p, q parameters.
  
  - Then it expects n lines, each with n characters. Denote empty cells with '.' and solved cells with the respective value.
  
  - For n <= 9 puzzles, the characters should be 1 - 9. For anything higher (up to 35), use capital letters with A being 10, B being 11, etc.
  
  - You can put extra lines after, but those will be ignored. Same with extra characters after the first n per line (except the first).
  
  -Except for the first line, DO NOT put spaces in between characters.

### To use:

1. Download the MonsterSudoku folder.

2. To compile, run g++ -std=c++11 -static -o main with all of the source files. You can name the -o any name you wish.

3. Run the .o you just created: ./main (or whatever you named the .o).

4. The program runs in a loop. You enter commands to either generate or solve. Type "help" to see how to use the commands.

### Example:

To generate a puzzle: gen m n p q file_path. This will generate a random puzzle based on the m, n, p, q parameters and save it to a text file at file_path.

To solve a puzzle: solve file_path [-v]. This will load up a puzzle from the file_path and solve it. The -v optional argument will give you a detailed report at the end.

Alternatively, you can also do solve -g m n p q [-v]. This will first randomly generate a puzzle based on the m, n, p, q parameters , and then solve it.
