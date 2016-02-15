#include "sudokusolver.h"
using namespace std;
SudokuSolver::SudokuSolver () {
    _bookKeep = BookKeeper ();
    _flags = {
       { HeuristicFlag::kMRV, false},
       { HeuristicFlag::kDH, false },
       { HeuristicFlag::kLCV, false },
       { HeuristicFlag::kACP, false },
       { HeuristicFlag::kMAC, false },
       { HeuristicFlag::kFC, false }
    };
}

void SudokuSolver::setFlag (HeuristicFlag::HeuristicFlag flag, bool value) {
    _flags[flag] = value;
}

bool SudokuSolver::getFlag (HeuristicFlag::HeuristicFlag flag) {
    return _flags[flag];
}

LogBundle SudokuSolver::solve (SudokuPuzzle puzzle, float timeout, char& error) {
//    cout << "Inside solve" << endl;
    int depth = -1;
    const clock_t start = clock ();
    int status = 0;
    int nodes = 0, backtracks = 0;
    float ppStart = clock () / CLOCKS_PER_SEC;
    float searchStart = clock () / CLOCKS_PER_SEC;
    float searchFinish;
    LogBundle L;

    if (_flags[HeuristicFlag::kACP]) {
        //Perform ACP
    }
    float ppFinish = clock () / CLOCKS_PER_SEC;
    L._totalStart = start / CLOCKS_PER_SEC;
    L._preStart = ppStart;
    L._preDone = ppFinish;
    L._searchStart = searchStart;
//    cout << "Back Track Searching" << endl;
    error = backTrackSearch (puzzle,depth,timeout,nodes,backtracks,start);
//    cout << "Search is done" << endl;
    searchFinish = clock () / CLOCKS_PER_SEC;
    L._searchDone = searchFinish;
    std::vector<std::pair<Position,char>> solution;
    if (error == Error::kNoError || error == Error::kFailure) {
        status = 1;
        if (puzzle.complete()) {
            //complete solution
            for (std::size_t x = 0; x < (std::size_t)puzzle.n(); ++x) {
                for (std::size_t y = 0; y < (std::size_t)puzzle.n(); ++y) {
                    Position p(x,y);
                    std::pair<Position,char> P(p,puzzle.sudoku()[x][y]._value);
                    solution.insert (solution.end(),P);
                }
            }
        }
        else {
            //Unsolvable
            for (std::size_t x = 0; x < (std::size_t)puzzle.n(); ++x) {
                for (std::size_t y = 0; y < (std::size_t)puzzle.n(); ++y) {
                    Position p(x,y);
                    std::pair<Position,char> P(p,'0');
                    solution.insert (solution.begin (),P);
                }
            }
        }
    }
    else if (error == Error::kTimeoutError){
        status = -1;
        for (std::size_t x = 0; x < (std::size_t)puzzle.n(); ++x) {
            for (std::size_t y = 0; y < (std::size_t)puzzle.n(); ++y) {
                Position p(x,y);
                std::pair<Position,char> P(p,'0');
                solution.insert (solution.begin (),P);
            }
        }
    }
    L._status = status;
    L._nodes = nodes;
    L._deadends = backtracks;
    L._solution = solution;
    L._solutionTime = (ppFinish - ppStart) + (searchFinish - searchStart);
    return L;

}

