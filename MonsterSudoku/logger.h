#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>

struct Log {
	/* Time stats */
	long double totalTime;
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

	/* Statuses */
	bool solved;
	bool solvable;
	bool timeout;

	Log() {
		totalTime = 0;
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
		nodes = 0;
		allNodes = 0;
		deadEnds = 0;
		solved = false;
		solvable = false;
		timeout = false;
	}

	friend std::ostream& operator <<(std::ostream& out, const Log& l) {
		out << "Total: " << l.totalTime << "s, Pre: " << l.preTime << "s, Backtrack: " << l.btTime << "s, CPP: " << l.cppTime << "s, CP: " << l.cpTime << "s, MRV: " << l.mrvTime << "s, LCV: " << l.lcvTime << "s, MD: " << l.mdTime << "s, MDU: " << l.mduTime <<"s, ACP: " << l.acpTime << "s, MAC: " << l.macTime << "s, FC: " << l.fcTime << "s, Legal Nodes: " << l.nodes << ", All Nodes: " << l.allNodes << ", Dead Ends: " << l.deadEnds << ", Solved: " << l.solved << ", Solvable: " << l.solvable << ", Timeout: " << l.timeout;
		return out;
	}
};

class Logger {
public:
	Logger();

	Log& log();
	Log log() const;

	friend std::ostream& operator <<(std::ostream& out, const Logger& l);
private:
	Log _log;
};

#endif // LOGGER_H
