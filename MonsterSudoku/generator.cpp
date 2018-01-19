#include "generator.h"

Generator::Generator() {
	// Set seed
	std::srand(std::time(nullptr));
	_timeout = 300;
}

Generator::Generator(int timeout) {
	std::srand(std::time(nullptr));
	_timeout = std::max(timeout, 0);
}

bool Generator::checkFile(std::string path) {
	std::ifstream file(path);
	if(file.is_open()) {
		file.close();
		return true;
	}
	return false;
}

Puzzle Generator::generate(std::string path) {
	/* Generate puzzle from file */
	int m, n, p, q;
	std::ifstream in(path);
	if(in.is_open()) {
		/* File is found */
		try {
			/* First line are the four parameters */
			in >> m >> n >> p >> q;
			if(!_areParamsLegal(m, n, p, q)) {
				throw utils::Error::Bad_Parameters;
			}
			/* The next lines are the puzzle */
			Puzzle puzzle(m, n, p, q);
			for(std::size_t x = 0; x < puzzle.size(); ++x) {
				for(std::size_t y = 0; y < puzzle.size(); ++y) {
					char c;
					in >> c;
					if(c != '.') {
						puzzle.access(x, y).setChosen(utils::convertCharToIndex (c));
					}
				}
			}
			in.close();
			if(puzzle.isLegal()) {
				return puzzle;
			}
			else {
				/* Illegal Puzzle */
				throw utils::Error::Illegal_Puzzle;
			}
		}
		catch(const std::exception& e) {
			/* Something went wrong when reading in file. Bad File Format */
			throw utils::Error::Bad_File_Format;
		}
	}
	else {
		/* File was not found */
		throw utils::Error::File_Not_Found;
	}
}

Puzzle Generator::generate(int m, int n, int p, int q) {
	/* Generate puzzle randomly */
	if(!_areParamsLegal(m, n, p, q)) {
		throw utils::Error::Bad_Parameters;
	}
	/* Generate an empty puzzle to specifications */
	Puzzle puzzle(m, n, p, q);
	bool reset = true;
	auto start = std::chrono::system_clock::now();
	/* Occasionally, when m ~ n^2, random generation ends up at a dead end. This requires a full restart */
	while(reset) {
		/* Generate a list of empty cells */
		std::vector<Position> emptyCells;
		for(std::size_t x = 0; x < puzzle.size(); ++x) {
			for(std::size_t y = 0; y < puzzle.size(); ++y) {
				emptyCells.push_back(Position(x, y));
			}
		}
		reset = false;
		/* Generate m random numbers */
		for(int i = 0; i < m; ++i) {
			/* Check for timeout */
			auto current = std::chrono::system_clock::now();
			std::chrono::duration<double> duration = current - start;
			if(duration.count() >= _timeout) {
				throw utils::Error::Timeout;
			}
			/* If no more empty cells, we just reset the entire thing */
			if(emptyCells.empty()) {
				puzzle.reset();
				reset = true;
				break;
			}
			/* Choose a random empty cell */
			std::size_t index = std::rand() % emptyCells.size();
			std::size_t x = emptyCells[index].x;
			std::size_t y = emptyCells[index].y;
			/* Remove chosen cell from empty cells */
			emptyCells.erase(emptyCells.begin() + index);
			/* Create a list of possible values for the cell */
			std::vector<bool> values = puzzle.neighborValues(x, y);
			/* Invert the values list to get legal values */
			std::vector<std::size_t> legal_values;
			for(std::size_t i = 0; i < values.size(); ++i) {
				if(!values[i]) {
					legal_values.push_back(i);
				}
			}
			/* Check if any legal values exist */
			if(legal_values.empty()) {
				/* No legal values */
				puzzle.reset();
				reset = true;
				break;
			}
			/* Choose a random value */
			index = std::rand() % legal_values.size();
			std::size_t v = legal_values[index];
			/* Assign */
			puzzle.set(x, y, v);
		}
	}
	/* Just to time the generation algorithm */
#if TIME
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << duration.count() << " seconds." << std::endl;
#endif
	return puzzle;
}

void Generator::saveToFile(const Puzzle& puzzle, std::string path) {
	/* Save puzzle to file */
	std::ofstream out(path);
	if(out.is_open()) {
		/* File found */
		/* Write parameters */
		out << puzzle.parameters() << std::endl;
		/* Write puzzle */
		for(std::size_t x = 0; x < puzzle.size(); ++x) {
			for(std::size_t y = 0; y < puzzle.size(); ++y) {
				if(puzzle.access(x, y).getVal () < puzzle.size()) {
					out << utils::convertIndexToChar (puzzle.access(x, y).getVal());
				}
				else {
					out << ".";
				}
			}
			out << std::endl;
		}
		out.close();
	}
	else {
		/* File not found */
		throw utils::Error::File_Not_Found;
	}
}

bool Generator::_areParamsLegal(int m, int n, int p, int q) const {
	/* All must be positive integers, with n being at least 2, and n being no more than n^2 - 1 (if it was n^2, then it would be solved already), and pq = n */
	return m > 0 && n > 1 && p > 0 && q > 0 && m < n * n && p * q == n;
}