char SudokuSolver::backTrackSearch (SudokuPuzzle &puzzle, int& level, float timeout, int& nodes, int& deadends, float start) {
    ++level;
//    cout << "Back Track Search @ Level=" << level << endl;
//    cout << "Checking for timeout... ";
    if ((clock () - start) / CLOCKS_PER_SEC > timeout) {
//        cout << "TIMEOUT" << endl;
        //throw Error::kTimeoutError;
        return Error::kTimeoutError;
    }
//    cout << "No Timeout" << endl;
//    cout << "Checking Solution for Completeness... ";
    if (checkCompleteness(puzzle)) {
//        cout << "COMPLETE" << endl;
        return Error::kNoError;
    }
//    cout << "Not Complete" << endl;
//    cout << "Selecting Next Variable... ";
    Variable v = selectNextVariable(puzzle);
//    cout << v._position._x << "," << v._position._y << endl;
    if (v._position._x == -1) {
        std::cout << "No more unassigned variables left; Complete should've have ended it, but didn't" << std::endl;
    }
//    cout << "Ordering Domain Values... ";
    Domain ordered = orderDomainValues(puzzle,v);
//    for (int i = 0; i < ordered._domain.size(); ++i) {cout << ordered._domain[i] << ",";} cout << std::endl;
    for (std::size_t i = 0; i < ordered._domain.size (); ++i) {
//        cout << "Checking the value=" << ordered._domain[i] << " @ " << i << "... ";
        if (checkConsistency(puzzle,v._position,ordered._domain[i])) {
//            cout << "CONSISTENT" << endl;
//            cout << "Assigning value" << endl;
//            cout << "value before=" << v._value << endl;
            assignValue(puzzle.sudoku()[v._position._x][v._position._y],ordered._domain[i],level);
//            cout << "value after=" << v._value << endl;
            ++nodes;
//            cout << "Nodes=" << nodes << endl;
            if (_flags[HeuristicFlag::kFC]) {
//                cout << "Forward Checking for v=" << v << "@ level=" << level << endl;
                forwardCheck (puzzle,puzzle.sudoku()[v._position._x][v._position._y],level);
            }

            if (_flags[HeuristicFlag::kMAC]) {
                //MAC
            }
//            cout << "Recursion" << endl;
            char result = backTrackSearch (puzzle,level,timeout,nodes,deadends,start);
//            cout << "result is... ";
            if (result == Error::kNoError) {
//                cout << "NO ERROR" << endl;
                return result;
            }
            else if (result == Error::kFailure) {
//                cout << "FAILURE" << endl;
                undoLastAction(puzzle);
                ++deadends;
            }
            else if (result == Error::kTimeoutError) {
//                cout << "TIMEOUT" << endl;
//                std::cout << "Timeout" << std::endl;
                return result;
            }
        }
        else {
//            cout << "Not Consistent" << endl;
        }
    }
    return Error::kFailure;
}

Variable SudokuSolver::selectNextVariable (SudokuPuzzle puzzle) {
//    cout << "In Select Next Variable" << endl;
//    cout << "Getting unassigned variables... ";
    std::vector<Variable> unassigned = getUnassignedVariables(puzzle);
//    cout << unassigned.size() << " of them" << endl;
    if (_flags[HeuristicFlag::kMRV]) {
        //MRV
    }
    if (_flags[HeuristicFlag::kDH]) {
        //DH
    }
    if (unassigned.size () > 0) {
        return unassigned[0];
    }
    else {
        Position p(-1,-1);
        Variable noV (p,{});
        return noV;
    }
}

Domain SudokuSolver::orderDomainValues (SudokuPuzzle puzzle, Variable variable) {
    if (_flags[HeuristicFlag::kLCV]) {
        //LCV
        return Domain ();
    }
    else {
        return variable._domain;
    }
}

bool SudokuSolver::checkConsistency (SudokuPuzzle puzzle, Position position, char value) {
    return checkAllDif (puzzle,position,value);
}

bool SudokuSolver::checkCompleteness (SudokuPuzzle puzzle) {
//    cout << "In Check Completeness" << endl;
    return puzzle.complete();
}

void SudokuSolver::bookKeep (int level, Variable& variable, Domain values) {
//    cout << "In BookKeep" << endl;
//    cout << "Bookkeeping..." << endl;
    _bookKeep.add(level,variable);
//    cout << "Finished bookkeeping" << endl;
//    cout << "values size=" << values._domain.size() << endl;
//    cout << "variables size=" << variable._domain._domain.size() << endl;
    //Remove values from domain
    for (std::size_t i = 0; i < values._domain.size(); ++i) {
//        cout << "i=" << i << ", d=" << values._domain[i] << endl;
        for (std::size_t j = 0; j < variable._domain._domain.size(); ++j) {
//            cout << "j=" << j << ", d=" << variable._domain._domain[i] << endl;
//            cout << "Checking if equal... ";
            if (values._domain[i] == variable._domain._domain[j]) {
//                cout << "EQUAL" << endl;
//                cout << "Removing... ";
//                cout << variable._domain._domain[j] << endl;
                variable._domain._domain.erase(variable._domain._domain.begin() + j);
//                cout << "Finished removing" << endl;
                j = variable._domain._domain.size();
            }
            else {
//                cout << "Not Equal" << endl;
            }
        }
    }
}

