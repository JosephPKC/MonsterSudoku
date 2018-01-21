#include "recorder.h"

Recorder::Recorder() {
	// Nothing
}

void Recorder::add(const Position& position, std::size_t value, int degrees, const Domain& domain) {
	Record r(position, value, domain, degrees);
	_records.push_back(r);
}

void Recorder::addPropagation(const Position& position, std::size_t elimination) {
	Subrecord s(position, elimination);
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
