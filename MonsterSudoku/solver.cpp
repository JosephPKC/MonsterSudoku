#include "solver.h"

Solver::Solver() {
	// Nothing
}

Solver::Solver(Heuristics heuristics) {
	_heuristics = heuristics;
}

Solver::Solver(bool mrv, bool dh, bool lcv, bool acp, bool mac, bool fc, bool cpp, bool cp) {
	_heuristics.hasMRV = mrv;
	_heuristics.hasMD = dh;
	_heuristics.hasLCV = lcv;
	_heuristics.hasACP = acp;
	_heuristics.hasMAC = mac;
	_heuristics.hasFC = fc;
	_heuristics.hasCPP = cpp;
	_heuristics.hasCP = cp;
}

Heuristics& Solver::getHeuristics() {
	return _heuristics;
}

Log Solver::getLog() const {
	return _logger.log();
}

Puzzle Solver::solve(Puzzle puzzle, double timeout) {
	/* Initialize */
	_logger = Logger();
	_start = std::chrono::system_clock::now();

	/* Pre-process */
	preSearch(puzzle);
	_logger.log().preTime = getDuration(_start);

	/* Search */
	utils::Error res = search(puzzle, timeout);

	/* Search Results */
	if(res != utils::Error::Success) {
		/* Different possible outcomes */
		if(res == utils::Error::Timeout) {
			/* Ran out of time */
			_logger.log().timeout = true;
		}
		else if(res == utils::Error::No_More_Values) {
			/* Unsolvable but Solved */
			_logger.log().solved = true;
		}
		_logger.log().totalTime = getDuration(_start);

		throw res;
	}
	/* Success: Solvable and Solved */
	_logger.log().solvable = true;
	_logger.log().solved = true;
	_logger.log().totalTime = getDuration(_start);

	/* Clean up  */
	_recorder = Recorder();
//	utils::deleteArray<int>(_degrees, puzzle.getSize());
	return puzzle;
}

utils::Error Solver::search(Puzzle& puzzle, double timeout) {
#if DEBUG && PAUSE
	char c;
	std::cin >> c;
#endif
	/* Check if done (base case) */
	if(puzzle.isSolved()) {
		return utils::Error::Success;
	}
	/* Check for timeout (fail case) */
	auto current = std::chrono::system_clock::now();
	std::chrono::duration<double> duration = current - _start;
	if(duration.count() >= timeout) {
		return utils::Error::Timeout;
	}
	/* Select a cell */
	Position chosen;
	try {
		chosen = selectNextCell(puzzle);
	}
	catch(utils::Error e) {
		return e;
	}
#if DEBUG
	std::cout << "Chosen: " << chosen << " -Domain: " << puzzle.getDomainSize(chosen.x, chosen.y) << std::endl;
#endif
	/* Get domain order for the cell */
	std::vector<std::size_t> values = orderValues(puzzle, chosen);
	/* Check if there are values */
	if(values.empty()) {
		return utils::Error::No_More_Values;
	}

	/* While there are still values, try to assign that value to the cell */
	while(!values.empty()) {
#if DEBUG
		std::cout << "Value Chosen: " << *values.begin() << std::endl;
#endif
		/* Do pre assignment processing */
		if(preAssign(puzzle, chosen, *values.begin())) {
			_logger.log().nodes += 1;
			/* Assign the value to the cell */
			assignValue(puzzle, chosen, *values.begin());
#if DEBUG
			std::cout << puzzle << std::endl;
#endif
			/* Do post assignment processing */
			postAssign(puzzle, chosen);
			/* Recur, or try a new value now */
			auto res = search(puzzle, timeout);
			if(res == utils::Error::Success) {
				return res;
			}
			else if(res != utils::Error::Timeout) {
#if DEBUG
				std::cout << "Backtracking: " << chosen << " & " << *values.begin() << std::endl;
#endif
				_logger.log().deadEnds += 1;
				backtrack(puzzle);
#if DEBUG
				std::cout << "Backtracked puzzle:\n" << puzzle << std::endl;
#endif
//				puzzle.access(chosen.x, chosen.y).set(*values.begin(), false);
				values.erase(values.begin());
			}
			else {
#if DEBUG
				std::cout << "Timeout" << std::endl;
#endif
				return res;
			}
		}
		else {
			/* Remove value from domain */
#if DEBUG
			std::cout << "Value: " << *values.begin() << " not legal" << std::endl;
#endif
//			puzzle.access(chosen.x, chosen.y).set(*values.begin(), false);
			values.erase(values.begin());
		}
	}
	/* If we run out of values, and no success, return fail */
#if DEBUG
	std::cout << "No more values" << std::endl;
#endif
	return utils::Error::No_More_Values;
}

