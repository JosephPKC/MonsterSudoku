#ifndef PUZZLE_H
#define PUZZLE_H
#include "cell.h"

struct Parameters {
	int m, n, p, q;
	Parameters() {
		m = 0;
		n = 0;
		p = 0;
		q = 0;
	}

	Parameters(int m, int n, int p, int q) {
		this->m = m;
		this->n = n;
		this->p = p;
		this->q = q;
	}

	friend std::ostream& operator << (std::ostream& out, const Parameters& p) {
		out << p.m << " " << p.n << " " << p.p << " " << p.q;
		return out;
	}
};

struct Box {
	std::size_t t, b, l, r;
	Box() {
		t = 0;
		b = 0;
		l = 0;
		r = 0;
	}

	Box(std::size_t t, std::size_t b, std::size_t l, std::size_t r) {
		this->t = t;
		this->b = b;
		this->l = l;
		this->r = r;
	}

	friend bool operator ==(const Box& l, const Box& r) {
		return l.t == r.t && l.b == r.b && l.l == r.l && l.r == r.r;
	}
};

struct Position {
	int x, y;
	Position() {
		x = 0;
		y = 0;
	}

	Position(int x, int y) {
		this->x = x;
		this->y = y;
	}

	friend bool operator == (const Position& l, const Position& r) {
		return l.x == r.x && l.y == r.y;
	}

	friend std::ostream& operator <<(std::ostream& out, const Position& p) {
		out << p.x << "," << p.y;
		return out;
	}
};

class Puzzle {
public:
	Puzzle();
	Puzzle(int m, int n, int p, int q);
	Puzzle(char** sudoku, int m, int n, int p, int q);
	Puzzle(const Puzzle& p);
	Puzzle& operator =(const Puzzle& p);
	~Puzzle();

	Cell** getSudoku() const;
	Parameters getParameters() const;
	int getM() const;
	int getN() const;
	int getP() const;
	int getQ() const;
	std::size_t getSize() const;
	Box getBox(std::size_t x, std::size_t y) const;
	Box getBox(Position p) const;
	std::size_t getDomainSize(std::size_t x, std::size_t y) const;
	std::size_t getDomainSize(Position p) const;
	Domain getDomain(std::size_t x, std::size_t y) const;
	Domain getDomain(Position p) const;

	std::vector<Position> getNeighbors(std::size_t x, std::size_t y, bool onlyEmpty = false) const;
	std::vector<Position> getNeighbors(Position p, bool onlyEmpty = false) const;
	std::vector<bool> getNeighborValues(std::size_t x, std::size_t y) const;
	std::vector<bool> getNeighborValues(Position p) const;

	Cell& access(std::size_t x, std::size_t y);
	Cell access(std::size_t x, std::size_t y) const;
	Cell& access(Position p);
	Cell access(Position p) const;
	void set(std::size_t x, std::size_t y, std::size_t val);
	void set(Position p, std::size_t val);
	void restore(std::size_t x, std::size_t y, Domain domain);
	void restore(Position p, Domain domain);

	void reset();
	void reset(std::size_t x, std::size_t y);
	void reset(Position p);

	friend std::ostream& operator << (std::ostream& out, const Puzzle& p);

	bool isComplete() const;
	bool isSolved() const;
	bool isLegal() const;

	bool isLegal(std::size_t x, std::size_t y) const;
	bool isLegal(Position p) const;
	bool isEmpty(std::size_t x, std::size_t y) const;
	bool isEmpty(Position p) const;
	bool isSameRow(std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2) const;
	bool isSameRow(Position p1, Position p2) const;
	bool isSameCol(std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2) const;
	bool isSameCol(Position p1, Position p2) const;
	bool isSameBox(std::size_t x1, std::size_t y1, std::size_t x2, std::size_t y2) const;
	bool isSameBox(Position p1, Position p2) const;
private:
	Parameters _parameters;
	Cell** _sudoku;
};


#endif // PUZZLE_H
