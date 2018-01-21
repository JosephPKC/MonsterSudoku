#include "solver.h"

Solver::Solver() {
	// Nothing
}

Solver::Solver(const Heuristics& heuristics) {
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

Puzzle Solver::solve(const Puzzle& puzzle, double timeout) {
	/* Initialize */
	_logger = Logger();
	_start = std::chrono::system_clock::now();

	Puzzle puzzleC = puzzle;
	/* Pre-process */
	preSearch(puzzleC);
	_logger.log().preTime = getDuration(_start);

	/* Search */
	utils::Error res = search(puzzleC, timeout);

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
	return puzzleC;
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
	if(getDuration(_start) >= timeout) {
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
	std::cout << "Chosen: " << chosen << " -Domain: " << puzzle.getDomainSize(chosen.x, chosen.y);
	if(_heuristics.hasMD) {
		std::cout << " -Degrees: " << _degrees[chosen.x][chosen.y];
	}
	std::cout << std::endl;
#endif

	/* Get domain order for the cell */
	std::vector<std::size_t> values = orderValues(puzzle, chosen);
	if(values.empty()) {
		return utils::Error::No_More_Values;
	}

	/* While there are still values, try to assign that value to the cell */
	while(!values.empty()) {

#if DEBUG
		std::cout << "Value Chosen: " << *values.begin() << std::endl;
#endif

		_logger.log().allNodes += 1;

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

			values.erase(values.begin());
		}
	}
	/* If we run out of values, and no success, return fail */
#if DEBUG
	std::cout << "No more values" << std::endl;
#endif

	return utils::Error::No_More_Values;
}

Position Solver::selectNextCell(const Puzzle& puzzle) {
	/* MRV */
	if(_heuristics.hasMRV) {
		if(_heuristics.hasMD) {
			return selectByMD(orderByMRV(puzzle));
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
		try {
			return selectByMD(puzzle);
		}
		catch(utils::Error e) {
			throw e;
		}
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

std::vector<std::size_t> Solver::orderValues(const Puzzle& puzzle, const Position& cell) {
	if(_heuristics.hasLCV) {
		return orderByLCV(puzzle, cell);
	}
	else {
		std::vector<std::size_t> values;
		Domain domain = puzzle.getDomain(cell.x, cell.y);
		for(std::size_t i = 0; i < puzzle.getSize(); ++i) {
			if(domain.values[i]) {
				values.push_back(i);
			}
		}
		return values;
	}
}

void Solver::backtrack(Puzzle& puzzle) {
	auto start = std::chrono::system_clock::now();

	Record r = _recorder.undo();

#if DEBUG
	std::cout << "Undo this record: " << r << std::endl;
#endif

	/* Unassign the cell that was assigned, and restore its domain */
	puzzle.restore(r.position, r.previousDomain);
	if(_heuristics.hasMD) {
		_degrees[r.position.x][r.position.y] = r.previousDegrees;
	}

	/* Restore the domains for each propagation */
	for(std::vector<Subrecord>::iterator it = r.propagations.begin(); it != r.propagations.end(); ++it) {
		Position p = it->position;
		puzzle.access(p.x, p.y).set(it->elimination, true);
	}
	_logger.log().btTime += getDuration(start);

	/* Restore neighbor degrees */
	updateDegrees(puzzle, r.position, 1);
}

void Solver::preSearch(Puzzle& puzzle) {
	// Create a network of Degrees
	if(_heuristics.hasMD) {
		initializeDegrees(puzzle);
	}
	// Constraint Propagation Precursor
	// Perform constraint propagation on the initial set of solved cells.
	if(_heuristics.hasCPP) {
		auto start = std::chrono::system_clock::now();
		propagateConstraintsInitial(puzzle);
		_logger.log().cppTime += getDuration(start);
	}
}

bool Solver::preAssign(Puzzle& puzzle, const Position& toAssignCell, std::size_t val) {
	// Check if move is legal
	bool res = isLegal(puzzle, toAssignCell, val);
	if(_heuristics.hasFC) {
		res = res && forwardCheck(puzzle, toAssignCell, val);
	}
	return res;
}

void Solver::postAssign(Puzzle& puzzle, const Position& assignedCell) {
	// Nothing for now.

	// Update degrees
	updateDegrees(puzzle, assignedCell, -1);

	// Will contain arc consistency propagation.
	// i.e. From the chosen cell, maintain arc consistency with neighbors, and their neighbors, etc...

	// Constraint Propagation
	// Perform constraint propagation on the assigned cell.
	if(_heuristics.hasCP) {
		auto start = std::chrono::system_clock::now();
		propagateConstraints(puzzle, assignedCell, true);
		_logger.log().cpTime += getDuration(start);
	}
	// Also keep a record of the eliminated values.
	// Forward Check
}

void Solver::assignValue(Puzzle& puzzle, const Position& cell, std::size_t value) {
	Domain previous = puzzle.access(cell).getDomain();
	puzzle.set(cell, value);
	int degrees = _heuristics.hasMD ? _degrees[cell.x][cell.y] : -1;
	_recorder.add(cell, value, degrees, previous);
	if(_heuristics.hasMD) {
		_degrees[cell.x][cell.y] = -1;
	}
}

void Solver::propagateConstraintsInitial(Puzzle& puzzle) {
	for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
		for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
			if(!puzzle.isEmpty(x, y)) {
				propagateConstraints(puzzle, Position(x, y));
			}
		}
	}
}

void Solver::propagateConstraints(Puzzle& puzzle, const Position& cell, bool record) {
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
	Block p = puzzle.getBlock(cell.x, cell.y);
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

Position Solver::selectByMRV(const Puzzle& puzzle) {
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
	_logger.log().mrvTime += getDuration(start);
	if(min == puzzle.getSize() + 1) {
		/* There were no empty cells */
		throw utils::Error::No_Empty_Cells;
	}
	return smallest;
}

std::vector<Position> Solver::orderByMRV(const Puzzle& puzzle) {
	auto start = std::chrono::system_clock::now();

	/* Find the smallest domain size */
	std::size_t min = puzzle.getSize() + 1;
	for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
		for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
			if(puzzle.isEmpty(x, y) && puzzle.getDomainSize(x, y) < min) {
				min = puzzle.getDomainSize(x, y);
			}
		}
	}

	if(min == puzzle.getSize() + 1) {
		_logger.log().mrvTime += getDuration(start);
		throw utils::Error::No_Empty_Cells;
	}
	std::vector<Position> smallests;
	for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
		for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
			if(puzzle.isEmpty(x, y) && puzzle.getDomainSize(x, y) == min) {
				smallests.push_back(Position(x, y));
			}
		}
	}
	_logger.log().mrvTime += getDuration(start);
	return smallests;
}