Position Solver::selectNextCell(Puzzle puzzle) {
	/* MRV */
	if(_heuristics.hasMRV) {
		if(_heuristics.hasMD) {

		}
		else {
			try {
				return selectByMRV(puzzle);
			}
			catch(utils::Error e) {
				throw e;
			}
		}
	}
	else if(_heuristics.hasMD) {
		int max = -1;
		Position largest = Position(puzzle.getSize(), puzzle.getSize());
		for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
			for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
				if(puzzle.isEmpty(x, y) && _degrees[x][y] > max) {
					max = _degrees[x][y];
					largest = Position(x, y);
				}
			}
		}
		if(max == -1) {
			throw utils::Error::No_Empty_Cells;
		}
		return largest;
	}
	else {
		/* Get the next empty cell */
		for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
			for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
				if(puzzle.isEmpty(x, y)) {
					return Position(x, y);
				}
			}
		}
		throw utils::Error::No_Empty_Cells;
	}
}

std::vector<std::size_t> Solver::orderValues(Puzzle puzzle, Position cell) {
	std::vector<std::size_t> values;
	Domain domain = puzzle.getDomain(cell.x, cell.y);
	for(std::size_t i = 0; i < puzzle.getSize(); ++i) {
		if(domain.values[i]) {
			values.push_back(i);
		}
	}
	return values;
}

void Solver::backtrack(Puzzle& puzzle) {
	auto start = std::chrono::system_clock::now();

	Record r = _recorder.undo();
//	std::cout << "Undo this record: " << r << std::endl;
	/* Unassign the cell that was assigned, and restore its domain */
	puzzle.restore(r.position.x, r.position.y, r.previousDomain);
	/* Restore the domains for each propagation */
	for(std::vector<Subrecord>::iterator it = r.propagations.begin(); it != r.propagations.end(); ++it) {
		Position p = it->position;
		puzzle.access(p.x, p.y).set(it->elimination, true);
	}

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> duration = end - start;
	_logger.log().btTime += duration.count();
}

void Solver::preSearch(Puzzle& puzzle) {
	// Create a network of Degrees
//	_degrees = new int*[puzzle.size()];
//	for(std::size_t x = 0; x < puzzle.size(); ++x) {
//		_degrees[x] = new int[puzzle.size()];
//		for(std::size_t y = 0; y < puzzle.size(); ++y) {
//			if(puzzle.isEmpty(x, y)) {
//				/* Get the number of empty neighbors */
//				int degrees = 0;
//				for(std::size_t i = 0; i < puzzle.size(); ++i) {
//					if(i != y && puzzle.isEmpty(x, i)) {
//						++degrees;
//					}
//					if(i != x && puzzle.isEmpty(i, y)) {
//						++degrees;
//					}
//				}
//				Perimeter p = puzzle.perimeter(x, y);
//				for(std::size_t i = p.t; i <= p.b; ++i) {
//					for(std::size_t j = p.l; j <= p.r; ++j) {
//						if(i != x && j != y && puzzle.isEmpty(i, j)) {
//							++degrees;
//						}
//					}
//				}
//				_degrees[x][y] = degrees;
////				std::cout << "Degrees for " << x << ", " << y << ": " << degrees << std::endl;
//			}
//			else {
//				_degrees[x][y] = 0;
//			}
//		}
//	}

	// Constraint Propagation Precursor
	// Perform constraint propagation on the initial set of solved cells.
	if(_heuristics.hasCPP) {
		auto start = std::chrono::system_clock::now();

		for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
			for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
				if(!puzzle.isEmpty(x, y)) {
					propagateConstraints(puzzle, Position(x, y));
				}
			}
		}

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> duration = end - start;
		_logger.log().cppTime += duration.count();
	}
}

bool Solver::preAssign(Puzzle& puzzle, Position toAssignCell, std::size_t val) {
	// Check if move is legal
	return isLegal(puzzle, toAssignCell, val);
}

void Solver::postAssign(Puzzle& puzzle, Position assignedCell) {
	// Nothing for now.
	// Will contain arc consistency propagation.
	// i.e. From the chosen cell, maintain arc consistency with neighbors, and their neighbors, etc...

	// Constraint Propagation
	// Perform constraint propagation on the assigned cell.
	if(_heuristics.hasCP) {
		auto start = std::chrono::system_clock::now();
		propagateConstraints(puzzle, assignedCell, true);
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> duration = end - start;
		_logger.log().cpTime += duration.count();
	}
	// Also keep a record of the eliminated values.
	// Forward Check
}

