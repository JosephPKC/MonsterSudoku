#include "logger.h"

Logger::Logger() {
	// Nothing
}

Log& Logger::log() {
	return _log;
}

Log Logger::log() const {
	return _log;
}

std::ostream& operator <<(std::ostream& out, const Logger& l) {
	out << l._log;
	return out;
}
