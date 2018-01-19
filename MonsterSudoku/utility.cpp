#include "utility.h"

namespace utils {
	std::vector<std::string> splitString(const std::string& s, char delimiter) {
		std::vector<std::string> tokens;
		std::stringstream tokenStream(s);
		std::string token;
		while(std::getline(tokenStream, token, delimiter)) {
			tokens.push_back(token);
		}
		return tokens;
	}

	int convertCharToIndex(char c) {
		/* Numbers 1 - 9 are 0 - 8 */
		if(c >= '1' && c <= '9') {
			return c - '1';	// '1' is 49
		}
		/* Numbers 10+ are denoted by letters, and are 9+ */
		else if(c >= 'A' && c <= 'Z') {
			return c - '8';	// '8' is 56
		}
		else {
			return -1;		// Bad char
		}
	}

	char convertIndexToChar(std::size_t i) {
		/* Numbers 0 to 8 */
		if(i < 9) {
			return i + '1';
		}
		else {
			return i + '8';
		}
	}

	std::vector<bool> getDefaultBools(int size, bool val) {
		std::vector<bool> vals;
		for(int i = 0; i < size; ++i) {
			vals.push_back(val);
		}
		return vals;
	}

}