void Solver::initializeDegrees(const Puzzle& puzzle) {
	_degrees = utils::create<int>(puzzle.getSize(), puzzle.getSize());
	for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
		for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
			if(puzzle.isEmpty(x, y)) {
				std::vector<Position> neighbors = puzzle.getNeighbors(x, y, true);
				_degrees[x][y] = neighbors.size();
			}
			else {
				_degrees[x][y] = -1;
			}
		}
	}
}

Position Solver::selectByMD(const Puzzle& puzzle) {
	/* Find the maximum degree and the first position that has that degree */
	auto start = std::chrono::system_clock::now();
	int max = -1;
	Position maxPos;
	for(std::size_t x = 0; x < puzzle.getSize(); ++x) {
		for(std::size_t y = 0; y < puzzle.getSize(); ++y) {
			if(_degrees[x][y] > max) {
				max = _degrees[x][y];
				maxPos = Position(x, y);
			}
		}
	}
	_logger.log().mdTime += getDuration(start);
	if(max == -1) {
		throw utils::Error::No_Empty_Cells;
	}
	return maxPos;
}

Position Solver::selectByMD(const std::vector<Position>& subset) {
	auto start = std::chrono::system_clock::now();
	int max = -1;
	Position maxPos;
	for(auto i : subset) {
		if(_degrees[i.x][i.y] > max) {
			max = _degrees[i.x][i.y];
			maxPos = i;
		}
	}
	_logger.log().mdTime += getDuration(start);
	if(max == -1) {
		throw utils::Error::No_Empty_Cells;
	}
	return maxPos;
}

