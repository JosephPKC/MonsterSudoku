#ifndef CELL_H
#define CELL_H
#include "utility.h"

struct Domain {
	// This way is faster than using integer vectors that remove. That will require O(n) searches constantly. This requires an O(1) access instead. Not to mention this uses less space (bool ~ 1 bit compared to int ~ 32 bits).
	std::vector<bool> domain;	//A vector of size N. Each index relates to a domain value (the index). Marked as true or false depending on whether it can be used or not.
	Domain() {
		// Nothing
	}
	Domain(int size) {
		for(int i = 0; i < size; ++i) {
			domain.push_back(true);
		}
	}

	friend std::ostream& operator <<(std::ostream& out, const Domain& d) {
		for(utils::vb_cit it = d.domain.begin(); it != d.domain.end(); ++it) {
			out << *it << " ";
		}
		return out;
	}
};

class Cell {
public:
	Cell();
	Cell(int size);

	Domain getDomain();
	void setDomain(Domain domain);
	void set(std::size_t value, bool val);
	void setChosen(std::size_t value);

	void reset();

	std::size_t getVal() const;

	bool isComplete() const;

	friend std::ostream& operator << (std::ostream& out, const Cell& c);
private:
	Domain _domain;
	std::size_t _chosen;
	std::size_t _size;
};

#endif // CELL_H
