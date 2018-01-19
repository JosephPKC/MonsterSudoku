#include "sudokugenerator.h"

SudokuGenerator::SudokuGenerator() {
    srand (time (0));
    setPresetCellNumber(0);
    setSudokuSize(0);
    setBlockRowSize(0);
    setBlockColumnSize(0);
}

SudokuGenerator::SudokuGenerator (int m, int n, int p, int q) {
    srand (time (0));
    if (!checkInputConstraints (m,n,p,q)) {
        throw  Error::kInputError;
    }
    _m = m;
    _n = n;
    _p = p;
    _q = q;

}

void SudokuGenerator::setPresetCellNumber (int m) {
    assert (m <= _n * _n);
    _m = m;
}

void SudokuGenerator::setSudokuSize (int n) {
    assert (checkInputConstraints (_m,n,_p,_q));
    _n = n;
}

void SudokuGenerator::setBlockRowSize (int p) {
    assert (p * _q == _n);
    _p = p;
}

void SudokuGenerator::setBlockColumnSize (int q) {
    assert (_p * q == _n);
    _q = q;
}

int SudokuGenerator::getPresetCellNumber () const {
    return _m;
}

int SudokuGenerator::getSudokuSize () const {
    return _n;
}

int SudokuGenerator::getBlockRowSize () const {
    return _p;
}

int SudokuGenerator::getBlockColumnSize () const {
    return _q;
}

char **SudokuGenerator::generateSudoku(long timeout) const {
    bool fail = true;
    char** sudoku = createSudoku (_n);
    const clock_t start = clock ();
    while (fail) {
        if ((clock () - start) / CLOCKS_PER_SEC >= timeout) {
            throw Error::kTimeoutError;
        }
        std::vector<std::pair<int,int>> cells = loadCells (_n);
        bool restart = false;
        for (std::size_t i = 0; i < (std::size_t) _m; ++i) {

            std::pair<int,int> cell = randomCell (sudoku, cells);
            if (cell.first != -1) {
                int value = randomValue (cell.first,cell.second,sudoku);
                if (value != -1) {
                    sudoku[cell.first][cell.second] = value;
                }
                else {
                    destroySudoku (sudoku);
                    sudoku = createSudoku (_n);
                    restart = true;
                }
            }
            else {
                //There are no more cells available.
                //This means that for some reason, m > n^2
                std::cout << "No more available cells for generation; m > n^2" << std::endl;
            }
            if (restart) {
                i = _m + 1;
            }
        }
        if (!restart) {
            fail = false;
        }
    }
    return sudoku;
}

bool SudokuGenerator::checkInputConstraints (int m, int n, int p, int q) const {
    return (p * q == n) && (m <= n * n) && n <= kTokenMax && m >= 0 && p >= 0 && q >= 0 && n >= 0;
}

char **SudokuGenerator::createSudoku(int n) const {
    char** sudoku = new char*[n];
    for (std::size_t x = 0; x < (std::size_t) n; ++x) {
        sudoku[x] = new char[n];
        for (std::size_t y = 0; y < (std::size_t) n; ++y) {
            sudoku[x][y] = kInitialToken;
        }
    }
    return sudoku;
}

std::pair<int,int> SudokuGenerator::randomCell (char** sudoku, std::vector<std::pair<int,int>>& cells) const {
    while (!cells.empty ()) {
        int pos = rand () % (int) cells.size();
        if (isEmpty (cells[pos].first, cells[pos].second, sudoku)) {
            std::pair<int,int> cell = cells[pos];
            removeFromCell (cells, pos);
            return cell;
        }
        else {
            removeFromCell (cells, pos);
        }
    }
    return std::make_pair (-1,-1);
}

bool SudokuGenerator::isEmpty (int x, int y, char **sudoku) const {
    return sudoku[x][y] == kInitialToken;
}

int SudokuGenerator::randomValue (int x, int y, char **sudoku) const {
    std::vector<char> values = loadValues (_n);
    while (!values.empty ()) {
        int pos = rand () % (int) values.size ();
        if (checkValueConstraints (x,y,values[pos],sudoku)) {
            return values[pos];
        }
        else {
            removeFromValue (values, pos);
        }
    }
    return -1;
}

bool SudokuGenerator::checkValueConstraints (int x, int y, int v, char **sudoku) const {
    for (std::size_t i = 0; i < (std::size_t) _n; ++i) {
        if (sudoku[x][i] == v) {
            return false;
        }
        if (sudoku[i][y] == v) {
            return false;
        }
    }
    int t, b, r, l;
    findBlockPerimeter (x,y,t,b,r,l);
    for (std::size_t i = t; i <= (std::size_t) b; ++i) {
        for (std::size_t j = l; j <= (std::size_t) r; ++j) {
            if (sudoku[i][j] == v) {
                return false;
            }
        }
    }
    return true;
}

void SudokuGenerator::findBlockPerimeter(int x, int y, int &t, int &b, int &r, int &l) const {
    t = (x / _p) * _p;
    b = t + _p - 1;
    l = (y / _q) * _q;
    r = l + _q - 1;
}

std::vector<std::pair<int,int>> SudokuGenerator::loadCells (int n) const {
    std::vector<std::pair<int,int>> cells;
    for (std::size_t x = 0; x < (std::size_t) n; ++x) {
        for (std::size_t y = 0; y < (std::size_t) n; ++y) {
            cells.insert(cells.end (), std::make_pair (x,y));
        }
    }
    return cells;
}

void SudokuGenerator::removeFromCell (std::vector<std::pair<int,int>>& vector, int pos) const {
    vector.erase (vector.begin () + pos);
}

std::vector<char> SudokuGenerator::loadValues (int n) const {
    std::vector<char> values;
    for (std::size_t i = 0; i < (std::size_t) n && i < (std::size_t) kTokenMax; ++i) {
        int v = i + kInitialToken;
        if (v > 57 && v < 65) {
            v += 65 - 57;
        }
        values.insert (values.end (), v);
    }
    return values;
}

void SudokuGenerator::removeFromValue (std::vector<char>& vector, int pos) const {
    vector.erase (vector.begin () + pos);
}

void SudokuGenerator::destroySudoku (char **&sudoku) const {
    for (std::size_t i = 0; i < (std::size_t) _n; ++i) {
        delete[] sudoku[i];
    }
    delete[] sudoku;
    sudoku = nullptr;
}
