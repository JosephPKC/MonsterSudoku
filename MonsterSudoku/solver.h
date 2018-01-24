#ifndef SOLVER_H
#define SOLVER_H
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <thread>
#include "puzzle.h"
#include "recorder.h"
#include "logger.h"
/* Implement CP */
/* Implement MRV, DH, and LCV */
/* Implement FC */
/* Implement ACP and MAC */
struct Heuristics {
	bool hasMRV;
	bool hasMD;
	bool hasLCV;
	bool hasACP;
	bool hasMAC;
	bool hasFC;

	Heuristics() {
		hasMRV = false;
		hasMD = false;
		hasLCV = false;
		hasACP = false;
		hasMAC = false;
		hasFC = false;
	}

	Heuristics(bool mrv, bool md, bool lcv, bool acp, bool mac, bool fc) {
		hasMRV = mrv;
		hasMD = md;
		hasLCV = lcv;
		hasACP = acp;
		hasMAC = mac;
		hasFC = fc;
	}

	void toggleAll(bool val) {
		hasMRV = hasMD = hasLCV = hasACP = hasMAC = hasFC = val;
	}

	friend std::ostream& operator << (std::ostream& out, const Heuristics& h) {
		out << h.hasMRV << h.hasMD << h.hasLCV << h.hasACP << h.hasMAC << h.hasFC;
		return out;
	}
};

class Solver {
public:
	Solver();

	Solver(const Heuristics& heuristics);
	Solver(bool mrv, bool dh, bool lcv, bool acp, bool mac, bool fc);

	Heuristics& getHeuristics();
	Log getLog() const;

	void setUpdate(bool val);
	void setPause(bool val);
	void setSleep(bool val);

	Puzzle solve(const Puzzle& puzzle, double timeout = 300);
private:
	Heuristics _heuristics;
	Recorder _recorder;
	Logger _logger;
	std::chrono::time_point<std::chrono::system_clock> _start;
	int** _degrees;
	bool _update;
	bool _pause;
	bool _sleep;

	/* Outer algorithms */
	utils::Error search(Puzzle& puzzle, double timeout);
	Position selectNextCell(const Puzzle& puzzle);
	std::vector<std::size_t> orderValues(const Puzzle& puzzle, const Position& cell);
	void backtrack(Puzzle& puzzle);
	bool preSearch(Puzzle& puzzle);
	bool preAssign(Puzzle& puzzle, const Position& toAssignCell, std::size_t val);
	bool postAssign(Puzzle& puzzle, const Position& assignedCell);
	void assignValue(Puzzle& puzzle, const Position& cell, std::size_t value);
	void propagateConstraintsInitial(Puzzle& puzzle);
	void propagateConstraints(Puzzle& puzzle, const Position& cell, bool record = false);

	/* Heuristic methods */
	Position selectByMRV(const Puzzle& puzzle);
	std::vector<Position> orderByMRV(const Puzzle& puzzle);
	void initializeDegrees(const Puzzle& puzzle);
	Position selectByMD(const Puzzle& puzzle);
	Position selectByMD(const std::vector<Position>& subset);
	void updateDegrees(const Puzzle& puzzle, const Position& cell, int change);
	std::vector<std::size_t> orderByLCV(const Puzzle& puzzle, const Position& cell);
	bool forwardCheck(Puzzle& puzzle, const Position& cell);
	bool initialArcConsistency(Puzzle& puzzle);
	bool maintainArcConsistency(Puzzle& puzzle, const Position& cell);

	/* Helper methods */
	bool isLegal(const Puzzle& puzzle, const Position& cell, std::size_t value);
	bool isConsistent(const Puzzle& puzzle, const Position& cell1, const Position& cell2, std::size_t& val);
	bool isConsistent(const Puzzle& puzzle, const Position& cell, std::size_t val);
	bool isConsistent(const std::vector<bool>& values, std::size_t val);
	std::vector<std::size_t> getInconsistentValues(const Puzzle& puzzle, const Position& cell1, const Position& cell2);
	double getDuration(const std::chrono::time_point<std::chrono::system_clock>& start) const;
	bool reduceArc(Puzzle& puzzle, const Position& cell1, const Position& cell2, bool record = false);
	void updateDisplay(const Puzzle& puzzle, const Position& changedCell);
};

#endif // SOLVER_H
