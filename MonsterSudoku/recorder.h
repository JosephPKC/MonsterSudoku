#ifndef RECORDER_H
#define RECORDER_H
#include "puzzle.h"

struct Subrecord {
	Position position;
	std::vector<bool> eliminations;

	Subrecord(int size = 0) {
		for(int i = 0; i < size; ++i) {
			eliminations.push_back(false);
		}
	}

	Subrecord(int size, std::size_t x, std::size_t y) {
		position = Position(x, y);
		for(int i = 0; i < size; ++i) {
			eliminations.push_back(false);
		}
	}

	Subrecord(int size, Position position) {
		this->position = position;
		for(int i = 0; i < size; ++i) {
			eliminations.push_back(false);
		}
	}

	friend std::ostream& operator <<(std::ostream& out, const Subrecord& s) {
		out << s.position << ": ";
		for(utils::vb_cit it = s.eliminations.begin(); it != s.eliminations.end(); ++it) {
			out << *it << " ";
		}
		return out;
	}
};

struct Record {
	Position position;
	std::size_t value;
	Domain previousDomain;
	std::vector<Subrecord> propagations;

	Record() {
		value = 0;
	}

	Record(Position position, std::size_t value, Domain domain) {
		this->position = position;
		this->value = value;
		this->previousDomain = domain;
	}

	friend std::ostream& operator <<(std::ostream& out, const Record& r) {
		out << r.position << ": " << r.value << ": " << r.previousDomain << ":: ";
		for(std::vector<Subrecord>::const_iterator it = r.propagations.begin(); it != r.propagations.end(); ++it) {
			out << *it << " ";
		}
		return out;
	}
};

class Recorder {
public:
	Recorder();

	void add(Position position, std::size_t value, Domain domain);
	void addPropagation(Position position, std::vector<bool> eliminations);

	Record undo();

	friend std::ostream& operator <<(std::ostream& out, const Recorder& r);
private:
	std::vector<Record> _records;
};

#endif // RECORDER_H
