#ifndef SOLVER_H
#define SOLVER_H
#include <chrono>
#include <ctime>
#include "puzzle.h"
#include "recorder.h"
/* Create naive backtracking search first */
/* Implement random naive select next variable and select value functions */
/* Implement naive pre process function */
/* Implement naive backtrack function */
/* Implement naive post assign function */
struct Heuristics {
	bool hasMRV;
	bool hasDH;
	bool hasLCV;
	bool hasACP;
	bool hasMAC;
	bool hasFC;

	Heuristics() {
		hasMRV = false;
		hasDH = false;
		hasLCV = false;
		hasACP = false;
		hasMAC = false;
		hasFC = false;
	}

	friend std::ostream& operator << (std::ostream& out, const Heuristics& h) {
		out << h.hasMRV << h.hasDH << h.hasLCV << h.hasACP << h.hasMAC << h.hasFC;
		return out;
	}
};

class Solver {
public:
	Solver();

	Solver(bool mrv, bool dh, bool lcv, bool acp, bool mac, bool fc);

	Heuristics& heuristics();

	Puzzle solve(Puzzle puzzle, double timeout);
private:
	Heuristics _heuristics;
	Recorder _recorder;
	std::chrono::time_point<std::chrono::system_clock> _start;

	/* Recursive backtracking search algorithm */
	utils::Error search(Puzzle& puzzle, double timeout);
	/* Algorithm to select next cell */
	Position selectNextCell(Puzzle puzzle);
	/* Algorithm to order values for cell */
	std::vector<std::size_t> orderValues(Puzzle puzzle, Position cell);
	/* Algorithm to backtrack */
	void backtrack(Puzzle& puzzle);
	/* Algorithm for any pre-processing */
	void preSearch(Puzzle& puzzle);
	/* Algorithm for any pre-assigment processing */
	bool preAssign(Puzzle& puzzle, Position toAssignCell, std::size_t val);
	/* Algorithm for any post-assignment processing */
	void postAssign(Puzzle& puzzle, Position assignedCell);
	/* Assign a value to cell */
	void assignValue(Puzzle& puzzle, Position cell, std::size_t value);
	/* Check if a value is legal */
	bool isLegal(Puzzle puzzle, Position cell, std::size_t value);
};

#endif // SOLVER_H
