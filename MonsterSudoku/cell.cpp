#include "cell.h"

Cell::Cell() {
	_chosen = 0;
	_size = 0;
}

Cell::Cell(int size) {
	_domain = Domain(size);
	_chosen = size;
	_size = size;
}

Domain Cell::getDomain() {
	return _domain;
}

void Cell::setDomain(Domain domain) {
	_domain = domain;
}

void Cell::set(std::size_t value, bool val) {
	if(value >= _size) {
		return;
	}
	_domain.domain[value] = val;
}

void Cell::setChosen(std::size_t value) {
	if(value >= _size) {
		return;
	}
	_chosen = value;
	for(std::size_t i = 0; i < _size; ++i) {
		set(i, false);
	}
	set(value, true);
}

void Cell::reset() {
	_chosen = _size;
	for(utils::vb_it it = _domain.domain.begin(); it != _domain.domain.end(); ++it) {
		*it = true;
	}
}

bool Cell::isComplete() const {
	return _chosen < _size;
}

std::size_t Cell::getVal() const {
	return _chosen;
}

std::ostream& operator << (std::ostream& out, const Cell& c) {
	if(c._chosen == c._size) {
		out << "-";
	}
	else {
		if(c._chosen + 1 < 10) {
			out << c._chosen + 1;
		}
		else {
			out << utils::convertIndexToChar (c._chosen);
		}
	}
	/* Print the domain as well */
#if DEBUG
	out << ": { ";
	utils::vb_cit end = c._domain.domain.end();
	--end;
	for(utils::vb_cit it = c._domain.domain.begin(); it != end; ++it) {
		out << *it << ", ";
	}
	out << *end << " }";
#endif
	return out;
}