void Solver::assignValue(Puzzle& puzzle, Position cell, std::size_t value) {
	Domain previous = puzzle.access(cell.x, cell.y).getDomain();
	puzzle.set(cell.x, cell.y, value);
	_recorder.add(cell, value, previous);

	/* Manage the neighbor's degrees */
//	std::vector<Position> neighbors = puzzle.neighbors(cell.x, cell.y, true);
//	for(std::vector<Position>::iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
//		--_degrees[it->x][it->y];
//		if(_degrees[it->x][it->y] < 0) {
////			std::cerr << "Degrees is less than 0. Should not happen. @ " << it->x << ", " << it->y << std::endl;
//		}
//	}
}

bool Solver::isLegal(Puzzle puzzle, Position cell, std::size_t value) {
	if(value == puzzle.getSize()) {
		return true;
	}
	std::size_t x = cell.x, y = cell.y;
	/* Check if there is a cell with the same value in the row & column of the cell */
	for(std::size_t i = 0; i < puzzle.getSize(); ++i) {
		if(i != y && !puzzle.isEmpty(x, i) && puzzle.access(x, i).getValue() == value) {
			return false;
		}
		if(i != x && !puzzle.isEmpty(i, y) && puzzle.access(i, y).getValue() == value) {
			return false;
		}
	}

	Box p = puzzle.getBox(cell.x, cell.y);
	for(std::size_t i = p.t; i <= p.b; ++i) {
		for(std::size_t j = p.l; j <= p.r; ++j) {
			if(i != x && j != y && !puzzle.isEmpty(i, j) && puzzle.access(i, j).getValue() == value) {
				return false;
			}
		}
	}
	return true;
}

void Solver::propagateConstraints(Puzzle& puzzle, Position cell, bool record) {
	std::size_t value = puzzle.access(cell.x, cell.y).getValue();
	/* Go through each row & column, and remove the value from their domains */
	for(std::size_t i = 0; i < puzzle.getSize(); ++i) {
		if(puzzle.isEmpty(cell.x, i)) {
			/* Cell that is empty */
			puzzle.access(cell.x, i).set(value, false);
			if(record) {
				_recorder.addPropagation(Position(cell.x, i), value);
			}
		}
		if(puzzle.isEmpty(i, cell.y)) {
			puzzle.access(i, cell.y).set(value, false);
			if(record) {
				_recorder.addPropagation(Position(i, cell.y), value);
			}
		}
	}
	Box p = puzzle.getBox(cell.x, cell.y);
	for(std::size_t i = p.t; i <= p.b; ++i) {
		for(std::size_t j = p.l; j <= p.r; ++j) {
			if(puzzle.isEmpty(i, j)) {
				puzzle.access(i, j).set(value, false);
				if(record) {
					_recorder.addPropagation(Position(i, j), value);
				}
			}
		}
	}
}

Position Solver::selectByMRV(Puzzle puzzle) {
	auto start = std::chrono::system_clock::now();

	/* Find the smallest domain size */
	std::size_t min = puzzle.getSize() + 1;
	Position smallest;
	for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
		for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
			if(puzzle.isEmpty(x, y) && puzzle.getDomainSize(x, y) < min) {
				min = puzzle.getDomainSize(x, y);
				smallest = Position(x, y);
			}
		}
	}
	_logger.log().mrvTime = getDuration(start);
	if(min == puzzle.getSize() + 1) {
		/* There were no empty cells */
		throw utils::Error::No_Empty_Cells;
	}
	return smallest;
}

std::vector<Position> Solver::orderByMRV(Puzzle puzzle) {
//	auto start = std::chrono::system_clock::now();

//	/* Find the smallest domain size */
//	std::size_t min = puzzle.size() + 1;
//	Position smallest = Position(puzzle.size(), puzzle.size());
//	for(std::size_t x = 0; x < puzzle.size(); ++x) {
//		for(std::size_t y = 0; y < puzzle.size(); ++y) {
//			if(puzzle.isEmpty(x, y) && puzzle.domainSize(x, y) < min) {
//				min = puzzle.domainSize(x, y);
//				smallest = Position(x, y);
//			}
//		}
//	}

//	if(min == puzzle.size() + 1) {
//		/* There were no empty cells */
//		auto end = std::chrono::system_clock::now();
//		std::chrono::duration<double> duration = end - start;
//		_logger.log().mrvTime += duration.count();
//		throw utils::Error::No_Empty_Cells;
//	}
//	std::vector<Position> smallests;

//	/* If MD heuristic is enabled, we should find all of the cells with the same smallest domain */
//	if(_heuristics.hasMD) {

//	}
//	/* Else, just find the first smallest domain */
//	else {
//		auto end = std::chrono::system_clock::now();
//		std::chrono::duration<double> duration = end - start;
//		_logger.log().mrvTime += duration.count();
//		smallests.push_back(smallest);
//		return smallests;
//	}
//	/* Get an empty cell with the smallest domain */


}

double Solver::getDuration(std::chrono::time_point<std::chrono::system_clock> start) const {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> duration = end - start;
	return duration.count();
}
