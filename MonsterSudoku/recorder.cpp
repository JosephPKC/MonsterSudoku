#include "recorder.h"

Recorder::Recorder() {
	// Nothing
}

void Recorder::add(Position position, std::size_t value, Domain domain) {
	Record r(position, value, domain);
	_records.push_back(r);
}

void Recorder::addPropagation(Position position, std::vector<bool> eliminations) {
	Subrecord s(eliminations.size(), position);
	s.eliminations = eliminations;
	std::vector<Record>::iterator it = _records.end();
	--it;
	it->propagations.push_back(s);
}

Record Recorder::undo() {
	if(_records.empty()) {
		throw utils::Error::No_Records;
	}
	/* Pop the last record */
	Record r = *--_records.end();
	_records.pop_back();
	return r;
}

std::ostream& operator <<(std::ostream& out, const Recorder& r) {
	for(std::vector<Record>::const_iterator it = r._records.begin(); it != r._records.end(); ++it) {
		out << *it << " ";
	}
	return out;
}
