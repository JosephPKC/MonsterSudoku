#ifndef CELL_H
#define CELL_H
#include "utility.h"

struct Domain {
	// This way is faster than using integer vectors that remove. That will require O(n) searches constantly. This requires an O(1) access instead. Not to mention this uses less space (bool ~ 1 bit compared to int ~ 32 bits).
	std::vector<bool> values;
	Domain() {
		// Nothing
	}
	Domain(int size) {
		for(int i = 0; i < size; ++i) {
			values.push_back(true);
		}
	}

	friend std::ostream& operator <<(std::ostream& out, const Domain& d) {
		for(auto i : d.values) {
			out << i << " ";
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
	void setEmpty();
	void setValue(std::size_t value);
	std::size_t getValue() const;

	void reset();
	bool isComplete() const;

	friend std::ostream& operator << (std::ostream& out, const Cell& c);
private:
	Domain _domain;
	std::size_t _value;
	std::size_t _size;
};

#endif // CELL_H
