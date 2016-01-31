#include "sudokusolver.h"
using namespace std;
SudokuSolver::SudokuSolver() {
    _alphabet = nullptr;
    _flags = {
       { HeuristicFlag::kMRV, false},
       { HeuristicFlag::kDH, false },
       { HeuristicFlag::kLCV, false },
       { HeuristicFlag::kACP, false },
       { HeuristicFlag::kMAC, false },
       { HeuristicFlag::kFC, false }
    };
    _recordBook = std::vector<Record> ();
}

SudokuSolver::SudokuSolver (char* alphabet) {
    _alphabet = alphabet;
    _flags = {
       { HeuristicFlag::kMRV, false},
       { HeuristicFlag::kDH, false },
       { HeuristicFlag::kLCV, false },
       { HeuristicFlag::kACP, false },
       { HeuristicFlag::kMAC, false },
       { HeuristicFlag::kFC, false }
    };
    _recordBook = std::vector<Record> ();
}

SudokuSolver::SudokuSolver (const SudokuSolver& S) {
    _alphabet = S._alphabet;
    _flags = S._flags;
    _recordBook = S._recordBook;
}

SudokuSolver& SudokuSolver::operator = (const SudokuSolver& S) {
    delete[] _alphabet;
    _alphabet = S._alphabet;
    _flags.clear();
    _flags = S._flags;
    _recordBook.clear ();
    _recordBook = S._recordBook;
    return *this;
}

SudokuSolver::~SudokuSolver () {
    delete[] _alphabet;
}

char* SudokuSolver::getAlphabet () {
    return _alphabet;
}

void SudokuSolver::setAlphabet (char* alphabet) {
    _alphabet = alphabet;
}

bool SudokuSolver::getFlag (HF flag) {
    return _flags[flag];
}

void SudokuSolver::setFlag(HF flag, bool value) {
    _flags[flag] = value;
}

std::vector<Record> SudokuSolver::getRecordBook () {
    return _recordBook;
}

char** SudokuSolver::solveSudoku (char** sudoku, int m, int n, int p, int q, int timeout, char* fileName) {
//    using namespace std;
    //what to do with m,p,q
    //Start Time


    int depth = -1;
    Assignment solution;
    const clock_t start = clock ();
    int status;
    int nodes = 0, backtracks = 0;
    long ppStart = clock () / CLOCKS_PER_SEC;
    long ppFinish;
    long searchStart = clock () / CLOCKS_PER_SEC;
    long searchFinish;
    std::vector<Variable> unassignedVariables = getUnassignedVariables (sudoku,n); //Pass as arg and perform in solve func?
    if (getFlag(HeuristicFlag::kACP)) { //Perform in solve func before?
        //Perform ACP
    }
    ppFinish = clock () / CLOCKS_PER_SEC;
    try {
        solution = recursiveBackTrack (solution, sudoku, n, unassignedVariables,p,q,depth,timeout,start,nodes,backtracks); // NODES??
        searchFinish = clock () / CLOCKS_PER_SEC;
        status = 1;
        applySolution (solution,sudoku,n);
        bool solved = false;
        if (solution[solution.size () - 1].second != Error::kFailure) {
            solved = true;
        }
        printLog (fileName,start / CLOCKS_PER_SEC,ppStart,ppFinish,searchStart,searchFinish,status,solution,nodes,backtracks,solved,n);
    }
    catch (char e) {
        if (e == Error::kTimeoutError) {
            status = -1;
            searchFinish = clock () / CLOCKS_PER_SEC;
            printLog (fileName,start / CLOCKS_PER_SEC,ppStart,ppFinish,searchStart,searchFinish,status,solution,nodes,backtracks,false,n);
            throw Error::kTimeoutError;
        }
    }



//    cout << "Assignment size: " << solution.size() << endl;
//    for (std::size_t i = 0; i < solution.size (); ++i) {
//        cout << "i: " << solution[i].first._position.first << "," << solution[i].first._position.second << ": " << solution[i].second << ";";
//    }
//    cout << endl;


//    for (std::size_t x = 0; x < n; ++x) {
//        std::cout << std::endl;
//        if (x % p == 0) {
//            for (std::size_t i = 0; i < 2 * n + q - 1; ++i) {
//                cout << "-";
//            }
//            std::cout << std::endl;
//        }
//        for (std::size_t y = 0; y < n; ++y) {
//            if (y % q == 0)
//                cout << "|";
//            if (sudoku[x][y] == kInitialToken)
//                cout << "  ";
//            else
//                std::cout << sudoku[x][y] << " ";
//        }

//    }
//    std::cout << std::endl;
    //End Time
    return sudoku; // Change or just copy sudoku
}