void SudokuSolver::undoLastAction (SudokuPuzzle &puzzle) {
//    cout << "In Undo Last Action" << endl;
//    cout << "Grabbing last changes... ";
    std::vector<Variable> changes = _bookKeep.undo();
//    cout << changes.size() << " many changes" << endl;
//    for (int i = 0; i < changes.size(); ++i) {cout << changes[i]._position._x << "," << changes[i]._position._y << ": "; for (int j = 0; j < changes[i]._domain._domain.size(); ++j) {cout << changes[i]._domain._domain[j] << ",";}cout << endl;}
    //Add values to domain

    for (std::size_t i = 0; i < changes.size(); ++i) {
//        cout << "Modifying current variables with stored ones @ i=" << i << endl;
        puzzle.sudoku()[changes[i]._position._x][changes[i]._position._y] = changes[i];
    }
}

void SudokuSolver::forwardCheck (SudokuPuzzle &puzzle, Variable variable, int level) {
//    cout << "Grabbing Neighbors of var=" << variable._position._x << "," << variable._position._y << " : " << variable << "... ";
    std::vector<Variable> neighbors = getNeighbors(puzzle,variable);
//    for (int i = 0; i < neighbors.size(); ++i) {cout << neighbors[i]._position._x << "," << neighbors[i]._position._y << ";";} cout << endl;
    for (std::size_t i = 0; i < neighbors.size(); ++i) {
        for (std::size_t j = 0; j < neighbors[i]._domain._domain.size(); ++j) {
//            cout << "Neighbor=" << i << ",val=" << neighbors[i]._domain._domain[j] <<  endl;
//            cout << "Checking for match... ";
            if (neighbors[i]._domain._domain[j] == variable._value) {
//                cout << "MATCH" << endl;
                Domain d;
//                cout << "Domain=" << neighbors[i]._position._x << "," << neighbors[i]._position._y << " : " << variable._value << endl;
                d.add(variable._value);
//                cout << "Book Keeping" << endl;
                bookKeep(level,puzzle.sudoku()[neighbors[i]._position._x][neighbors[i]._position._y],d);
//                cout << "Domain for neighbor=" << i << " now: ";
//                for (int z = 0; z < puzzle.sudoku()[neighbors[i]._position._x][neighbors[i]._position._y]._domain._domain.size (); ++z) {cout << puzzle.sudoku()[neighbors[i]._position._x][neighbors[i]._position._y]._domain._domain[z] << ",";} cout << endl;
                j = neighbors[i]._domain._domain.size();
            }
            else {
//                cout << "no match" << endl;
            }
        }
    }
}

std::vector<Variable> SudokuSolver::applyMRV(SudokuPuzzle puzzle, std::vector<Variable> unassigned) {
    return std::vector<Variable> ();
}

std::vector<Variable> SudokuSolver::applyDH(SudokuPuzzle puzzle, std::vector<Variable> unassigned) {
    return std::vector<Variable> ();
}

Domain SudokuSolver::applyLCV(SudokuPuzzle puzzle, Variable variable) {
    return Domain ();
}

void SudokuSolver::applyAC3 (SudokuPuzzle &puzzle) {

}

int SudokuSolver::getDegree (SudokuPuzzle puzzle, Variable variable) {
    return 0;
}

int SudokuSolver::getRemainingValues  (SudokuPuzzle puzzle, Variable variable) {
    return 0;
}

int SudokuSolver::getConstraints (SudokuPuzzle puzzle, Variable variable, char value) {
    return 0;
}

bool SudokuSolver::checkAllDif (SudokuPuzzle puzzle, Position position, char value) {
    //cout << checkRow (puzzle,position,value) << endl;
    //cout << checkColumn(puzzle,position,value) << endl;
    //cout << checkBox (puzzle,position,value) << endl;
    return checkRow (puzzle,position,value) && checkColumn(puzzle,position,value) && checkBox (puzzle,position,value);
}

bool SudokuSolver::checkRow (SudokuPuzzle puzzle, Position position, char value) {
    for (std::size_t i = 0; i < (std::size_t) puzzle.n(); ++i) {
        if (position._y != i  && puzzle.sudoku()[position._x][i]._value == value) {
            return false;
        }
    }
    return true;
}

