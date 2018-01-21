#include "puzzle.h"

Puzzle::Puzzle() {
	_sudoku = nullptr;
}

Puzzle::Puzzle(int m, int n, int p, int q) {
	_parameters = Parameters(m, n, p, q);
	_sudoku = utils::create<Cell>(getSize(), getSize());
	for(std::size_t x = 0; x < getSize(); ++x) {
		for(std::size_t y = 0; y < getSize(); ++y) {
			_sudoku[x][y] = Cell(n);
		}
	}
}

Puzzle::Puzzle(char** sudoku, int m, int n, int p, int q) {
	_parameters = Parameters(m, n, p, q);
	_sudoku = utils::create<Cell>(getSize(), getSize());

	for(std::size_t x = 0; x < getSize(); ++x) {
		for(std::size_t y = 0; y < getSize(); ++y) {
			/* Create a brand new Cell */
			if(sudoku[x][y] == '.') {
				_sudoku[x][y] = Cell(n);
			}
			else {
				/* Fill the cell with the given char */
				_sudoku[x][y] = Cell(n);
				int index = utils::convertCharToIndex(sudoku[x][y]);
				if(index != -1) {
					_sudoku[x][y].setValue((std::size_t)index);
				}
			}
		}
	}
}

Puzzle::Puzzle(const Puzzle& p) {
	_parameters = Parameters(p.getM(), p.getN(), p.getP(), p.getQ());
	/*  the sudoku */
	_sudoku = utils::makeCopy<Cell>(p.getSudoku(), getSize());
}

Puzzle& Puzzle::operator =(const Puzzle& p) {
	/* Delete old puzzle */
	utils::deleteArray<Cell>(_sudoku, getSize());
	_parameters = Parameters(p.getM(), p.getN(), p.getP(), p.getQ());
	/*  the sudoku */
	_sudoku = utils::makeCopy<Cell>(p.getSudoku(), getSize());
	return *this;
}

Puzzle::~Puzzle() {
	utils::deleteArray<Cell>(_sudoku, getSize());
}

Cell** Puzzle::getSudoku() const {
	return _sudoku;
}

Parameters Puzzle::getParameters() const {
	return _parameters;
}

int Puzzle::getM() const {
	return _parameters.m;
}

int Puzzle::getN() const {
	return _parameters.n;
}

int Puzzle::getP() const {
	return _parameters.p;
}

int Puzzle::getQ() const {
	return _parameters.q;
}

std::size_t Puzzle::getSize() const {
	return (std::size_t)_parameters.n;
}

Block Puzzle::getBlock(std::size_t x, std::size_t y) const {
	std::size_t t, b, l, r;
	t = (x / getP()) * getP();
	b = t + getP() - 1;
	l = (y / getQ()) * getQ();
	r = l + getQ() - 1;
	return Block(t, b, l, r);
}

Block Puzzle::getBlock(Position p) const {
	return getBlock(p.x, p.y);
}

std::size_t Puzzle::getDomainSize(std::size_t x, std::size_t y) const {
	if(x >= getSize() || y >= getSize()) {
		return getSize() + 1;
	}
	int count = 0;
	std::vector<bool> values = getDomain(x, y).values;
	for(utils::vb_it it = values.begin(); it != values.end(); ++it) {
		if(*it) {
			++count;
		}
	}
	return count;
}

std::size_t Puzzle::getDomainSize(const Position& p) const {
	return getDomainSize(p.x, p.y);
}

Domain Puzzle::getDomain(std::size_t x, std::size_t y) const {
	if(x >= getSize() || y >= getSize()) {
		return Domain();
	}
	return access(x, y).getDomain();
}

Domain Puzzle::getDomain(const Position& p) const {
	return getDomain(p.x, p.y);
}