Variable SudokuSolver::selectNextVariable (std::vector<Variable> unassignedVariables, Assignment assignment) {
    std::vector<Variable> shortened;
    if (_flags[HeuristicFlag::kMRV]) {

    }
    else {
        shortened = unassignedVariables;
    }
    if (_flags[HeuristicFlag::kDH]) {

    }
    if (!shortened.empty()){
        return shortened[0];
    }
    else {
        return Variable (std::make_pair<int,int> (-1,-1));
    }
}

std::vector<char> SudokuSolver::orderDomainValues (Variable variable, Assignment assignment) {
    std::vector<char> domainValues;
    if (_flags[HeuristicFlag::kLCV]) {

    }
    else {
        domainValues = variable._domain;
    }
    return domainValues;
}

Assignment SudokuSolver::recursiveBackTrack (Assignment& assignment, char** sudoku, int n, std::vector<Variable>& unassignedVariables, int p, int q, int d, long timeout, const clock_t start, int& nodes, int& backtracks) {
    ++d;
//    cout << d << ": BEGIN" << endl;
    if ((clock () - start) / CLOCKS_PER_SEC > timeout) {
        throw Error::kTimeoutError;
    }
    if (isComplete(assignment,sudoku,n)) {
        return assignment;
    }
//    cout << d << ": NOT COMPLETE" << endl;
    Variable v = selectNextVariable (unassignedVariables,assignment);
//    cout << d << ": Variable: " << v._position.first << ", " << v._position.second << endl;
    if (v._position.first == -1) {
        return assignment;
    }

    std::vector<char> values = orderDomainValues (v,assignment); //Might need sudoku arg
//    cout << d << ": " << endl;
//    for (std::size_t i = 0; i < values.size (); ++i) {
//        cout << values[i] << ",";
//    }
//    cout << endl;

    for (std::size_t i = 0; i < values.size(); ++i) {
//        cout << "i: " << i << ": " << values[i] << endl;
        if  (checkAllDifConstraint(v._position.first,v._position.second,values[i],sudoku,n,p,q,assignment)) { //Maybe n arg
//            cout << "ALLDIF SATISFIED" << endl;
            std::pair<Variable,char> vc;
            vc.first = v;
            vc.second = values[i];
            assignment.insert (assignment.end (),vc);
//            cout << "INSERT: " << v._position.first << "," << v._position.second << ": " << values[i] << endl;
            unassignedVariables.erase (unassignedVariables.begin ());
//            cout << "UNASSIGNED: " << endl;
//            for (std::size_t i = 0; i < unassignedVariables.size(); ++i) {
//                cout << "i: " << unassignedVariables[i]._position.first << "," << unassignedVariables[i]._position.second << ";";
//            }
//            cout << endl;
            if (getFlag(HeuristicFlag::kFC)) {
                //Perform FC & BK
            }
            if (getFlag(HeuristicFlag::kMAC)) {
                //Perform MAC & BK
            }
            Assignment result = recursiveBackTrack (assignment,sudoku,n,unassignedVariables,p,q,d,timeout,start,nodes,backtracks);
//            cout << "POPPED OUT to " << d << endl;
//            cout << "RESULT: " << endl;
//            for (std::size_t i = 0; i < result.size (); ++i) {
//                cout << "i: " << result[i].first._position.first << "," << result[i].first._position.second << ": " << result[i].second << ";";
//            }
//            cout << endl;
            if (result[result.size () - 1].second != Error::kFailure) {
//                cout << "NO FAILURE" << endl;
                return result;
            }
//            cout << "FAILURE" << endl;
            assignment.erase (assignment.end ()); //For the failure flag
            assignment.erase (assignment.end ());
            ++backtracks;
//            cout << "ASSIGNMENT CURRENTLY: " << endl;
//            for (std::size_t i = 0; i < assignment.size(); ++i) {
//                cout << "i: " << assignment[i].first._position.first << "," << assignment[i].first._position.second << ": " << assignment[i].second << ";";
//            }
            unassignedVariables.insert (unassignedVariables.begin (),v);
        }
    }
    assignment.insert (assignment.end (), std::make_pair (Variable (),Error::kFailure));
    return assignment;
}



void SudokuSolver::bookKeep (std::pair<Variable,char> assignment, RemovalRecord removals) {

}

std::vector<Variable> SudokuSolver::applyMRV (std::vector<Variable> potentialVariables) {

}

std::vector<Variable> SudokuSolver::applyDH (std::vector<Variable> potentialVariables, char** sudoku, int n) {

}