void Solver::updateDegrees(const Puzzle& puzzle, const Position& cell, int change) {
	if(!_heuristics.hasMD) {
		return;
	}
	auto start = std::chrono::system_clock::now();
	std::vector<Position> neighbors = puzzle.getNeighbors(cell, true);
	for(std::vector<Position>::iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
		_degrees[it->x][it->y] += change;
	}
	_logger.log().mduTime += getDuration(start);
}

std::vector<std::size_t> Solver::orderByLCV(const Puzzle& puzzle, const Position& cell) {
	auto start = std::chrono::system_clock::now();
	/* Get a list of domain values */
	std::vector<std::pair<std::size_t, int>> values;
	Domain d = puzzle.getDomain(cell);
	for(std::size_t i = 0; i < d.values.size(); ++i) {
		if(d.values[i]) {
			values.push_back(std::make_pair(i, 0));
		}
	}

	/* For each value, check the neighbors domains to see if this value exists */
	std::vector<Position> neighbors = puzzle.getNeighbors(cell, true);
	for(std::size_t i = 0; i < values.size(); ++i) {
		for(auto& n : neighbors) {
			if(puzzle.getDomain(n).values[values[i].first]) {
				++values[i].second;
			}
		}
	}

	/* Sort the values vector from least to greatest based on their constraints */
	struct pairCompare {
		bool operator()(std::pair<std::size_t, int> l, std::pair<std::size_t, int> r) {
			return l.second < r.second;
		}
	};
	std::sort(values.begin(), values.end(), pairCompare());

	std::vector<std::size_t> lcvValues;
	for(auto& i : values) {
		lcvValues.push_back(i.first);
	}

	_logger.log().lcvTime += getDuration(start);

	return lcvValues;
}

bool Solver::forwardCheck(const Puzzle& puzzle, const Position& cell, std::size_t tentative) {
	auto start = std::chrono::system_clock::now();

	std::vector<Position> neighbors = puzzle.getNeighbors(cell, true);
	/* For every neighbor, check if the tentative value is consistent amongst them */
	for(auto& n : neighbors) {
		if(!isConsistent(puzzle, n, tentative)) {
			_logger.log().fcTime += getDuration(start);
			return false;
		}
	}
	_logger.log().fcTime += getDuration(start);
	return true;
}

bool Solver::isLegal(const Puzzle& puzzle, const Position& cell, std::size_t value) {
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

	Block p = puzzle.getBlock(cell.x, cell.y);
	for(std::size_t i = p.t; i <= p.b; ++i) {
		for(std::size_t j = p.l; j <= p.r; ++j) {
			if(i != x && j != y && !puzzle.isEmpty(i, j) && puzzle.access(i, j).getValue() == value) {
				return false;
			}
		}
	}
	return true;
}

bool Solver::isConsistent(const Puzzle& puzzle, const Position& cell1, const Position& cell2) {
	std::vector<bool> values1 = puzzle.getDomain(cell1).values;
	std::vector<bool> values2 = puzzle.getDomain(cell2).values;
	/* For every value in the domain */
	for(std::size_t i = 0; i < puzzle.getSize(); ++i) {
		if(values1[i]) {
			bool consistent = isConsistent(values2, i);
			if(!consistent) {
				/* If no value is consistent, then these two cells are not consistent */
				return false;
			}
		}
		/* Do the inverse */
		if(values2[i]) {
			bool consistent = isConsistent(values1, i);
			if(!consistent) {
				return false;
			}
		}
	}
	return true;
}

bool Solver::isConsistent(const Puzzle& puzzle, const Position& cell, std::size_t val) {
	std::vector<bool> values = puzzle.getDomain(cell).values;
	return isConsistent(values, val);
}

bool Solver::isConsistent(const std::vector<bool>& values, std::size_t val) {
	for(std::size_t i = 0; i < values.size(); ++i) {
		if(values[i] && i != val) {
			return true;
		}
	}
	return false;
}

double Solver::getDuration(const std::chrono::time_point<std::chrono::system_clock>& start) const {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<long double> duration = end - start;
	return duration.count();
}
