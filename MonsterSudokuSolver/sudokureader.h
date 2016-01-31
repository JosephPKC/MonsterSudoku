#ifndef SUDOKUREADER_H
#define SUDOKUREADER_H
#include <fstream>
#include "sudokugenerator.h"

//Input file for generating a sudoku is simply M N P Q
//Output file for saving a sudoku is N P Q \n Sudoku Puzzle Grid
class SudokuReader {
private:
    char* _inputFileName;
    char* _outputFileName;

    void destroyFileName (char *&fileName);

public:
    SudokuReader();
    SudokuReader (char* inputFileName, char* outputFileName);
    SudokuReader (const SudokuReader& S);
    SudokuReader& operator = (const SudokuReader& S);
    ~SudokuReader ();

    void setInputFileName (char* inputFileName);
    void setOutputFileName (char* outputFileName);

    char* getInputFileName () const;
    char* getOutputFileName () const;

    char** SudokuReader::loadSudoku (int& m, int& n, int& p, int& q, long timeout);
    char** loadSudoku (int& n, int& p, int& q, long timeout);
    void saveSudoku (int m, int n, int p, int q, char **sudoku, char e);
};

#endif // SUDOKUREADER_H