std::vector<Position> Puzzle::getNeighbors(std::size_t x, std::size_t y, bool onlyEmpty) const {
	if(x >= getSize() || y >= getSize()) {
		return std::vector<Position>();
	}
	std::vector<Position> neighbors;
	/* Row & Column */
	for(std::size_t i = 0; i < getSize(); ++i) {
		if(i != y) {
			if(onlyEmpty) {
				if(isEmpty(x, i)) {
					neighbors.push_back(Position(x, i));
				}
			}
			else {
				neighbors.push_back(Position(x, i));
			}
		}
		if(i != x) {
			if(onlyEmpty) {
				if(isEmpty(i, y)) {
					neighbors.push_back(Position(i, y));
				}
			}
			else {
				neighbors.push_back(Position(i, y));
			}
		}
	}
	/* Box */
	Block perim = getBlock(x, y);
	for(std::size_t i = perim.t; i <= perim.b; ++i) {
		for(std::size_t j = perim.l; j <= perim.r; ++j) {
			/* Ignore the chosen cell, and ignore cells that were already in the same row and column */
			if(i != x && j != y && !isSameRow(i, j, x, y) && !isSameCol(i, j, x, y)) {
				if(onlyEmpty) {
					if(isEmpty(i, j)) {
						neighbors.push_back(Position(i, j));
					}
				}
				else {
					neighbors.push_back(Position(i, j));
				}
			}
		}
	}
	return neighbors;
}

std::vector<Position> Puzzle::getNeighbors(const Position& p, bool onlyEmpty) const {
	return getNeighbors(p.x, p.y, onlyEmpty);
}

std::vector<bool> Puzzle::getNeighborValues(std::size_t x, std::size_t y) const {
	/* Get a falsified value vector */
	std::vector<bool> values;
	for(int i = 0; i < getN(); ++i) {
		values.push_back(false);
	}

	for(std::size_t i = 0; i < getSize(); ++i) {
		/* Row */
		if(i != y && !isEmpty(x, i)) {
			values[access(x, i).getValue()] = true;
		}
		/* Col */
		if(i != x && !isEmpty(i, y)) {
			values[access(i, y).getValue()] = true;
		}
	}
	/* Get Block Indices */
	Block perim = getBlock(x, y);
	/* Check block */
	for(std::size_t i = perim.t; i <= perim.b; ++i) {
		for(std::size_t j = perim.l; j <= perim.r; ++j) {
			if(i != x && j != y && !isEmpty(i, j)) {
				values[access(i, j).getValue()] = true;
			}
		}
	}
	return values;
}

std::vector<bool> Puzzle::getNeighborValues(const Position& p) const {
	return getNeighborValues(p.x, p.y);
}

Cell& Puzzle::access(std::size_t x, std::size_t y) {
	if(x >= getSize()) {
		throw std::out_of_range("Index " + std::to_string(x) + " out of range. Size is " + std::to_string(getSize()) + ".");
	}
	if(y >= getSize()) {
		throw std::out_of_range("Index " + std::to_string(y) + " out of range. Size is " + std::to_string(getSize()) + ".");
	}
	return _sudoku[x][y];
}

Cell Puzzle::access(std::size_t x, std::size_t y) const {
	if(x >= getSize()) {
		throw std::out_of_range("Index " + std::to_string(x) + " out of range. Size is " + std::to_string(getSize()) + ".");
	}
	if(y >= getSize()) {
		throw std::out_of_range("Index " + std::to_string(y) + " out of range. Size is " + std::to_string(getSize()) + ".");
	}
	return _sudoku[x][y];
}

Cell& Puzzle::access(const Position& p) {
	return access(p.x, p.y);
}

Cell Puzzle::access(const Position& p) const {
	return access(p.x, p.y);
}

void Puzzle::set(std::size_t x, std::size_t y, std::size_t val) {
	if(x >= getSize() || y >= getSize()) {
		return;
	}
	_sudoku[x][y].setValue(val);
}

void Puzzle::set(const Position& p, std::size_t val) {
	set(p.x, p.y, val);
}

