#include "sudokupuzzle.h"
using namespace std;
void SudokuPuzzle::create(Variable *&array, int size)
{
    array = new Variable[size];
}

void SudokuPuzzle::create(Variable **&array, int size)
{
    array = new Variable*[size];
    for (std::size_t i = 0; i < size; ++i) {
        create (array[i], size);
    }
}

SudokuPuzzle::SudokuPuzzle()
{
    _m = 0;
    _n = 0;
    _p = 0;
    _q = 0;
    _sudoku = nullptr;
}

SudokuPuzzle::SudokuPuzzle(char **sudoku, int m, int n, int p, int q)
{
//    cout << "Inside Puzzle Constructor" << endl;
//    cout << "n=" << n << endl;
    _m = m;
    _n = n;
    _p = p;
    _q = q;
//    cout << "Creating the array" << endl;
    create (_sudoku, _n);
    for (std::size_t x = 0; x < _n; ++x) {
        for (std::size_t y = 0; y < _n; ++y) {
//            cout << "x=" << x << ",y=" << y << endl;
            Position P (x,y);
            Domain D;
            if (sudoku[x][y] == '0') {
                for (std::size_t i = 0; i < _n; ++i) {
                    D.add(_alphabet[i]);
                }
            }
            else {
                D.add(sudoku[x][y]);
            }
            Variable V (P,D,sudoku[x][y]);
            _sudoku[x][y] = V;
        }
    }
}

int SudokuPuzzle::n() const
{
    return _n;
}

void SudokuPuzzle::setN(int n)
{
    _n = n;
}

Variable **SudokuPuzzle::sudoku() const
{
    return _sudoku;
}

void SudokuPuzzle::setSudoku(Variable **sudoku)
{
    _sudoku = sudoku;
}

int SudokuPuzzle::m() const
{
    return _m;
}

void SudokuPuzzle::setM(int m)
{
    _m = m;
}

int SudokuPuzzle::p() const
{
    return _p;
}

void SudokuPuzzle::setP(int p)
{
    _p = p;
}

int SudokuPuzzle::q() const
{
    return _q;
}

void SudokuPuzzle::setQ(int q)
{
    _q = q;
}

Variable &SudokuPuzzle::access(int x, int y)
{
    return _sudoku[x][y];
}


void SudokuPuzzle::set(int x, int y, Variable v)
{

}

bool SudokuPuzzle::consistent(int x, int y)
{
    //???????????
}

bool SudokuPuzzle::complete()
{
//    cout << "In Complete" << endl;
    for (std::size_t x = 0; x < _n; ++x) {
        for (std::size_t y = 0; y < _n; ++y) {
//            std::cout << "x=" << x << "," << "y=" << y << ":" << "v=" << _sudoku[x][y]._value << std::endl;
            if (_sudoku[x][y]._value == '0') {

                return false;
            }
        }
    }
    return true;
}

void SudokuPuzzle::display()
{
    for (std::size_t x = 0; x < _n; ++x) {
        std::cout << std::endl;
        if (x % _p == 0) {
            for (std::size_t i = 0; i < 2 * _n + _q - 1; ++i) {
                std::cout << "-";
            }
            std::cout << std::endl;
        }
        for (std::size_t y = 0; y < _n; ++y) {
            if (y % _q == 0)
                std::cout << "|";
            if (_sudoku[x][y]._value == '0')
                std::cout << "  ";
            else
                std::cout << _sudoku[x][y] << " ";
        }

    }
    std::cout << std::endl;
}


