#ifndef SOLVER_H
#define SOLVER_H
#include <chrono>
#include <ctime>
#include <algorithm>
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
	bool hasCPP;
	bool hasCP;

	Heuristics() {
		hasMRV = false;
		hasMD = false;
		hasLCV = false;
		hasACP = false;
		hasMAC = false;
		hasFC = false;
		hasCPP = false;
		hasCP = false;
	}

	void toggleAll(bool val) {
		hasMRV = hasMD = hasLCV = hasACP = hasMAC = hasFC = hasCPP = hasCP = val;
	}

	friend std::ostream& operator << (std::ostream& out, const Heuristics& h) {
		out << h.hasMRV << h.hasMD << h.hasLCV << h.hasACP << h.hasMAC << h.hasFC << h.hasCPP << h.hasCP;
		return out;
	}
};

class Solver {
public:
	Solver();

	Solver(const Heuristics& heuristics);
	Solver(bool mrv, bool dh, bool lcv, bool acp, bool mac, bool fc, bool cpp, bool cp);

	Heuristics& getHeuristics();
	Log getLog() const;

	Puzzle solve(const Puzzle& puzzle, double timeout = 300);
private:
	Heuristics _heuristics;
	Recorder _recorder;
	Logger _logger;
	std::chrono::time_point<std::chrono::system_clock> _start;
	int** _degrees;

	/* Outer algorithms */
	utils::Error search(Puzzle& puzzle, double timeout);
	Position selectNextCell(const Puzzle& puzzle);
	std::vector<std::size_t> orderValues(const Puzzle& puzzle, const Position& cell);
	void backtrack(Puzzle& puzzle);
	void preSearch(Puzzle& puzzle);
	bool preAssign(Puzzle& puzzle, const Position& toAssignCell, std::size_t val);
	void postAssign(Puzzle& puzzle, const Position& assignedCell);
	void assignValue(Puzzle& puzzle, const Position& cell, std::size_t value);

	/* Heuristic methods */
	void propagateConstraintsInitial(Puzzle& puzzle);
	void propagateConstraints(Puzzle& puzzle, const Position& cell, bool record = false);
	Position selectByMRV(const Puzzle& puzzle);
	std::vector<Position> orderByMRV(const Puzzle& puzzle);
	void initializeDegrees(const Puzzle& puzzle);
	Position selectByMD(const Puzzle& puzzle);
	Position selectByMD(const std::vector<Position>& subset);
	void updateDegrees(const Puzzle& puzzle, const Position& cell, int change);
	std::vector<std::size_t> orderByLCV(const Puzzle& puzzle, const Position& cell);
	bool forwardCheck(const Puzzle& puzzle, const Position& cell, std::size_t tentative);
	void maintainArcConsistency(Puzzle& puzzle, bool initial = false);

	/* Helper methods */
	bool isLegal(const Puzzle& puzzle, const Position& cell, std::size_t value);
	bool isConsistent(const Puzzle& puzzle, const Position& cell1, const Position& cell2, std::size_t& val);
	bool isConsistent(const Puzzle& puzzle, const Position& cell, std::size_t val);
	bool isConsistent(const std::vector<bool>& values, std::size_t val);
	std::vector<std::size_t> getInconsistentValues(const Puzzle& puzzle, const Position& cell1, const Position& cell2);
	double getDuration(const std::chrono::time_point<std::chrono::system_clock>& start) const;
};

#endif // SOLVER_H