bool SudokuSolver::checkColumn (SudokuPuzzle puzzle, Position position, char value) {
    for (std::size_t i = 0; i < (std::size_t) puzzle.n(); ++i) {
        if (position._x != i  && puzzle.sudoku()[i][position._y]._value == value) {
            return false;
        }
    }
    return true;
}

bool SudokuSolver::checkBox (SudokuPuzzle puzzle, Position position, char value) {
    int t = (position._x / puzzle.p()) * puzzle.p();
    int b = t + puzzle.p() - 1;
    int l = (position._y / puzzle.q()) * puzzle.q();
    int r = l + puzzle.q() - 1;
    for (std::size_t i = t; i <= (std::size_t) b; ++i) {
        for (std::size_t j = l; j <= (std::size_t) r; ++j) {
            if (position._x != i && position._y != j && puzzle.sudoku()[i][j]._value == value) {
                return false;
            }
        }
    }
    return true;
}

std::vector<Variable> SudokuSolver::getUnassignedVariables (SudokuPuzzle puzzle) {
    std::vector<Variable> unassigned;
    for (std::size_t x = 0; x < puzzle.n (); ++x) {
        for (std::size_t y = 0; y < puzzle.n (); ++y) {
            if (puzzle.access(x,y)._value == '0') {
                unassigned.insert (unassigned.end (), puzzle.sudoku()[x][y]);
            }
        }
    }
    return unassigned;
}

int SudokuSolver::findSmallest (std::vector<int> list) {
    if (list.size () < 1) {
        return -1;
    }
    int min = list[0];
    for (std::size_t i = 1; i < list.size (); ++i) {
        if (min > list[i]) {
            min = list[i];
        }
    }
    return min;
}

int SudokuSolver::findLargest (std::vector<int> list) {
    if (list.size () < 1) {
        return -1;
    }
    int max = list[0];
    for (std::size_t i = 1; i < list.size (); ++i) {
        if (max < list[i]) {
            max = list[i];
        }
    }
    return max;
}

bool SudokuSolver::checkArc (SudokuPuzzle puzzle, Variable variable1, Variable variable2) {
//    int x1 = variable1._position._x, x2 = variable2._position._x;
//    int y1 = variable1._position._y, y2 = variable2._position._y;
//    int t1 = (x1 / puzzle.p()) * puzzle.p(), t2 = (x2 / puzzle.p()) * puzzle.p();
//    int b1 = t1 + puzzle.p() - 1, b2 = t2 + puzzle.p() - 1;
//    int l1 = (y1 / puzzle.q()) * puzzle.q(), l2 = (y2 / puzzle.q()) * puzzle.q();
//    int r1 = l1 + puzzle.q() - 1, t2 = l2 + puzzle.q() - 1;
//    bool consistent = false;
//    if (x1 == x2 || y1 == y2 || (t1 == t2 && b1 == b2 && l1 == l2 && r1 == r2)) {
//        for (std::size_t i = 0; i < variable1._domain._domain.size(); ++i) {
//            consistent = false;
//            for (std::size_t j = 0; j < variable2._domain._domain.size(); ++j) {
//                if (variable1._domain._domain[i] != variable2._domain._domain[j]) {
//                    consistent = true;
//                    j = variable2._domain._domain.size();
//                }
//            }
//            if (!consistent) {
//                return false;
//            }
//        }
//        return true;
//    }
//    else {
//        return true;
    //    }
}

void SudokuSolver::assignValue(Variable& v, char value, int level) {
//    cout << "In Assign Value" << endl;
    Domain d;
    for (std::size_t i = 0; i < v._domain._domain.size (); ++i) {
        if (v._domain._domain[i] != value) {
            d.add(v._domain._domain[i]);
        }
    }
//    for (std::size_t i = 0; i < d._domain.size(); ++i) {cout << "d=" << d._domain[i] << " ";} cout << std::endl;
//    cout << "Book Keeping" << endl;
    bookKeep(level,v,d);
//    cout << "Assigning value=" << value << endl;
    v._value = value;
}

