#include "puzzle.h"

Puzzle::Puzzle() {
	_sudoku = nullptr;
}

Puzzle::Puzzle(int m, int n, int p, int q) {
	_parameters = Parameters(m, n, p, q);
	_create(_sudoku, n);
	for(std::size_t x = 0; x < size(); ++x) {
		for(std::size_t y = 0; y < size(); ++y) {
			_sudoku[x][y] = Cell(n);
		}
	}
}

Puzzle::Puzzle(char** sudoku, int m, int n, int p, int q) {
	_parameters = Parameters(m, n, p, q);
	_create(_sudoku, n);

	for(std::size_t x = 0; x < size(); ++x) {
		for(std::size_t y = 0; y < size(); ++y) {
			/* Create a brand new Cell */
			if(sudoku[x][y] == '.') {
				_sudoku[x][y] = Cell(n);
			}
			else {
				/* Fill the cell with the given char */
				_sudoku[x][y] = Cell(n);
				int index = utils::convertCharToIndex(sudoku[x][y]);
				if(index != -1) {
					_sudoku[x][y].setChosen((std::size_t)index);
				}
			}
		}
	}
}

Puzzle::Puzzle(const Puzzle& p) {
	_parameters = Parameters(p.m(), p.n(), p.p(), p.q());
	/* Copy the sudoku */
	_sudoku = _copy(p.sudoku(), size());
}

Puzzle& Puzzle::operator =(const Puzzle& p) {
	/* Delete old puzzle */
	_delete(_sudoku, size());
	_parameters = Parameters(p.m(), p.n(), p.p(), p.q());
	/* Copy the sudoku */
	_sudoku = _copy(p.sudoku(), size());
	return *this;
}

Puzzle::~Puzzle() {
	_delete(_sudoku, size());
}

Cell** Puzzle::sudoku() const {
	return _sudoku;
}

Parameters Puzzle::parameters() const {
	return _parameters;
}

int Puzzle::m() const {
	return _parameters.m;
}

int Puzzle::n() const {
	return _parameters.n;
}

int Puzzle::p() const {
	return _parameters.p;
}

int Puzzle::q() const {
	return _parameters.q;
}

std::size_t Puzzle::size() const {
	return (std::size_t)_parameters.n;
}


Cell& Puzzle::access(std::size_t x, std::size_t y) {
	if(x >= size()) {
		throw std::out_of_range("Index " + std::to_string(x) + " out of range. Size is " + std::to_string(size()) + ".");
	}
	if(y >= size()) {
		throw std::out_of_range("Index " + std::to_string(y) + " out of range. Size is " + std::to_string(size()) + ".");
	}
	return _sudoku[x][y];
}

Cell Puzzle::access(std::size_t x, std::size_t y) const {
	if(x >= size()) {
		throw std::out_of_range("Index " + std::to_string(x) + " out of range. Size is " + std::to_string(size()) + ".");
	}
	if(y >= size()) {
		throw std::out_of_range("Index " + std::to_string(y) + " out of range. Size is " + std::to_string(size()) + ".");
	}
	return _sudoku[x][y];
}

void Puzzle::set(std::size_t x, std::size_t y, std::size_t val) {
	if(x >= size() || y >= size()) {
		return;
	}
	_sudoku[x][y].setChosen (val);
}

void Puzzle::restore(std::size_t x, std::size_t y, Domain domain) {
	if(x >= size() || y >= size()) {
//		std::cout << "x & y are too big" << std::endl;
		return;
	}
	_sudoku[x][y].setEmpty();
//	std::cout << "new val: " << _sudoku[x][y].getVal() << std::endl;
	_sudoku[x][y].setDomain(domain);
}

void Puzzle::reset() {
	for(std::size_t x = 0; x < size(); ++x) {
		for(std::size_t y = 0; y < size(); ++y) {
			reset(x, y);
		}
	}
}

void Puzzle::reset(std::size_t x, std::size_t y) {
	if(x >= size() || y >= size()) {
		return;
	}
	_sudoku[x][y].reset();
}

std::vector<bool> Puzzle::neighborValues(std::size_t x, std::size_t y) const {
	std::vector<bool> values;
	for(int i = 0; i < size(); ++i) {
		values.push_back(false);
	}

	for(std::size_t i = 0; i < size(); ++i) {
		/* Row */
		if(i != y && !isEmpty(x, i)) {
			values[access(x, i).getVal()] = true;
		}
		/* Col */
		if(i != x && !isEmpty(i, y)) {
			values[access(i, y).getVal()] = true;
		}
	}
	/* Get Block Indices */
	Perimeter perim = perimeter(x, y);
	/* Check block */
	for(std::size_t i = perim.t; i <= perim.b; ++i) {
		for(std::size_t j = perim.l; j <= perim.r; ++j) {
			if(i != x && j != y && !isEmpty(i, j)) {
				values[access(i, j).getVal()] = true;
			}
		}
	}
	return values;
}

