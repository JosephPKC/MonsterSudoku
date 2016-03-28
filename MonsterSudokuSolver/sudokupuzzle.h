#ifndef SUDOKUPUZZLE_H
#define SUDOKUPUZZLE_H
#include <vector>
#include <iostream>
using namespace std;
const static int _tokenSize = 35;
const static char _alphabet[_tokenSize] = {
    49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90
};
struct Position {
    int _x;
    int _y;
    Position () {
        _x = 0;
        _y = 0;
    }
    Position (int x, int y) {
        _x = x;
        _y = y;
    }
    friend bool operator == (const Position& L, const Position& R) {
        return L._x == R._x && L._y == R._y;
    }
};

struct Domain {
    std::vector<char> _domain;
    Domain () {
        _domain = std::vector<char> ();
    }
    void add (char value) {
        _domain.insert (_domain.end(), value);
    }
};

struct Variable {
    Position _position;
    Domain _domain;
    char _value;
    Variable (Position position = Position (), Domain domain = Domain (), char value = '0') {
        _position = position;
        _domain = domain;
        _value = value;
    }
    friend bool operator == (const Variable& L, const Variable& R) {
        return L._position == R._position;
    }
    friend std::ostream& operator << (std::ostream& out, const Variable& R) {
        out << R._value;
        return out;
    }
};

class SudokuPuzzle {
private:
    Variable** _sudoku;
    int _m;
    int _n;
    int _p;
    int _q;

    void create (Variable *&array, int size);
    void create (Variable **&array, int size);

public:
    SudokuPuzzle();
    SudokuPuzzle (char** sudoku, int m, int n, int p, int q);

    Variable **sudoku() const;
    void setSudoku(Variable **sudoku);
    int m() const;
    void setM(int m);
    int n() const;
    void setN(int n);
    int p() const;
    void setP(int p);
    int q() const;
    void setQ(int q);

    Variable& access (int x, int y);
    void set (int x, int y, Variable v);

    bool consistent (int x, int y);
    bool complete ();

    void display ();
};

#endif // SUDOKUPUZZLE_H