std::vector<Variable> SudokuSolver::getNeighbors(SudokuPuzzle puzzle, Variable variable) {
    std::vector<Variable> neighbors;
    for (std::size_t x = 0; x < (std::size_t)puzzle.n(); ++x) {
        for (std::size_t y = 0; y < (std::size_t)puzzle.n(); ++y) {
            if ((x != variable._position._x || y != variable._position._y) && (x == variable._position._x || y == variable._position._y || withinBlock (puzzle, variable, puzzle.sudoku()[x][y]))) {
                neighbors.insert (neighbors.end (), puzzle.sudoku()[x][y]);
            }
        }
    }
    return neighbors;
}

bool SudokuSolver::withinBlock(SudokuPuzzle puzzle, Variable variable1, Variable variable2) {
    int x1 = variable1._position._x, x2 = variable2._position._x;
    int y1 = variable1._position._y, y2 = variable2._position._y;
    int t1 = (x1 / puzzle.p()) * puzzle.p(), t2 = (x2 / puzzle.p()) * puzzle.p();
    int b1 = t1 + puzzle.p() - 1, b2 = t2 + puzzle.p() - 1;
    int l1 = (y1 / puzzle.q()) * puzzle.q(), l2 = (y2 / puzzle.q()) * puzzle.q();
    int r1 = l1 + puzzle.q() - 1, r2 = l2 + puzzle.q() - 1;
    return (x1 == x2 || y1 == y2 || (t1 == t2 && b1 == b2 && l1 == l2 && r1 == r2));
}




//using namespace std;
//SudokuSolver::SudokuSolver() {
//    _alphabet = nullptr;
//    _flags = {
//       { HeuristicFlag::kMRV, false},
//       { HeuristicFlag::kDH, false },
//       { HeuristicFlag::kLCV, false },
//       { HeuristicFlag::kACP, false },
//       { HeuristicFlag::kMAC, false },
//       { HeuristicFlag::kFC, false }
//    };
//    _recordBook = std::vector<Record> ();
//}

//SudokuSolver::SudokuSolver (char* alphabet) {
//    _alphabet = alphabet;
//    _flags = {
//       { HeuristicFlag::kMRV, false},
//       { HeuristicFlag::kDH, false },
//       { HeuristicFlag::kLCV, false },
//       { HeuristicFlag::kACP, false },
//       { HeuristicFlag::kMAC, false },
//       { HeuristicFlag::kFC, false }
//    };
//    _recordBook = std::vector<Record> ();
//}

//SudokuSolver::SudokuSolver (const SudokuSolver& S) {
//    _alphabet = S._alphabet;
//    _flags = S._flags;
//    _recordBook = S._recordBook;
//}

//SudokuSolver& SudokuSolver::operator = (const SudokuSolver& S) {
//    delete[] _alphabet;
//    _alphabet = S._alphabet;
//    _flags.clear();
//    _flags = S._flags;
//    _recordBook.clear ();
//    _recordBook = S._recordBook;
//    return *this;
//}

//SudokuSolver::~SudokuSolver () {
////	cout << "DESTROYING" << endl;
//    //delete _alphabet;
////	cout << "END" << endl;
//}

//char* SudokuSolver::getAlphabet () {
//    return _alphabet;
//}

//void SudokuSolver::setAlphabet (char* alphabet) {
//    _alphabet = alphabet;
//}

//bool SudokuSolver::getFlag (HF flag) {
//    return _flags[flag];
//}

//void SudokuSolver::setFlag(HF flag, bool value) {
//    _flags[flag] = value;
//}

//std::vector<Record> SudokuSolver::getRecordBook () {
//    return _recordBook;
//}

//char** SudokuSolver::solveSudoku (char** sudoku, int n, int p, int q, int timeout, char* fileName) {
////    using namespace std;
//    //what to do with m,p,q
//    //Start Time