std::ostream& operator << (std::ostream& out, const Puzzle& p) {
	for(std::size_t x = 0; x < p.size(); ++x) {
		if(x % p.p() == 0) {
			for(std::size_t i = 0; i < 2 * p.n() + 2 * p.p() + 1; ++i) {
				out << "-";
			}
			out << std::endl;
		}
		for(std::size_t y = 0; y < p.size(); ++y) {
			if(y % p.q() == 0) {
				out << "| ";
			}
			try {
				out << p.access(x, y) << " ";
			}
			catch(std::out_of_range e) {
				std::cerr << "Out of Range: " + std::string(e.what()) << std::endl;
				return out;
			}
		}
		out << "|";
		out << std::endl;
	}
	for(std::size_t i = 0; i < 2 * p.n() + 2 * p.p() + 1; ++i) {
		out << "-";
	}
	out << std::endl;
	return out;
}

bool Puzzle::isComplete() const {
	for(std::size_t x = 0; x < size(); ++x) {
		for(std::size_t y = 0; y < size(); ++y) {
			if(!_sudoku[x][y].isComplete()) {
				return false;
			}
		}
	}
	return true;
}

bool Puzzle::isSolved() const {
	return isComplete() && isLegal();
}

bool Puzzle::isLegal() const {
	/* Check each cell to see if ts legal */
	for(std::size_t x = 0; x < size(); ++x) {
		for(std::size_t y = 0; y < size(); ++y) {
			if(!isLegal(x, y)) {
				return false;
			}
		}
	}
	return true;
}

bool Puzzle::isLegal(std::size_t x, std::size_t y) const {
	/* Check a specific cell for legality */
	std::size_t c = access(x, y).getVal();
	if(c == size()) {
		return true;
	}
	/* Check row & column */
	for(std::size_t i = 0; i < size(); ++i) {
		/* Row */
		if(i != y && !isEmpty(x, i) && access(x, i).getVal() == c) {
//			std::cout << "Violated Row: " << x << i << std::endl;
			return false;
		}
		/* Col */
		if(i != x && !isEmpty(i, y) && access(i, y).getVal() == c) {
//			std::cout << "Violated Col: " << i << y << std::endl;
			return false;
		}
	}
	/* Get Block Indices */
	Perimeter perim = perimeter(x, y);
	/* Check block */
	for(std::size_t i = perim.t; i <= perim.b; ++i) {
		for(std::size_t j = perim.l; j <= perim.r; ++j) {
			if(i != x && j != y && !isEmpty(i, j) && access(i, j).getVal() == c) {
//				std::cout << "Violated BLock: " << i << y << std::endl;
				return false;
			}
		}
	}
	return true;
}

bool Puzzle::isEmpty(std::size_t x, std::size_t y) const {
	if(x >= size() || y >= size()) {
		throw std::out_of_range("out of range");
	}
	return access(x, y).getVal() == size();
}

Perimeter Puzzle::perimeter(std::size_t x, std::size_t y) const {
	std::size_t t, b, l, r;
	t = (x / p()) * p();
	b = t + p() - 1;
	l = (y / q()) * q();
	r = l + q() - 1;
	return Perimeter(t, b, l, r);
}

void Puzzle::_create(Cell*& array, std::size_t size) {
	array = new Cell[size];
}

void Puzzle::_create(Cell**& array, std::size_t size) {
	array = new Cell*[size];
	for(std::size_t i = 0; i < size; ++i) {
		_create(array[i], size);
	}
}

Cell** Puzzle::_copy(Cell** puzzle, std::size_t size) {
	Cell** copy;
	_create(copy, size);
	for(std::size_t x = 0; x < size; ++x) {
		for(std::size_t y = 0; y < size; ++y) {
			copy[x][y] = puzzle[x][y];
		}
	}
	return copy;
}

void Puzzle::_delete(Cell**& puzzle, std::size_t size) {
	for(std::size_t i = 0; i < size; ++i) {
		delete[] puzzle[i];
		puzzle[i] = nullptr;
	}
	delete[] puzzle;
	puzzle = nullptr;
}
