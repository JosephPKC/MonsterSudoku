#include "generator.h"

Generator::Generator() {
	std::srand(std::time(nullptr));
	_timeout = 300;
	_gen = 3;
}

Generator::Generator(int timeout, int gen) {
	std::srand(std::time(nullptr));
	_timeout = std::max(timeout, 0);
	_gen = gen;
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
			for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
				for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
					char c;
					in >> c;
					if(c != '.') {
						puzzle.access(x, y).setValue(utils::convertCharToIndex (c));
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
		for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
			for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
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
			std::vector<bool> values = puzzle.getNeighborValues(x, y);
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

Puzzle Generator::generateSolvable(int m, int n, int p, int q) {
	/* A Generator that guarantees solvable puzzles:
	 * Fill in the puzzle completely so that it is legal.
	 *		Generate the first row by going in order.
	 *		Then go to the next row and offset it by q.
	 *		If the offset addition forces it to go over n, modulate it and then add 1.
	 *		Continue until puzzle is completely filled.
	 * First, randomly swap any two block rows (entire blocks in a row).
	 * Then, randomly swap any two block columns
	 * Then, within randomly selected block rows, randomly swap any two rows within that block row.
	 * Then, within randomly selected block columns, randomly swap any two columns within that block column.
	 * Then randomly choose and empty out cells up to n^2 - m.
	 */
	if(!_areParamsLegal(m, n, p, q)) {
		throw utils::Error::Bad_Parameters;
	}
	/* Create a puzzle & fill */
	Puzzle puzzle(m, n, p, q);
	int offset = 0;
	std::size_t val = 0;
	for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
		for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
			puzzle.set(x, y, val);
			++val;
			if(val >= puzzle.getSize()) {
				val = 0;
			}
		}
		offset += q;
		if(offset >= n) {
			offset -= n;
			++offset;
		}
		val = offset;
	}
	/* Swap blocks */
	for(int i = 0; i < _gen * 2; ++i) {
		/* Swap block rows */
		int first = (std::rand() % n);
		int second = (std::rand() % n);
//		std::cout << "Generated: " << first << " & " << second << std::endl;
		int upper = std::min(first, second);
		int lower = std::max(first, second);
		Block upperBlock = puzzle.getBlock(upper, 0);
		Block lowerBlock = puzzle.getBlock(lower, 0);
//		int first = 0;
//		int second = q;
		if(upperBlock.t != lowerBlock.t) {
			int dist = lowerBlock.t - upperBlock.t;
//			std::cout << "Distance from " << upper << ": " << dist << std::endl;
			for(std::size_t x = upperBlock.t; x <= upperBlock.b; ++x) {
				for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
					std::size_t temp = puzzle.access(x, y).getValue();
					puzzle.set(x, y, puzzle.access(x + dist, y).getValue());
					puzzle.set(x + dist, y, temp);
				}
			}
		}
		/* Swap block columns */
		first = (std::rand() % n);
		second = (std::rand() % n);
//		std::cout << "Generated: " << first << " & " << second << std::endl;
		int left = std::min(first, second);
		int right = std::max(first, second);
		Block leftBlock = puzzle.getBlock(0, left);
		Block rightBlock = puzzle.getBlock(0, right);
		if(leftBlock.l != rightBlock.l) {
			int dist = rightBlock.l - leftBlock.l;
//			std::cout << "Distance from " << left << ": " << dist << std::endl;
			for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
				for(std::size_t y = leftBlock.l; y <= leftBlock.r; ++y) {
//					std::cout << "Swapping " << x << ", " << y << " & " << x << ", " << y + dist << std::endl;
					std::size_t temp = puzzle.access(x, y).getValue();
					puzzle.set(x, y, puzzle.access(x, y + dist).getValue());
					puzzle.set(x, y + dist, temp);
				}
			}
		}
	}
	for(int i = 0; i < _gen * 50; ++i) {
		/* Swap rows in blocks */
		int row = (std::rand() % n);
		Block block = puzzle.getBlock(row, 0);
		int first = std::rand() % p;
		int second = std::rand() % p;
		if(first != second) {
			first += block.t;
			second += block.t;
			for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
				std::size_t temp = puzzle.access(first, y).getValue();
				puzzle.set(first, y, puzzle.access(second, y).getValue());
				puzzle.set(second, y, temp);
			}
		}
		/* Swap columns in blocks */
		int col = (std::rand() % n);
		block = puzzle.getBlock(0, col);
		first = std::rand() % q;
		second = std::rand() % q;
		if(first != second) {
			first += block.l;
			second += block.l;
			for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
				std::size_t temp = puzzle.access(x, first).getValue();
				puzzle.set(x, first, puzzle.access(x, second).getValue());
				puzzle.set(x, second, temp);
			}
		}
	}

	/* Randomly remove filled cells up to n^2 - m */
	std::vector<Position> cells;
	for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
		for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
			cells.push_back(Position(x, y));
		}
	}
	for(int i = 0; i < n*n - m; ++i) {
		std::size_t r = std::rand() % cells.size();
		puzzle.reset(cells[r]);
		cells.erase(cells.begin() + r);
	}

//	for(std::size_t i = 0; i < puzzle.getSize(); ++i) {
//		std::cout << "r/c: " << i << std::endl;
//		for(int j = 0; j < _gen * 10; ++j) {
//			/* Swap rows in blocks */
//			Block block = puzzle.getBlock(i, 0);
//			std::size_t other = std::rand() % p;
//			std::cout << "random row: " << other << std::endl;
//			if(i != other) {
//				other += block.t;
//				for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
//					std::size_t temp = puzzle.access(i, y).getValue();
//					puzzle.set(i, y, puzzle.access(other, y).getValue());
//					puzzle.set(other, y, temp);
//				}
//			}
//			std::cout << puzzle << std::endl;
//			/* Swap columns in blocks */
//			block = puzzle.getBlock(0, i);
//			other = std::rand() % q;
//			std::cout << "random col: " << other << std::endl;
//			if(i != other) {
//				other += block.l;
//				for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
//					std::size_t temp = puzzle.access(x, i).getValue();
//					puzzle.set(x, i, puzzle.access(x, other).getValue());
//					puzzle.set(x, other, temp);
//				}
//			}
//			std::cout << puzzle << std::endl;
//			char c;
////			std::cin >> c;
//		}

	return puzzle;
}

void Generator::saveToFile(const Puzzle& puzzle, std::string path) {
	/* Save puzzle to file */
	std::ofstream out(path);
	if(out.is_open()) {
		/* File found */
		/* Write parameters */
		out << puzzle.getParameters() << std::endl;
		/* Write puzzle */
		for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
			for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
				if(puzzle.access(x, y).getValue () < puzzle.getSize()) {
					out << utils::convertIndexToChar (puzzle.access(x, y).getValue());
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
