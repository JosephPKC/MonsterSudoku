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

Heuristics& Solver::heuristics() {
	return _heuristics;
}

Log Solver::log() {
	return _logger.log();
}

Puzzle Solver::solve(Puzzle puzzle, double timeout) {
	_start = std::chrono::system_clock::now();
	preSearch(puzzle);
	auto preSearchEnd = std::chrono::system_clock::now();
	auto res = search(puzzle, timeout);
	if(res != utils::Error::Success) {
		if(res == utils::Error::Timeout) {
			_logger.log().timeout = true;
		}
		else if(res == utils::Error::No_More_Values) {
			_logger.log().solved = true;
		}

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> duration = end - _start;
		std::chrono::duration<double> preDuration = preSearchEnd - _start;

		_logger.log().totalTime = duration.count();
		_logger.log().preTime = preDuration.count();

		throw res;
	}
	_logger.log().solvable = true;
	_logger.log().solved = true;

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> duration = end - _start;
	std::chrono::duration<double> preDuration = preSearchEnd - _start;

	_logger.log().totalTime = duration.count();
	_logger.log().preTime = preDuration.count();

#if TIME
	std::cout << "Total Solve Time: " << duration.count() << "s." << std::endl;

	std::cout << "Pre Search Time: " << preDuration.count() << "s." << std::endl;
#endif
	return puzzle;
}

/* Recursive backtracking search algorithm */
utils::Error Solver::search(Puzzle& puzzle, double timeout) {
	char c;
//	std::cin >> c;
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
//	std::cout << "Chosen: " << chosen << std::endl;
	/* Get domain order for the cell */
	std::vector<std::size_t> values = orderValues(puzzle, chosen);
	/* Check if there are values */
	if(values.empty()) {
		return utils::Error::No_More_Values;
	}

	/* While there are still values, try to assign that value to the cell */
	while(!values.empty()) {
//		std::cout << "Value Chosen: " << *values.begin() << std::endl;
		/* Do pre assignment processing */
		if(preAssign(puzzle, chosen, *values.begin())) {
			_logger.log().nodes += 1;
			/* Assign the value to the cell */
			assignValue(puzzle, chosen, *values.begin());
//			std::cout << puzzle << std::endl;
			/* Do post assignment processing */
			postAssign(puzzle, chosen);
			/* Recur, or try a new value now */
			auto res = search(puzzle, timeout);
			if(res == utils::Error::Success) {
				return res;
			}
			else if(res != utils::Error::Timeout) {
//				std::cout << "Backtracking: " << chosen << " & " << *values.begin() << std::endl;
				_logger.log().deadEnds += 1;
				backtrack(puzzle);
//				std::cout << "Backtracked puzzle:\n" << puzzle << std::endl;
//				puzzle.access(chosen.x, chosen.y).set(*values.begin(), false);
				values.erase(values.begin());

			}
			else {
//				std::cout << "Timeout" << std::endl;
				return res;
			}
		}
		else {
			/* Remove value from domain */
//			std::cout << "Value: " << *values.begin() << " not legal" << std::endl;
//			puzzle.access(chosen.x, chosen.y).set(*values.begin(), false);
			values.erase(values.begin());
		}
	}
	/* If we run out of values, and no success, return fail */
//	std::cout << "No more values" << std::endl;
	return utils::Error::No_More_Values;
}

Position Solver::selectNextCell(Puzzle puzzle) {
	for(std::size_t x = 0; x < puzzle.size(); ++x) {
		for(std::size_t y = 0; y < puzzle.size(); ++y) {
			if(puzzle.isEmpty(x, y)) {
				return Position(x, y);
			}
		}
	}
	throw utils::Error::No_Empty_Cells;
}

std::vector<std::size_t> Solver::orderValues(Puzzle puzzle, Position cell) {
	std::vector<std::size_t> values;
	Domain domain = puzzle.access(cell.x, cell.y).getDomain();
	for(std::size_t i = 0; i < puzzle.size(); ++i) {
		if(domain.domain[i]) {
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

	// Constraint Propagation Precursor
	// Perform constraint propagation on the initial set of solved cells.
	if(_heuristics.hasCPP) {
		auto start = std::chrono::system_clock::now();

		for(std::size_t x = 0; x < puzzle.size(); ++x) {
			for(std::size_t y = 0; y < puzzle.size(); ++y) {
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
}

bool Solver::isLegal(Puzzle puzzle, Position cell, std::size_t value) {
	if(value == puzzle.size()) {
		return true;
	}
	std::size_t x = cell.x, y = cell.y;
	/* Check if there is a cell with the same value in the row & column of the cell */
	for(std::size_t i = 0; i < puzzle.size(); ++i) {
		if(i != y && !puzzle.isEmpty(x, i) && puzzle.access(x, i).getVal() == value) {
			return false;
		}
		if(i != x && !puzzle.isEmpty(i, y) && puzzle.access(i, y).getVal() == value) {
			return false;
		}
	}

	Perimeter p = puzzle.perimeter(cell.x, cell.y);
	for(std::size_t i = p.t; i <= p.b; ++i) {
		for(std::size_t j = p.l; j <= p.r; ++j) {
			if(i != x && j != y && !puzzle.isEmpty(i, j) && puzzle.access(i, j).getVal() == value) {
				return false;
			}
		}
	}
	return true;
}

void Solver::propagateConstraints(Puzzle& puzzle, Position cell, bool record) {
	std::size_t value = puzzle.access(cell.x, cell.y).getVal();
	/* Go through each row & column, and remove the value from their domains */
	for(std::size_t i = 0; i < puzzle.size(); ++i) {
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
	Perimeter p = puzzle.perimeter(cell.x, cell.y);
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
