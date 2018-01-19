#ifndef RECORDER_H
#define RECORDER_H
#include "puzzle.h"

struct Subrecord {
	Position position;
	std::size_t elimination;

	Subrecord() {
		elimination = 0;
	}

	Subrecord(std::size_t x, std::size_t y, std::size_t e) {
		position = Position(x, y);
		elimination = e;
	}

	Subrecord(Position position, std::size_t e) {
		this->position = position;
		elimination = e;
	}

	friend std::ostream& operator <<(std::ostream& out, const Subrecord& s) {
		out << s.position << ": " << s.elimination;
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
	void addPropagation(Position position, std::size_t elimination);

	Record undo();

	friend std::ostream& operator <<(std::ostream& out, const Recorder& r);
private:
	std::vector<Record> _records;
};

#endif // RECORDER_H