//    int depth = -1;
//    Assignment solution;
//    const clock_t start = clock ();
//    int status;
//    int nodes = 0, backtracks = 0;
//    float ppStart = clock () / CLOCKS_PER_SEC;
//    float ppFinish;
//    float searchStart = clock () / CLOCKS_PER_SEC;
//    float searchFinish;
//    std::vector<Variable> unassignedVariables = getUnassignedVariables (sudoku,n); //Pass as arg and perform in solve func?
//    if (getFlag(HeuristicFlag::kACP)) { //Perform in solve func before?
//        //Perform ACP
//    }
//    ppFinish = clock () / CLOCKS_PER_SEC;
//    try {
//        solution = recursiveBackTrack (solution, sudoku, n, unassignedVariables,p,q,depth,timeout,start,nodes,backtracks); // NODES??
//        searchFinish = clock () / CLOCKS_PER_SEC;
//        status = 1;
//        applySolution (solution,sudoku,n);
//        bool solved = false;
//        if (solution[solution.size () - 1].second != Error::kFailure) {
//            solved = true;
//        }
//        printLog (fileName,start / CLOCKS_PER_SEC,ppStart,ppFinish,searchStart,searchFinish,status,solution,nodes,backtracks,solved,n);
//    }
//    catch (char e) {
//        if (e == Error::kTimeoutError) {
//            status = -1;
//            searchFinish = clock () / CLOCKS_PER_SEC;
//            printLog (fileName,start / CLOCKS_PER_SEC,ppStart,ppFinish,searchStart,searchFinish,status,solution,nodes,backtracks,false,n);
//            throw Error::kTimeoutError;
//        }
//    }



////    cout << "Assignment size: " << solution.size() << endl;
////    for (std::size_t i = 0; i < solution.size (); ++i) {
////        cout << "i: " << solution[i].first._position.first << "," << solution[i].first._position.second << ": " << solution[i].second << ";";
////    }
////    cout << endl;


////    for (std::size_t x = 0; x < n; ++x) {
////        std::cout << std::endl;
////        if (x % p == 0) {
////            for (std::size_t i = 0; i < 2 * n + q - 1; ++i) {
////                cout << "-";
////            }
////            std::cout << std::endl;
////        }
////        for (std::size_t y = 0; y < n; ++y) {
////            if (y % q == 0)
////                cout << "|";
////            if (sudoku[x][y] == kInitialToken)
////                cout << "  ";
////            else
////                std::cout << sudoku[x][y] << " ";
////        }

////    }
////    std::cout << std::endl;
//    //End Time
//    return sudoku; // Change or just copy sudoku
//}


//Variable SudokuSolver::selectNextVariable (std::vector<Variable> unassignedVariables, Assignment assignment) {
//    std::vector<Variable> shortened;
//    if (_flags[HeuristicFlag::kMRV]) {

//    }
//    else {
//        shortened = unassignedVariables;
//    }
//    if (_flags[HeuristicFlag::kDH]) {

//    }
//    if (!shortened.empty()){
//        return shortened[0];
//    }
//    else {
//        return Variable (std::make_pair<int,int> (-1,-1));
//    }
//}

//std::vector<char> SudokuSolver::orderDomainValues (Variable variable, Assignment assignment) {
//    std::vector<char> domainValues;
//    if (_flags[HeuristicFlag::kLCV]) {

//    }
//    else {
//        domainValues = variable._domain;
//    }
//    return domainValues;
//}

//Assignment SudokuSolver::recursiveBackTrack (Assignment& assignment, char** sudoku, int n, std::vector<Variable>& unassignedVariables, int p, int q, int d, float timeout, const clock_t start, int& nodes, int& backtracks) {
//    ++d;
////    cout << d << ": BEGIN" << endl;
//    if ((clock () - start) / CLOCKS_PER_SEC > timeout) {
//        throw Error::kTimeoutError;
//    }
//    if (isComplete(assignment,sudoku,n)) {
//        return assignment;
//    }
////    cout << d << ": NOT COMPLETE" << endl;
//    Variable v = selectNextVariable (unassignedVariables,assignment);
////    cout << d << ": Variable: " << v._position.first << ", " << v._position.second << endl;
//    if (v._position.first == -1) {
//        return assignment;
//    }

//    std::vector<char> values = orderDomainValues (v,assignment); //Might need sudoku arg
////    cout << d << ": " << endl;
////    for (std::size_t i = 0; i < values.size (); ++i) {
////        cout << values[i] << ",";
////    }
////    cout << endl;

