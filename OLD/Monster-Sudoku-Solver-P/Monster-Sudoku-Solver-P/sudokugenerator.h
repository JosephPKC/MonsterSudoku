#ifndef SUDOKUGENERATOR_H
#define SUDOKUGENERATOR_H
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cassert>
#include <iostream>

namespace Error {
    const char kInputError = '!';
    const char kTimeoutError = '?';
    const char kNoError = '0';
    const char kFailure = '$';
    const char kSelectNextVariableEmpty = '#';
    const char kEmptyDomain = '%';
}
//const static char kTokenAlphabet[35] = {'0','1','2','3','4','5','6','7','8','9','10'};


const static int kTokenMax = 34;
const static char kInitialToken = '0';





class SudokuGenerator {
private:
    int _m;
    int _n;
    int _p;
    int _q;

    bool checkInputConstraints (int m, int n, int p, int q) const;
    char** createSudoku (int n) const;

    std::pair<int, int> randomCell(char **sudoku, std::vector<std::pair<int, int> > &cells) const;
    bool isEmpty (int x, int y, char **sudoku) const;
    int randomValue (int x, int y, char **sudoku) const;
    bool checkValueConstraints (int x, int y, int v, char** sudoku) const;
    void findBlockPerimeter (int x, int y, int& t, int& b, int& r, int& l) const;

    std::vector<std::pair<int,int>> loadCells (int n) const;
    void removeFromCell (std::vector<std::pair<int,int>>& vector, int pos) const;
    std::vector<char> loadValues(int n) const;
    void removeFromValue (std::vector<char> &vector, int pos) const;

//    bool selectCell (int& x, int& y) const;
//    int selectValue (int x, int y, char** sudoku) const;
    void destroySudoku (char **&sudoku) const;



    //Move

public:
    SudokuGenerator();
    SudokuGenerator (int m, int n, int p, int q);

    void setPresetCellNumber (int m);
    void setSudokuSize (int n);
    void setBlockRowSize (int p);
    void setBlockColumnSize (int q);

    int getPresetCellNumber () const;
    int getSudokuSize () const;
    int getBlockRowSize () const;
    int getBlockColumnSize () const;

    char** generateSudoku (long timeout) const;
};


#endif // SUDOKUGENERATOR_H
