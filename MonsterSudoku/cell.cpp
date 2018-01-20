#include "cell.h"

Cell::Cell() {
	_value = 0;
	_size = 0;
}

Cell::Cell(int size) {
	_domain = Domain(size);
	_value = size;
	_size = size;
}

Domain Cell::getDomain() {
	return _domain;
}

void Cell::setDomain(const Domain& domain) {
	_domain = domain;
}

void Cell::set(std::size_t value, bool val) {
	if(value >= _size) {
		return;
	}
	_domain.values[value] = val;
}

void Cell::setValue(std::size_t value) {
	if(value >= _size) {
		return;
	}
	_value = value;
	for(std::size_t i = 0; i < _size; ++i) {
		set(i, false);
	}
	set(value, true);
}

void Cell::setEmpty() {
	_value = _size;
}

void Cell::reset() {
	_value = _size;
	for(utils::vb_it it = _domain.values.begin(); it != _domain.values.end(); ++it) {
		*it = true;
	}
}

bool Cell::isComplete() const {
	return _value < _size;
}

std::size_t Cell::getValue() const {
	return _value;
}

std::ostream& operator << (std::ostream& out, const Cell& c) {
	if(c._value == c._size) {
		out << "-";
	}
	else {
		if(c._value + 1 < 10) {
			out << c._value + 1;
		}
		else {
			out << utils::convertIndexToChar (c._value);
		}
	}
	/* Print the domain as well */
#if VERBOSE
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
