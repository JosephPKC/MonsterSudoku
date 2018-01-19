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

struct Perimeter {
	std::size_t t, b, l, r;
	Perimeter() {
		t = 0;
		b = 0;
		l = 0;
		r = 0;
	}

	Perimeter(std::size_t t, std::size_t b, std::size_t l, std::size_t r) {
		this->t = t;
		this->b = b;
		this->l = l;
		this->r = r;
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

	Cell** sudoku() const;

	Parameters parameters() const;

	int m() const;
	int n() const;
	int p() const;
	int q() const;

	std::size_t size() const;

	Cell& access(std::size_t x, std::size_t y);
	Cell access(std::size_t x, std::size_t y) const;
	void set(std::size_t x, std::size_t y, std::size_t val);
	void restore(std::size_t x, std::size_t y, Domain domain);

	void reset();
	void reset(std::size_t x, std::size_t y);

	std::vector<bool> neighborValues(std::size_t x, std::size_t y) const;

	friend std::ostream& operator << (std::ostream& out, const Puzzle& p);

	bool isComplete() const;
	bool isSolved() const;

	bool isLegal() const;
	bool isLegal(std::size_t x, std::size_t y) const;

	bool isEmpty(std::size_t x, std::size_t y) const;

	Perimeter perimeter(std::size_t x, std::size_t y) const;
private:
	Parameters _parameters;
	Cell** _sudoku;

	void _create(Cell*& array, std::size_t size);
	void _create(Cell**& array, std::size_t size);
	Cell** _copy(Cell** puzzle, std::size_t size);
	void _delete(Cell**& puzzle, std::size_t size);


};

#endif // PUZZLE_H