//    for (std::size_t i = 0; i < values.size(); ++i) {
////        cout << "i: " << i << ": " << values[i] << endl;
//        if  (checkAllDifConstraint(v._position.first,v._position.second,values[i],sudoku,n,p,q,assignment)) { //Maybe n arg
////            cout << "ALLDIF SATISFIED" << endl;
//            std::pair<Variable,char> vc;
//            vc.first = v;
//            vc.second = values[i];
//            assignment.insert (assignment.end (),vc);
// //           cout << "INSERT: " << v._position.first << "," << v._position.second << ": " << values[i] << endl;
//            unassignedVariables.erase (unassignedVariables.begin ());
////            cout << "UNASSIGNED: " << endl;
////            for (std::size_t i = 0; i < unassignedVariables.size(); ++i) {
////                cout << "i: " << unassignedVariables[i]._position.first << "," << unassignedVariables[i]._position.second << ";";
////            }
////            cout << endl;
//            if (getFlag(HeuristicFlag::kFC)) {
//                //Perform FC & BK
//            }
//            if (getFlag(HeuristicFlag::kMAC)) {
//                //Perform MAC & BK
//            }
//            Assignment result = recursiveBackTrack (assignment,sudoku,n,unassignedVariables,p,q,d,timeout,start,nodes,backtracks);
////            cout << "POPPED OUT to " << d << endl;
////            cout << "RESULT: " << endl;
////            for (std::size_t i = 0; i < result.size (); ++i) {
////                cout << "i: " << result[i].first._position.first << "," << result[i].first._position.second << ": " << result[i].second << ";";
////            }
////            cout << endl;
//            if (result[result.size () - 1].second != Error::kFailure) {
////                cout << "NO FAILURE" << endl;
//                return result;
//            }
// //           cout << "FAILURE" << endl;
//  //          for (std::size_t i = 0; i < assignment.size(); ++i) {
//   //             cout << "i: " << assignment[i].first._position.first << "," << assignment[i].first._position.second << ": " << assignment[i].second << ";";
//   //         }
//	//		cout << assignment.size() << endl;
//	//		cout << assignment[assignment.size() - 1].first._position.first << ',' << assignment[assignment.size() - 1].first._position.second << ":" << assignment[assignment.size() - 1].second << endl;
//            //assignment.erase (assignment.end ()); //For the failure flag
//			assignment.erase(assignment.end() - 1, assignment.end());
//            //assignment.erase (assignment.end ());
//            ++backtracks;
//       //     cout << "ASSIGNMENT CURRENTLY: " << endl;
////            for (std::size_t i = 0; i < assignment.size(); ++i) {
////                cout << "i: " << assignment[i].first._position.first << "," << assignment[i].first._position.second << ": " << assignment[i].second << ";";
////            }
//            unassignedVariables.insert (unassignedVariables.begin (),v);
//        }
//		//cout << "NOT SATISFIED" << endl;
//    }
//    assignment.insert (assignment.end (), std::make_pair (Variable (),Error::kFailure));
//    return assignment;
//}



//void SudokuSolver::bookKeep (std::pair<Variable,char> assignment, RemovalRecord removals) {
//    //For every removal, enter the removal in the form of Level: (Variable : Value) into the end of the list
//    //If undo is required, Pop and add the value to the variable of the previous level.
//}

//std::vector<Variable> SudokuSolver::applyMRV (std::vector<Variable> potentialVariables) {
//	return std::vector<Variable> ();
//}

//std::vector<Variable> SudokuSolver::applyDH (std::vector<Variable> potentialVariables, char** sudoku, int n) {
//	return std::vector<Variable>();
//}

//std::vector<char> SudokuSolver::applyLCV (Variable variable, char** sudoku, int n) {
//	return std::vector<char>();
//}

//void SudokuSolver::applyAC3 (char** sudoku, int n) {

//}

//void SudokuSolver::applyFC (char** sudoku, int n) {
//    //Remove the assigned value from every variable's domain in the row, column, and block
//}

//std::vector<Variable> SudokuSolver::getUnassignedVariables (char** sudoku, int n) const {
//    std::vector<Variable> variables;
//    for (std::size_t x = 0; x < n; ++x) {
//        for (std::size_t y = 0; y < n; ++y) {
//            std::vector<char> domain;
//            if (sudoku[x][y] != '0') {
//                domain.insert(domain.begin (), sudoku[x][y]);
//            }
//            else {
//                domain = getTokenAlphabet(n);
//            }
//            variables.insert (variables.end (), Variable(std::make_pair<int,int> ((int)x,(int)y),domain));
//        }
//    }
//    return variables;
//}

