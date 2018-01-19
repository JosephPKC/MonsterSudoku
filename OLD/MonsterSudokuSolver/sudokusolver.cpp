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

LogBundle SudokuSolver::solve (SudokuPuzzle puzzle, long timeout, char& error) {
    int depth = 0;
    const clock_t start = clock ();
    int status = 0;
    int nodes = 0, backtracks = 0;
    long ppStart = clock () / CLOCKS_PER_SEC;
    long searchStart = clock () / CLOCKS_PER_SEC;
    long searchFinish;
    LogBundle L;
    std::vector<std::pair<Position,char>> solution;

    if (_flags[HeuristicFlag::kACP]) {
        //Perform ACP
        bool result = applyAC3(puzzle,depth);
        if (!result) {
            L._totalStart = start / CLOCKS_PER_SEC;
            L._preStart = ppStart;
            L._preDone = clock () / CLOCKS_PER_SEC;
            L._searchStart = searchStart;
            L._searchDone = clock () / CLOCKS_PER_SEC;
            for (std::size_t x = 0; x < (std::size_t)puzzle.n(); ++x) {
                for (std::size_t y = 0; y < (std::size_t)puzzle.n(); ++y) {
                    Position p(x,y);
                    std::pair<Position,char> P(p,'0');
                    solution.insert (solution.begin (),P);
                }
            }
            L._status = 1;
            L._nodes = nodes;
            L._deadends = backtracks;
            L._solution = solution;
            L._solutionTime = (L._preDone - ppStart) + (L._searchDone - searchStart);
            return L;
        }
    }
    long ppFinish = clock () / CLOCKS_PER_SEC;
    L._totalStart = start / CLOCKS_PER_SEC;
    L._preStart = ppStart;
    L._preDone = ppFinish;
    L._searchStart = searchStart;
    error = backTrackSearch (puzzle,depth,timeout,nodes,backtracks,start);
    searchFinish = clock () / CLOCKS_PER_SEC;
    L._searchDone = searchFinish;

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

char SudokuSolver::backTrackSearch (SudokuPuzzle &puzzle, int& level, long timeout, int& nodes, int& deadends, long start) {
    ++level;
    if ((clock () - start) / CLOCKS_PER_SEC > timeout) {
        return Error::kTimeoutError;
    }
    if (checkCompleteness(puzzle)) {
        return Error::kNoError;
    }
    Variable v = selectNextVariable(puzzle);
    if (v._position._x == -1) {
//        std::cout << "No more unassigned variables left; Complete should've have ended it, but didn't" << std::endl;
    }
    Domain ordered = orderDomainValues(puzzle,v._position);
    for (std::size_t i = 0; i < ordered._domain.size (); ++i) {
        bool r = true;
        if (checkConsistency(puzzle,v._position,ordered._domain[i])) {
            assignValue(puzzle.sudoku()[v._position._x][v._position._y],ordered._domain[i],level);
            ++nodes;
            if (_flags[HeuristicFlag::kFC]) {
                forwardCheck (puzzle,puzzle.sudoku()[v._position._x][v._position._y],level);
            }

            if (_flags[HeuristicFlag::kMAC]) {
                r = applyAC3 (puzzle,level);
                for (std::size_t x = 0; x < puzzle.n(); ++x) {
                    for (std::size_t y = 0; y < puzzle.n(); ++y) {
                        for (std::size_t i = 0; i < puzzle.sudoku()[x][y]._domain._domain.size(); ++i) {
                        }
                    }
                }
                if(!r) {
                    undoLastAction(puzzle);
                }
            }
            if (r) {
                char result = backTrackSearch (puzzle,level,timeout,nodes,deadends,start);
                if (result == Error::kNoError) {
                    return result;
                }
                else if (result == Error::kFailure) {
                    undoLastAction(puzzle);
                    ++deadends;
                }
                else if (result == Error::kTimeoutError) {
                    return result;
                }
            }
        }
        else {
        }
    }
    return Error::kFailure;
}

Variable SudokuSolver::selectNextVariable (SudokuPuzzle puzzle) {
    std::vector<Variable> unassigned = getUnassignedVariables(puzzle);
    if (_flags[HeuristicFlag::kMRV]) {
        //MRV
        std::vector<Variable> mrv = applyMRV (puzzle,unassigned);
        if (_flags[HeuristicFlag::kDH]) {
            //DH
            std::vector<Variable> dh = applyDH (puzzle,mrv);
            return dh[0];
        }
        else {
            return mrv[0];
        }
    }
    else if (_flags[HeuristicFlag::kDH]) {
        std::vector<Variable> dh = applyDH (puzzle,unassigned);
        return dh[0];
    }
    else if (unassigned.size () > 0) {
        return unassigned[0];
    }
    else {
        Position p(-1,-1);
        Variable noV (p,{});
        return noV;
    }
}

Domain SudokuSolver::orderDomainValues (SudokuPuzzle puzzle, Position position) {
    if (_flags[HeuristicFlag::kLCV]) {
        //LCV
        Domain lcv = applyLCV (puzzle, puzzle.sudoku()[position._x][position._y]);
        return lcv;
    }
    else {
        return puzzle.sudoku()[position._x][position._y]._domain;
    }
}

bool SudokuSolver::checkConsistency (SudokuPuzzle puzzle, Position position, char value) {
    return checkAllDif (puzzle,position,value);
}

bool SudokuSolver::checkCompleteness (SudokuPuzzle puzzle) {
    return puzzle.complete();
}

void SudokuSolver::bookKeep (int level, Variable& variable, Domain values) {
    _bookKeep.add(level,variable);
    for (std::size_t i = 0; i < values._domain.size(); ++i) {
        for (std::size_t j = 0; j < variable._domain._domain.size(); ++j) {
            if (values._domain[i] == variable._domain._domain[j]) {
                variable._domain._domain.erase(variable._domain._domain.begin() + j);
                j = variable._domain._domain.size();
            }
        }
    }
}

void SudokuSolver::undoLastAction (SudokuPuzzle &puzzle) {
    std::vector<Variable> changes = _bookKeep.undo();
    //Add values to domain

    for (std::size_t i = 0; i < changes.size(); ++i) {
        puzzle.sudoku()[changes[i]._position._x][changes[i]._position._y] = changes[i];
    }
}

void SudokuSolver::forwardCheck (SudokuPuzzle &puzzle, Variable variable, int level) {
    std::vector<Variable> neighbors = getNeighbors(puzzle,variable._position);
    for (std::size_t i = 0; i < neighbors.size(); ++i) {
        for (std::size_t j = 0; j < neighbors[i]._domain._domain.size(); ++j) {
            if (neighbors[i]._domain._domain[j] == variable._value) {
                Domain d;
                d.add(variable._value);
                bookKeep(level,puzzle.sudoku()[neighbors[i]._position._x][neighbors[i]._position._y],d);
                j = neighbors[i]._domain._domain.size();
            }
        }
    }
}

std::vector<Variable> SudokuSolver::applyMRV(SudokuPuzzle puzzle, std::vector<Variable> unassigned) {
    int min = getRemainingValues (puzzle,unassigned[0]._position);
    for (std::size_t i = 1; i < unassigned.size (); ++i) {
        int cur = getRemainingValues (puzzle,unassigned[i]._position);
        if (cur < min) {
            min = cur;
        }
    }
    std::vector<Variable> mrvVector;
    for (std::size_t i = 0; i < unassigned.size (); ++i) {
        if (getRemainingValues (puzzle,unassigned[i]._position) == min) {
            mrvVector.insert (mrvVector.end (), unassigned[i]);
        }
    }
    return mrvVector;
}

std::vector<Variable> SudokuSolver::applyDH(SudokuPuzzle puzzle, std::vector<Variable> unassigned) {
    int max = getDegree (puzzle,unassigned[0]._position,unassigned);
    for (std::size_t i = 1; i < unassigned.size (); ++i) {
        int cur = getDegree (puzzle,unassigned[i]._position,unassigned);
        if (cur > max) {
            max = cur;
        }
    }
    std::vector<Variable> dhVector;
    for (std::size_t i = 0; i < unassigned.size (); ++i) {
        if (getDegree (puzzle,unassigned[i]._position,unassigned) == max) {
            dhVector.insert (dhVector.end (), unassigned[i]);
        }
    }
    return dhVector;
}

Domain SudokuSolver::applyLCV(SudokuPuzzle puzzle, Variable variable) {
    std::vector<std::pair<char,int>> lcvVector;
    for (std::size_t i = 0; i < variable._domain._domain.size(); ++i) {
        std::pair<char,int> p(variable._domain._domain[i],getConstraints (puzzle,variable._position,variable._domain._domain[i]));
        lcvVector.insert (lcvVector.end(),p);
    }
    //Sort from smallest to largest
    for (std::size_t i = 0; i < lcvVector.size(); ++i) {
        int j = i;
        while (j > 0 && lcvVector[j].second < lcvVector[j - 1].second) {
            std::pair<char,int> temp = lcvVector[j];
            lcvVector[j] = lcvVector[j - 1];
            lcvVector[j - 1] = temp;
            j--;
        }
    }
    //Create domain
    Domain d;
    for (std::size_t i = 0; i < lcvVector.size(); ++i) {
        d.add(lcvVector[i].first);
    }
    return d;
}

bool SudokuSolver::applyAC3 (SudokuPuzzle &puzzle, int level) {
    std::vector<std::pair<Position,Position>> arcs;
    for (std::size_t x = 0; x < puzzle.n(); ++x) {
        for (std::size_t y = 0; y < puzzle.n(); ++y) {
            std::vector<Variable> neighbors = getNeighbors (puzzle,puzzle.sudoku()[x][y]._position);
            for (std::size_t j = 0; j < neighbors.size(); ++j) {
                Position c(x,y);
                std::pair<Position,Position> p(c,neighbors[j]._position);
                arcs.insert (arcs.end(),p);
            }
        }
    }
    for (std::size_t i = 0; i < arcs.size(); ++i) {
        char fail;
        Position cur1 = arcs[i].first;
        Position cur2 = arcs[i].second;
        arcs.erase (arcs.begin ());
        --i;
        if (!checkArc (puzzle,cur1,cur2,fail)) {
            Domain d;
            d.add(fail);
            int x = cur1._x, y = cur1._y;
            bookKeep(level,puzzle.sudoku()[x][y],d);
            if (puzzle.sudoku()[x][y]._domain._domain.empty()) {
                return false;
            }
            std::vector<Variable> neighbors = getNeighbors (puzzle,cur1);
            for (std::size_t j = 0; j < neighbors.size(); ++j) {
                bool in = false;
                std::pair<Position,Position> p(neighbors[j]._position,cur1);
                for (std::size_t k = 0; k < arcs.size(); ++k) {
                    if (p.first._x == arcs[k].first._x && p.first._y == arcs[k].first._y && p.second._x == arcs[k].second._x && p.second._y == arcs[k].second._y) {
                        in = true;
                    }
                }
                if (!in) {
                    arcs.insert (arcs.end (),p);
                }
            }
        }
    }
    return true;
}

int SudokuSolver::getDegree (SudokuPuzzle puzzle, Position position, std::vector<Variable> unassigned) {
    int degree = 0;
    std::vector<Variable> neighbors = getNeighbors (puzzle, position);
    for (std::size_t i = 0; i < neighbors.size (); ++i) {
        if (neighbors[i]._value == '0') {
            ++degree;
        }
    }
    return degree;
}

int SudokuSolver::getRemainingValues  (SudokuPuzzle puzzle, Position position) {
    //Count how many values in the domain
    return puzzle.sudoku()[position._x][position._y]._domain._domain.size();
}

int SudokuSolver::getConstraints (SudokuPuzzle puzzle, Position position, char value) {
    int constraints = 0;
    std::vector<Variable> neighbors = getNeighbors (puzzle, position);
    for (std::size_t i = 0; i < neighbors.size (); ++i) {
        if (neighbors[i]._value == '0') {
            for (std::size_t j = 0; j < neighbors[i]._domain._domain.size(); ++j) {
                if (neighbors[i]._domain._domain[j] == value) {
                    ++constraints;
                }
            }
        }
    }
    return constraints;
}

std::vector<std::pair<Variable, Variable> > SudokuSolver::loadConstraints(SudokuPuzzle puzzle) {

}

bool SudokuSolver::checkAllDif (SudokuPuzzle puzzle, Position position, char value) {
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

bool SudokuSolver::checkArc (SudokuPuzzle puzzle, Position check, Position against, char& fail) {
    bool consistent;
    for (std::size_t i = 0; i < puzzle.sudoku()[check._x][check._y]._domain._domain.size(); ++i) {
        consistent = false;
        for (std::size_t j = 0; j < puzzle.sudoku()[against._x][against._y]._domain._domain.size(); ++j) {
            if (puzzle.sudoku()[check._x][check._y]._domain._domain[i] != puzzle.sudoku()[against._x][against._y]._domain._domain[j]) {
                consistent = true;
                j = puzzle.sudoku()[against._x][against._y]._domain._domain.size();
            }
        }
        if (!consistent) {
            fail = puzzle.sudoku()[check._x][check._y]._domain._domain[i];
            return false;
        }
    }
    return true;
}

void SudokuSolver::assignValue(Variable& v, char value, int level) {
    Domain d;
    for (std::size_t i = 0; i < v._domain._domain.size (); ++i) {
        if (v._domain._domain[i] != value) {
            d.add(v._domain._domain[i]);
        }
    }
    bookKeep(level,v,d);
    v._value = value;
}

std::vector<Variable> SudokuSolver::getNeighbors(SudokuPuzzle puzzle, Position variable) {
    std::vector<Variable> neighbors;
    for (std::size_t x = 0; x < (std::size_t)puzzle.n(); ++x) {
        for (std::size_t y = 0; y < (std::size_t)puzzle.n(); ++y) {

            if ((x != variable._x || y != variable._y) && (x == variable._x || y == variable._y || withinBlock (puzzle, variable, puzzle.sudoku()[x][y]._position))) {
                neighbors.insert (neighbors.end (), puzzle.sudoku()[x][y]);
            }
        }
    }
    return neighbors;
}

bool SudokuSolver::withinBlock(SudokuPuzzle puzzle, Position variable1, Position variable2) {
    int x1 = variable1._x, x2 = variable2._x;
    int y1 = variable1._y, y2 = variable2._y;
    int t1 = (x1 / puzzle.p()) * puzzle.p(), t2 = (x2 / puzzle.p()) * puzzle.p();
    int b1 = t1 + puzzle.p() - 1, b2 = t2 + puzzle.p() - 1;
    int l1 = (y1 / puzzle.q()) * puzzle.q(), l2 = (y2 / puzzle.q()) * puzzle.q();
    int r1 = l1 + puzzle.q() - 1, r2 = l2 + puzzle.q() - 1;
    return (x1 == x2 || y1 == y2 || (t1 == t2 && b1 == b2 && l1 == l2 && r1 == r2));
}