std::vector<char> SudokuSolver::applyLCV (Variable variable, char** sudoku, int n) {

}

void SudokuSolver::applyAC3 (char** sudoku, int n) {

}

void SudokuSolver::applyFC (char** sudoku, int n) {

}

std::vector<Variable> SudokuSolver::getUnassignedVariables (char** sudoku, int n) const {
    std::vector<Variable> variables;
    for (std::size_t x = 0; x < n; ++x) {
        for (std::size_t y = 0; y < n; ++y) {
            std::vector<char> domain;
            if (sudoku[x][y] != '0') {
                domain.insert(domain.begin (), sudoku[x][y]);
            }
            else {
                domain = getTokenAlphabet(n);
            }
            variables.insert (variables.end (), Variable(std::make_pair<int,int> (x,y),domain));
        }
    }
    return variables;
}

bool SudokuSolver::checkAllDifConstraint (int x, int y, int v, char** sudoku, int n, int p, int q, Assignment current) const {
    for (std::size_t i = 0; i < (std::size_t) n; ++i) {
        if (y != i && sudoku[x][i] == v) {
            return false;
        }
        if (x != i && sudoku[i][y] == v) {
            return false;
        }
    }
    int t = (x / p) * p;
    int b = t + p - 1;
    int l = (y / q) * q;
    int r = l + q - 1;
    for (std::size_t i = 0; i < current.size(); ++i) {
        if (current[i].first._position.first == x && current[i].second == v) {
            return false;
        }
        if (current[i].first._position.second == y && current[i].second == v) {
            return false;
        }
        if (current[i].first._position.first >= t && current[i].first._position.first <= b && current[i].first._position.second >= l && current[i].first._position.second <= r && current[i].second == v) {
            return false;
        }
    }


    for (std::size_t i = t; i <= (std::size_t) b; ++i) {
        for (std::size_t j = l; j <= (std::size_t) r; ++j) {
            if (x != i && y != j && sudoku[i][j] == v) {
                return false;
            }
        }
    }
    return true;
}

bool SudokuSolver::isComplete(Assignment assignment, char **sudoku, int n) {
    for (std::size_t x = 0; x < n; ++x) {
        for (std::size_t y = 0; y < n; ++y) {
//            if (assignment.) {
                //Check each variable
//            }
        }
    }
    return assignment.size () == n * n;
}

void SudokuSolver::applySolution(Assignment assignment, char **sudoku, int n) {
//    cout << "IN APPLY SOLUTION" << endl;
//    for (std::size_t i = 0; i < assignment.size (); ++i) {
//        cout << "i: " << assignment[i].first._position.first << "," << assignment[i].first._position.second << ": " << assignment[i].second << ";";
//    }
//    cout << endl;
    for (std::size_t i = 0; i < assignment.size(); ++i) {
//        cout << assignment[i].first._position.first << "," << assignment[i].first._position.second << endl;
//        cout << assignment[i].second << endl;
        sudoku[assignment[i].first._position.first][assignment[i].first._position.second] = assignment[i].second;
    }
}

void SudokuSolver::printLog(char *fileName, long start, long ppStart, long ppFinish, long searchStart, long searchFinish, int status, Assignment solution, int nodes, int backtracks, bool solved,int n) {
    std::ofstream output (fileName);
    if (output.is_open ()) {
        output << "TOTAL_START=" << start << std::endl;
        output << "PREPROCESSING_START=" << ppStart << std::endl;
        output << "PREPROCESSING_DONE=" << ppFinish << std::endl;
        output << "SEARCH_START=" << searchStart << std::endl;
        output << "SEARCH_DONE=" << searchFinish << std::endl;
        output << "SOLUTION_TIME=" << (ppFinish - ppStart) + (searchFinish - searchStart) << std::endl;
        output << "STATUS=";
        switch (status) {
            case -1: {
                output << "timeout";
                break;
            }
            case 0: {
                output << "error";
                break;
            }
            case 1: {
                output << "success";
                break;
            }
        }
        output << std::endl;
        output << "SOLUTION=(";
        if (solved) {
            for (std::size_t i = 0; i < solution.size(); ++i) {
                output << solution[i].second;
                if (i != solution.size() - 1) {
                    output << ",";
                }
            }
        }
        else {
            for (std::size_t i = 0; i < n * n; ++i) {
                output << '0';
                if (i != (n * n) - 1) {
                    output << ",";
                }
            }
        }
        output << ")" << std::endl;
        output << "COUNT_NODES=" << nodes << std::endl;
        output << "COUNT_DEADENDS=" << backtracks << std::endl;
    }
    output.close ();
}
