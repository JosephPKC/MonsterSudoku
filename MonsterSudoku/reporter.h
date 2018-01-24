#ifndef REPORTER_H
#define REPORTER_H
#include "generator.h"
#include "solver.h"
/* Run a series of tests based on a string (file path) array */
/* Sample Methods
 * runtests (std::array(string file paths), heuristics combo) - runs the solver with the heuristics combo on each of the puzzles in the file paths array
 *		Do the above for each heuristics combo for Part I
 */
struct ReportInfo {
	double totalTime;
	double solveTime;
	double preTime;
	double btTime;
	double cppTime;
	double cpTime;
	double mrvTime;
	double lcvTime;
	double mdTime;
	double acpTime;
	double macTime;
	double fcTime;
	double mduTime;
	/* Counts */
	int nodes;
	int allNodes;
	int deadEnds;

	ReportInfo() {
		totalTime = 0;
		solveTime = 0;
		preTime = 0;
		btTime = 0;
		cppTime = 0;
		cpTime = 0;
		mrvTime = 0;
		lcvTime = 0;
		mdTime = 0;
		acpTime = 0;
		macTime = 0;
		fcTime = 0;
		mduTime = 0;
		/* Counts */
		nodes = 0;
		allNodes = 0;
		deadEnds = 0;
	}

	friend ReportInfo operator +(ReportInfo& l, const ReportInfo& o) {
		ReportInfo r;
		r.totalTime = l.totalTime + o.totalTime;
		r.solveTime = l.solveTime + o.solveTime;
		r.preTime = l.preTime + o.preTime;
		r.cppTime = l.cppTime + o.cppTime;
		r.cpTime = l.cpTime + o.cpTime;
		r.mrvTime = l.mrvTime + o.mrvTime;
		r.mdTime = l.mdTime + o.mdTime;
		r.lcvTime = l.lcvTime + o.lcvTime;
		r.acpTime = l.acpTime + o.acpTime;
		r.macTime = l.macTime + o.macTime;
		r.fcTime = l.fcTime + o.fcTime;
		r.btTime = l.btTime + o.btTime;
		r.mduTime = l.mduTime + o.mduTime;
		r.nodes = l.nodes + o.nodes;
		r.allNodes = l.allNodes + o.allNodes;
		r.deadEnds = l.deadEnds + o.deadEnds;
		return r;
	}

	friend std::ostream& operator <<(std::ostream& out, const ReportInfo& l) {
		out << "Total: " << l.totalTime << "s, Solve: " << l.solveTime << "s, Pre: " << l.preTime << "s, Backtrack: " << l.btTime << "s, CPP: " << l.cppTime << "s, CP: " << l.cpTime << "s, MRV: " << l.mrvTime << "s, LCV: " << l.lcvTime << "s, MD: " << l.mdTime << "s, MDU: " << l.mduTime <<"s, ACP: " << l.acpTime << "s, MAC: " << l.macTime << "s, FC: " << l.fcTime << "s, All Nodes: " << l.allNodes << ", Legal Nodes: " << l.nodes << ", Dead Ends: " << l.deadEnds;
		return out;
	}
};

struct Report {
	ReportInfo avg;			// Average times and nodes
	double solvable;		// Percentage solvable
	double timeout;			// Percentage timeout
	double completable;		// Percentage completable = !timeout

	Report() {
		solvable = timeout = completable = 0;
	}

	friend std::ostream& operator <<(std::ostream& out, const Report& r) {
		out << r.avg << ", % Solvable: " << r.solvable << ", % Timeout: " << r.timeout << ", % Completable: " << r.completable;
		return out;
	}
};

class Reporter {
public:
	Reporter();
	/* Generate a puzzles of the same parameters */
	void generateTestPuzzles(int a, const std::string& prefix, int m, int n, int p, int q);
	/* Generate a puzzles of different m parameters */
	void generateTestPuzzles(const std::string& prefix, const std::vector<int>& m, int n, int p, int q);
	/* Generate a puzzles of different m, n, p, q parameters */
	void generateTestPuzzles(const std::string& prefix, const std::vector<int>& m, const std::vector<int>& n, const std::vector<int>& p, const std::vector<int>& q);

	/* Run the solver with heuristics for each puzzle, and find average */
	Report runHeuristicsAnalysis(const std::vector<std::string>& paths, const Heuristics& combination, double timeout);

private:
	ReportInfo createReportFromLog(const Log& l);
	void setAverage(ReportInfo& r, int n);
	void writePuzzleToFile(const std::string& path, const Puzzle& p);
};

#endif // REPORTER_H