void Puzzle::restore(std::size_t x, std::size_t y, const Domain& domain) {
	if(x >= getSize() || y >= getSize()) {
		return;
	}
	_sudoku[x][y].setEmpty();
	_sudoku[x][y].setDomain(domain);
}

void Puzzle::restore(const Position& p, const Domain& domain) {
	restore(p.x, p.y, domain);
}

void Puzzle::reset() {
	for(std::size_t x = 0; x < getSize(); ++x) {
		for(std::size_t y = 0; y < getSize(); ++y) {
			reset(x, y);
		}
	}
}

void Puzzle::reset(std::size_t x, std::size_t y) {
	if(x >= getSize() || y >= getSize()) {
		return;
	}
	_sudoku[x][y].reset();
}

void Puzzle::reset(const Position& p) {
	reset(p.x, p.y);
}

std::ostream& operator << (std::ostream& out, const Puzzle& p) {
	for(std::size_t x = 0; x < p.getSize(); ++x) {
		if(x % p.getP() == 0) {
			for(std::size_t i = 0; i < (std::size_t)(2 * p.getN() + 2 * p.getP() + 1); ++i) {
				out << "-";
			}
			out << std::endl;
		}
		for(std::size_t y = 0; y < p.getSize(); ++y) {
			if(y % p.getQ() == 0) {
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
	for(std::size_t i = 0; i < (std::size_t)(2 * p.getN() + 2 * p.getP() + 1); ++i) {
		out << "-";
	}
	out << std::endl;
	return out;
}

bool Puzzle::isComplete() const {
	for(std::size_t x = 0; x < getSize(); ++x) {
		for(std::size_t y = 0; y < getSize(); ++y) {
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
	for(std::size_t x = 0; x < getSize(); ++x) {
		for(std::size_t y = 0; y < getSize(); ++y) {
			if(!isLegal(x, y)) {
				return false;
			}
		}
	}
	return true;
}

bool Puzzle::isLegal(std::size_t x, std::size_t y) const {
	/* Check a specific cell for legality */
	std::size_t c = access(x, y).getValue();
	if(c == getSize()) {
		return true;
	}
	/* Check row & column */
	for(std::size_t i = 0; i < getSize(); ++i) {
		/* Row */
		if(i != y && !isEmpty(x, i) && access(x, i).getValue() == c) {
			return false;
		}
		/* Col */
		if(i != x && !isEmpty(i, y) && access(i, y).getValue() == c) {
			return false;
		}
	}
	/* Get Block Indices */
	Block perim = getBlock(x, y);
	/* Check block */
	for(std::size_t i = perim.t; i <= perim.b; ++i) {
		for(std::size_t j = perim.l; j <= perim.r; ++j) {
			if(i != x && j != y && !isEmpty(i, j) && access(i, j).getValue() == c) {
				return false;
			}
		}
	}
	return true;
}

bool Puzzle::isLegal(const Position& p) const {
	return isLegal(p.x, p.y);
}

bool Puzzle::isEmpty(std::size_t x, std::size_t y) const {
	if(x >= getSize() || y >= getSize()) {
		throw std::out_of_range("out of range");
	}
	return access(x, y).getValue() == getSize();
}

bool Puzzle::isEmpty(const Position& p) const {
	return isEmpty(p.x, p.y);
}

bool Puzzle::isSameRow(std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2) const {
	return x1 == x2 && y1 != y2;
}

bool Puzzle::isSameRow(const Position& p1, const Position& p2) const {
	return isSameRow(p1.x, p1.y, p2.x, p2.y);
}

bool Puzzle::isSameCol(std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2) const {
	return x1 != x2 && y1 == y2;
}

bool Puzzle::isSameCol(const Position& p1, const Position& p2) const {
	return isSameCol(p1.x, p2.y, p2.x, p2.y);
}

bool Puzzle::isSameBox(std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2) const {
	return getBlock(x1, y1) == getBlock(x2, y2);
}

bool Puzzle::isSameBox(const Position& p1, const Position& p2) const {
	return isSameBox(p1.x, p1.y, p2.x, p2.y);
}
