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
int main(int argc, char *argv[]) {
//    char* inputFiles[61] = {
//        "input.txt",
//        "PE1.txt",
//        "PE2.txt",
//        "PE3.txt",
//        "PE4.txt",
//        "PE5.txt",
//        "PE6.txt",
//        "PE7.txt",
//        "PE8.txt",
//        "PE9.txt",
//        "PE10.txt",
//        "PE11.txt",
//        "PE12.txt",
//        "PE13.txt",
//        "PE14.txt",
//        "PE15.txt",
//        "PE16.txt",
//        "PE17.txt",
//        "PE18.txt",
//        "PE19.txt",
//        "PE20.txt",
//        "PE21.txt",
//        "PE22.txt",
//        "PE23.txt",
//        "PE24.txt",
//        "PE25.txt",
//        "PE26.txt",
//        "PE27.txt",
//        "PE28.txt",
//        "PE29.txt",
//        "PE30.txt",
//        "PE31.txt",
//        "PE32.txt",
//        "PE33.txt",
//        "PE34.txt",
//        "PE35.txt",
//        "PE36.txt",
//        "PE37.txt",
//        "PE38.txt",
//        "PE39.txt",
//        "PE40.txt",
//        "PE41.txt",
//        "PE42.txt",
//        "PE43.txt",
//        "PE44.txt",
//        "PE45.txt",
//        "PE46.txt",
//        "PE47.txt",
//        "PE48.txt",
//        "PE49.txt",
//        "PE50.txt",
//        "PH1.txt",
//        "PH2.txt",
//        "PH3.txt",
//        "PH4.txt",
//        "PH5.txt",
//        "PM1.txt",
//        "PM2.txt",
//        "PM3.txt",
//        "PM4.txt",
//        "PM5.txt"
//    };
    int m = 0,n = 0,p = 0,q = 0;
    float timeout = 300;
    char* inputFileName = "input.txt";
    char* outputFileName = "output.txt";
    vector<char*> options;
    if (argc >= 3) {
        inputFileName = argv[0];
        outputFileName = argv[1];
        timeout = atol(argv[2]);
        int opNum = argc - 3;
        for (std::size_t i = 0; (std::size_t)i < (std::size_t)opNum; ++i) {
            options[i] = argv[3 + i];
        }
    }
//    SudokuSolver S;
//    S.setFlag(HeuristicFlag::kFC,true);
//    for (int i = 0; i < 61; ++i) {
////        SudokuReader R (inputFileName,"");
//        SudokuReader R(inputFiles[i],"");
//        char** sudoku = R.loadSudoku(n,p,q,timeout);
//        cout << "Sudoku Puzzle: " << inputFiles[i] << " @ i=" << i << " with parameters:\n"
////             << "m: " << m << endl
//             << "n: " << n << endl
//             << "p: " << p << endl
//             << "q: " << q << endl;
//        char e = '0';
//    //    cout << "Creating Sudoku Puzzle" << endl;
//        SudokuPuzzle P (sudoku,m,n,p,q);
//        cout << "Initial Puzzle~\n";
//        P.display ();
//    //    cout << "Solving" << endl;
//        LogBundle B = S.solve(P,timeout,e);
//    //    cout << "Creating Logger" << endl;
//        Logger L(B);
//        L.logInfo(outputFileName);
//        cout << "Finished Puzzle~\n";
//        P.display ();
//        cout << "Output Info~\n";
//        L.printInfo();
//        cin.get();
//    }
    SudokuSolver S;
    for (std::size_t i = 0; i < (std::size_t)options.size(); ++i) {
        if (options[i] == "FC"){
            S.setFlag(HeuristicFlag::kFC,true);
        }
        else {

        }
    }

    SudokuReader R (inputFileName,outputFileName);
//    SudokuReader R(inputFiles[i],"");
    char** sudoku = R.loadSudoku(n,p,q,timeout);
//    cout << "Sudoku Puzzle: " << inputFiles[i] << " @ i=" << i << " with parameters:\n"
//             << "m: " << m << endl
       cout << "n: " << n << endl
         << "p: " << p << endl
         << "q: " << q << endl;
    char e = '0';
//    cout << "Creating Sudoku Puzzle" << endl;
    SudokuPuzzle P (sudoku,m,n,p,q);
    cout << "Initial Puzzle~\n";
    P.display ();
//    cout << "Solving" << endl;
    LogBundle B = S.solve(P,timeout,e);
//    cout << "Creating Logger" << endl;
    Logger L(B);
    L.logInfo(outputFileName);
    cout << "Finished Puzzle~\n";
    P.display ();
    cout << "Output Info~\n";
    L.printInfo();
    cin.get();
    return 0;
}
