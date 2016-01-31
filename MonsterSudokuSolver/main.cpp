#include <iostream>
#include "sudokureader.h"
#include "sudokusolver.h"
using namespace std;
//X means done, -- means partially done, ? means confused
//TODO: Create Logs for Generator X
//Implement Command Line Arguments --
//Create Backtracking Search X
//Add Timers for Search x
//Add Timeout for Generator X
//                Search x
//Create Backtracking Log x

//TODO: Copy Constructors for SudokuReader and SudokuSolver are incomplete (requires copy array)
//Note:
//To turn in as LastName_UCINUMBERID_TeamName.zip, w/ folders:
//bin - Single executable or main script
//src - Source Code
//doc - Reports, Progress Reports for every submission, Final Report

//Input Parameters
//Command Line Arguments, input_file_name, output_file_name, timeout in seconds, options
//Options are GEN, BT, FC, ACP, MAC, MVC, DH, LCV

//Generator should
//If inputs are invalid, output text file with a single line: error: invalid input parameters
//If time out, output text file with a single line: timeout
//If success, output problem file

//Backtracking Serach should
//PRoduce log file:
//TOTAL_START - starting time in sec
//PREPROCESSING_START - ACP start time in sec
//PREPROCESSING_DONE - ACP finish time in sec
//SEARCH_START - Backtrack start time in sec
//SEARCH_DONE - Backtrack finish time in sec
//SOLUTION_TIME - ACP + Serach time in sec
//STATUS = success or timeout or error
//SOLUTION - assignment (Var:Value)
//SOLUTION - (0,0,0,0...) if no solution
//COUNT_NODES - Number of nodes (variables)
//COUNT_DEADENDS - Number of backtracks
//Terminate the program if a solution is found before timeout

//Main Flow
//Grab arguments from command line
//Turn on options flags
//Generate sudoku from input file
//Save to output file
//Solve sudoku puzzle
//Generate log for solution
//If at any time the runtime is longer than timeout, then end for generation and for solving


void print (char** sudoku, int n, int p, int q, char e) {
    if (e != Error::kInputError || e != Error::kTimeoutError) {
        for (std::size_t x = 0; x < n; ++x) {
            std::cout << std::endl;
            if (x % p == 0) {
                for (std::size_t i = 0; i < 2 * n + q - 1; ++i) {
                    cout << "-";
                }
                std::cout << std::endl;
            }
            for (std::size_t y = 0; y < n; ++y) {
                if (y % q == 0)
                    cout << "|";
                if (sudoku[x][y] == kInitialToken)
                    cout << "  ";
                else
                    std::cout << sudoku[x][y] << " ";
            }

        }
        std::cout << std::endl;
    }
    else {
        cout << "ERROR" << endl;
    }

}

int main(int argc, char *argv[]) {
    static  char alpha[] = { 49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90};
    int m,n,p,q;
    long timeout = 300;
    char* inputFileName = "input.txt";
    char* outputFileName = "output.txt";
    char* logFileName = "BTlog.txt";
    vector<char*> options;
    if (argc >= 3) {
        inputFileName = argv[0];
        outputFileName = argv[1];
        timeout = atol(argv[2]);
        int opNum = argc - 3;
        for (std::size_t i = 0; i < opNum; ++i) {
            options[i] = argv[3 + i];
        }
    }

//    SudokuReader R (inputFileName,outputFileName); //Change
    SudokuReader R (inputFileName,"");
//    cout << R.getInputFileName() << endl;
    char** sudoku = R.loadSudoku(m,n,p,q,timeout);
    cout << "Sudoku Puzzle with parameters:\n"
         << "m: " << m << endl
         << "n: " << n << endl
         << "p: " << p << endl
         << "q: " << q << endl;
    char e = **sudoku;
//    R.saveSudoku(m,n,p,q,sudoku,e);
    if (e != Error::kInputError && e != Error::kTimeoutError) {
        for (std::size_t x = 0; x < n; ++x) {
            std::cout << std::endl;
            if (x % p == 0) {
                for (std::size_t i = 0; i < 2 * n + q - 1; ++i) {
                    cout << "-";
                }
                std::cout << std::endl;
            }
            for (std::size_t y = 0; y < n; ++y) {
                if (y % q == 0)
                    cout << "|";
                if (sudoku[x][y] == kInitialToken)
                    cout << "  ";
                else
                    std::cout << sudoku[x][y] << " ";
            }

        }
        std::cout << std::endl;
    }

    const clock_t start = clock ();
    char** b = R.loadSudoku(m,n,p,q,timeout);
    SudokuSolver S (alpha);
    try {
//        b = S.solveSudoku(b,m,n,p,q,timeout,logFileName);
        b = S.solveSudoku(b,m,n,o,q,timeout,outputFileName);
    }
    catch (char e) {
        cout << "TIME OUT" << endl;
    }

    print (b,n,p,q,**b);
    cout << (clock () - start) / CLOCKS_PER_SEC << endl;
    return 0;
}
//Parameters: M is the number of preset cells. N is the size of the sudoku. P is the row size per block. Q is the column size per block.
//Pre-Constraints: PxQ = N. M <= N^2. Tokens must be the first N characters from 1-9, A-Z
//Constraints: AllDif - Tokens must be unique to its row, its column, and its block.
/* Problem Reader and Generator
 * GENERATING (M,N,P,Q,output file name)
 * -Begin with an NxN array initialized to ALL zero.
 * -For 0 to M: Randomly choose an empty (zero) cell.
 *  -Randomly choose a value for that cell.
 *  -Check if that value satisfies all of that cell's constraints.
 *  -If it does not satisfy the constraints, then remove that value from the domain, and choose a different value.
 *  -If there are no more values in the domain, then fail globally and restart from the freshly-initialized NxN array.
 *  -If it does succeed, choose the next random cell.
 *  -Once this occurs M times, the sudoku puzzle is generated.
 *  -Output the file as M N P Q on first line, then the puzzle as an array, with spaces in between each cell.
 * READING (input file name)
 * -Read in a file with file name input.
 * -First line is N P Q
 * -After is the sudoku puzzle
 */

/* Solver
 * -Backtracking Search
 *  -Use MRV, DH, and LCV heuristics
 *  -Use Forward Checking, Book Keeping, and Arc Consistency
 *  -Local Search and MinConflicts Heuristic
 *  -Hardest M Analysis
 *
 */
