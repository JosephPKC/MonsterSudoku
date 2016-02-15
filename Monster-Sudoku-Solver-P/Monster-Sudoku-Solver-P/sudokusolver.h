#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H
#include <map>
#include <vector>
#include <fstream>
#include "sudokugenerator.h"
#include "bookkeeper.h"
#include "logger.h"
namespace HeuristicFlag {
    enum HeuristicFlag {
        kMRV,
        kDH,
        kLCV,
        kACP,
        kMAC,
        kFC
    };
}


class SudokuSolver {
private:
    std::map<HeuristicFlag::HeuristicFlag,bool> _flags;
    BookKeeper _bookKeep;

    char backTrackSearch(SudokuPuzzle& puzzle, int &level, float timeout, int &nodes, int &deadends, float start);
    Variable selectNextVariable (SudokuPuzzle puzzle);
    Domain orderDomainValues (SudokuPuzzle puzzle, Variable variable);
    bool checkConsistency (SudokuPuzzle puzzle, Position position, char value);
    bool checkCompleteness (SudokuPuzzle puzzle);

    void bookKeep (int level, Variable &variable, Domain values);
    void undoLastAction (SudokuPuzzle& puzzle);
    void forwardCheck (SudokuPuzzle& puzzle, Variable variable, int level);
    std::vector<Variable> applyMRV (SudokuPuzzle puzzle, std::vector<Variable> unassigned);
    std::vector<Variable> applyDH (SudokuPuzzle puzzle, std::vector<Variable> unassigned);
    Domain applyLCV (SudokuPuzzle puzzle, Variable variable);
    void applyAC3 (SudokuPuzzle& puzzle);

    int getDegree (SudokuPuzzle puzzle, Variable variable);
    int getRemainingValues  (SudokuPuzzle puzzle, Variable variable);
    int getConstraints (SudokuPuzzle puzzle, Variable variable, char value);

    bool checkAllDif (SudokuPuzzle puzzle, Position position, char value);
    bool checkRow (SudokuPuzzle puzzle, Position position, char value);
    bool checkColumn (SudokuPuzzle puzzle, Position position, char value);
    bool checkBox (SudokuPuzzle puzzle, Position position, char value);

    std::vector<Variable> getUnassignedVariables (SudokuPuzzle puzzle);
    int findSmallest (std::vector<int> list);
    int findLargest (std::vector<int> list);
    bool checkArc (SudokuPuzzle puzzle, Variable variable1, Variable variable2);
    void assignValue (Variable &v, char value, int level);
    std::vector<Variable> getNeighbors (SudokuPuzzle puzzle, Variable variable);
    bool withinBlock (SudokuPuzzle puzzle, Variable variable1, Variable variable2);

public:
    SudokuSolver ();

    void setFlag (HeuristicFlag::HeuristicFlag flag, bool value);
    bool getFlag (HeuristicFlag::HeuristicFlag flag);

    LogBundle solve (SudokuPuzzle puzzle, float timeout, char &error);
};


















































//struct Variable {
//    std::pair<int,int> _position;
//    std::vector<char> _domain;
//    Variable (std::pair<int,int> position = std::make_pair<int,int> (0,0), std::vector<char> domain = std::vector<char> ()) {
//        _position = position;
//        _domain = domain;
//    }
//    friend bool operator == (const Variable& L, const Variable& R) {
//        return L._position == R._position;
//    }
//};
//typedef std::vector<std::pair<Variable,std::vector<char>>> RemovalRecord;
//struct Record {
//    std::pair<Variable,char> _assignment;
//    RemovalRecord _record;
//    Record (std::pair<Variable,char> assignment, RemovalRecord record) {
//        _assignment = assignment;
//        _record = record;
//    }
//    Record () {
//        _assignment = std::make_pair<Variable,char> (Variable (), '0');
//        _record = RemovalRecord ();
//    }
//};

//typedef HeuristicFlag::HeuristicFlag HF;
//typedef std::vector<std::pair<Variable,char>> Assignment;

//class SudokuSolver {
//private:
//    Variable selectNextVariable (std::vector<Variable> unassignedVariables, Assignment assignment);
//    std::vector<char> orderDomainValues (Variable variable, Assignment assignment);
//    Assignment recursiveBackTrack (Assignment &assignment, char** sudoku, int n, std::vector<Variable> &unassignedVariables, int p, int q, int d, float timeout, const clock_t start, int &nodes, int &backtracks);
//    void bookKeep (std::pair<Variable,char> assignment, RemovalRecord removals);

//    std::vector<Variable> applyMRV (std::vector<Variable> potentialVariables);
//    std::vector<Variable> applyDH (std::vector<Variable> potentialVariables, char** sudoku, int n);
//    std::vector<char> applyLCV (Variable variable, char** sudoku, int n);
//    void applyAC3 (char** sudoku, int n);
//    void applyFC (char** sudoku, int n);

//    std::vector<Variable> getUnassignedVariables (char** sudoku, int n) const;
//    bool checkAllDifConstraint (int x, int y, int v, char** sudoku, int n, int p, int q, Assignment current) const;
//    bool isComplete (Assignment assignment, char** sudoku, int n);
//    void applySolution (Assignment assignment, char** sudoku, int n);
//    void printLog (char* fileName, float start, float ppStart, float ppFinish, float searchStart, float searchFinish, int status, Assignment solution, int nodes, int backtracks, bool solved, int n);

//    char* _alphabet;
//    std::map<HF,bool> _flags;
//    std::vector<Record> _recordBook;



//    std::vector<char> getTokenAlphabet (int n) const{
//        std::vector<char> tokens;
//        for (std::size_t i = 1; i <= (std::size_t) n && i <= (std::size_t) kTokenMax; ++i) {
//            int v = i + kInitialToken;
//            if (v > 57 && v < 65) {
//                v += 65 - 57;
//            }
//            tokens.insert (tokens.end (), v);
//        }
//        return tokens;
//    }

//    int getTokenPosition (char token, std::vector<char> alphabet) {
//        for (std::size_t i = 0; i < alphabet.size(); ++i) {
//            if (token == alphabet[i]) {
//                return i;
//            }
//        }
//        return -1;
//    }
//public:
//    SudokuSolver();
//    SudokuSolver (char* alphabet);
//    SudokuSolver (const SudokuSolver& S);
//    SudokuSolver& operator = (const SudokuSolver& S);
//    ~SudokuSolver ();

//    char* getAlphabet ();
//    void setAlphabet (char* alphabet);
//    bool getFlag (HF flag);
//    void setFlag(HF flag, bool value);
//    std::vector<Record> getRecordBook ();

//    char** solveSudoku (char** sudoku, int n, int p, int q, int timeout, char *fileName);


//};
//Use backtracking search WITH
//MRV and DH for ordering variables
//LCV for ordering values
//Forward Checking, PAC, and MAC for constraint propagation
//Bookkeeping for Failures
//Create contraint graphs

/* Backtracking Search Algorithm
 * Take in as input: current assignment, the problem (as a graph)
 * if the assignment is complete, then return it
 * Perform ACP.
 * SELECT an unassigned variable using MRV, DH
 * For each value in the Selected variable's domain, SELECT unassigned value using LCV
 *  If the value is consistent with assignment according to the constraints, assign the value to the variable
 *  Perform FC, MAC
 *  Bookkeep the changes
 *  recursively go again
 *  If the recursive call returns success, then return the result
 *  Remove the value from the assignment, and revert the changes via bookkeep
 * If the loop ends here, return failure
 */
#endif // SUDOKUSOLVER_H
