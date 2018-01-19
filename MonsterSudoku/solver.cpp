#include "solver.h"

Solver::Solver() {
	// Nothing
}

Solver::Solver(bool mrv, bool dh, bool lcv, bool acp, bool mac, bool fc) {
	_heuristics.hasMRV = mrv;
	_heuristics.hasDH = dh;
	_heuristics.hasLCV = lcv;
	_heuristics.hasACP = acp;
	_heuristics.hasMAC = mac;
	_heuristics.hasFC = fc;
}

Heuristics& Solver::heuristics() {
	return _heuristics;
}

Puzzle Solver::solve(Puzzle puzzle, double timeout) {
	_start = std::chrono::system_clock::now();
	preSearch(puzzle);
	auto res = search(puzzle, timeout);
	if(res != utils::Error::Success) {
		throw res;
	}
#if DEBUG
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> duration = end - start;
	std::cout << duration.count() << "s finished." << std::endl;
#endif
	return puzzle;
}

/* Recursive backtracking search algorithm */
utils::Error Solver::search(Puzzle& puzzle, double timeout) {
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
			/* Assign the value to the cell */
			assignValue(puzzle, chosen, *values.begin());
			std::cout << puzzle << std::endl;
			/* Do post assignment processing */
			postAssign(puzzle, chosen);
			/* Recur, or try a new value now */
			auto res = search(puzzle, timeout);
			if(res == utils::Error::Success) {
				return res;
			}
			else if(res != utils::Error::Timeout) {
//				std::cout << "Backtracking: " << chosen << " & " << *values.begin() << std::endl;
				backtrack(puzzle);
				values.erase(values.begin());
			}
			else {
				return res;
			}
		}
		else {
			/* Remove value from domain */
			values.erase(values.begin());
		}
	}
	/* If we run out of values, and no success, return fail */
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
	Record r = _recorder.undo();
	/* Unassign the cell that was assigned, and restore its domain */
	puzzle.restore(r.position.x, r.position.y, r.previousDomain);
	/* Restore the domains for each propagation */
	for(std::vector<Subrecord>::iterator it = r.propagations.begin(); it != r.propagations.end(); ++it) {
		Position p = it->position;
		for(std::size_t i = 0; i < puzzle.size(); ++i) {
			if(it->eliminations[i]) {
				puzzle.access(p.x, p.y).set(i, true);
			}
		}
	}
}

void Solver::preSearch(Puzzle& puzzle) {
	// Nothing for now.
	// Will have arc consistency precursor check
}

bool Solver::preAssign(Puzzle& puzzle, Position toAssignCell, std::size_t val) {
	// Nothing for now.
	// Will contain pre-assignment constraint satisfaction checks
	// i.e. Forward Checking. If a to be assigned value violates a constraint, don't bother assigning it.
	// Check if move is legal
	return isLegal(puzzle, toAssignCell, val);
}

void Solver::postAssign(Puzzle& puzzle, Position assignedCell) {
	// Nothing for now.
	// Will contain arc consistency propagation.
	// i.e. From the chosen cell, maintain arc consistency with neighbors, and their neighbors, etc...
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
			if(i != y && j != x && !puzzle.isEmpty(i, j) && puzzle.access(i, j).getVal() == value) {
				return false;
			}
		}
	}
	return true;
}