//bool SudokuSolver::checkAllDifConstraint (int x, int y, int v, char** sudoku, int n, int p, int q, Assignment current) const {
//    for (std::size_t i = 0; i < (std::size_t) n; ++i) {
//        if (y != i && sudoku[x][i] == v) {
//            return false;
//        }
//        if (x != i && sudoku[i][y] == v) {
//            return false;
//        }
//    }
//    int t = (x / p) * p;
//    int b = t + p - 1;
//    int l = (y / q) * q;
//    int r = l + q - 1;
//    for (std::size_t i = 0; i < current.size(); ++i) {
//        if (current[i].first._position.first == x && current[i].second == v) {
//            return false;
//        }
//        if (current[i].first._position.second == y && current[i].second == v) {
//            return false;
//        }
//        if (current[i].first._position.first >= t && current[i].first._position.first <= b && current[i].first._position.second >= l && current[i].first._position.second <= r && current[i].second == v) {
//            return false;
//        }
//    }


//    for (std::size_t i = t; i <= (std::size_t) b; ++i) {
//        for (std::size_t j = l; j <= (std::size_t) r; ++j) {
//            if (x != i && y != j && sudoku[i][j] == v) {
//                return false;
//            }
//        }
//    }
//    return true;
//}

//bool SudokuSolver::isComplete(Assignment assignment, char **sudoku, int n) {
//    for (std::size_t x = 0; x < n; ++x) {
//        for (std::size_t y = 0; y < n; ++y) {
////            if (assignment.) {
//                //Check each variable
////            }
//        }
//    }
//    return assignment.size () == n * n;
//}

//void SudokuSolver::applySolution(Assignment assignment, char **sudoku, int n) {
////    cout << "IN APPLY SOLUTION" << endl;
////    for (std::size_t i = 0; i < assignment.size (); ++i) {
////        cout << "i: " << assignment[i].first._position.first << "," << assignment[i].first._position.second << ": " << assignment[i].second << ";";
////    }
////    cout << endl;
//    for (std::size_t i = 0; i < assignment.size(); ++i) {
////        cout << assignment[i].first._position.first << "," << assignment[i].first._position.second << endl;
////        cout << assignment[i].second << endl;
//        sudoku[assignment[i].first._position.first][assignment[i].first._position.second] = assignment[i].second;
//    }
//}

//void SudokuSolver::printLog(char *fileName, float start, float ppStart, float ppFinish, float searchStart, float searchFinish, int status, Assignment solution, int nodes, int backtracks, bool solved,int n) {
//    std::ofstream output (fileName);
//    if (output.is_open ()) {
//        output << "TOTAL_START=" << start << std::endl;
//        output << "PREPROCESSING_START=" << ppStart << std::endl;
//        output << "PREPROCESSING_DONE=" << ppFinish << std::endl;
//        output << "SEARCH_START=" << searchStart << std::endl;
//        output << "SEARCH_DONE=" << searchFinish << std::endl;
//        output << "SOLUTION_TIME=" << (ppFinish - ppStart) + (searchFinish - searchStart) << std::endl;
//        output << "STATUS=";
//        switch (status) {
//            case -1: {
//                output << "timeout";
//                break;
//            }
//            case 0: {
//                output << "error";
//                break;
//            }
//            case 1: {
//                output << "success";
//                break;
//            }
//        }
//        output << std::endl;
//        output << "SOLUTION=(";
//        if (solved) {
//            for (std::size_t i = 0; i < solution.size(); ++i) {
//                output << solution[i].second;
//                if (i != solution.size() - 1) {
//                    output << ",";
//                }
//            }
//        }
//        else {
//            for (std::size_t i = 0; i < n * n; ++i) {
//                output << '0';
//                if (i != (n * n) - 1) {
//                    output << ",";
//                }
//            }
//        }
//        output << ")" << std::endl;
//        output << "COUNT_NODES=" << nodes << std::endl;
//        output << "COUNT_DEADENDS=" << backtracks << std::endl;
//    }
//    output.close ();
//}
