#include "reporter.h"

Reporter::Reporter() {

}

void Reporter::generateTestPuzzles(int a, const std::string& prefix, int m, int n, int p, int q) {
	Generator g;
	for(int i = 0; i < a; ++i) {
		Puzzle puzzle = g.generate(m, n, p, q);
		writePuzzleToFile(prefix + std::to_string(i + 1) + ".txt", puzzle);
	}
}

void Reporter::generateTestPuzzles(const std::string& prefix, const std::vector<int>& m, int n, int p, int q) {
	Generator g;
	for(std::size_t i = 0; i < m.size(); ++i) {
		Puzzle puzzle = g.generate(m[i], n, p, q);
writePuzzleToFile(prefix + std::to_string(i + 1) + ".txt", puzzle);
	}
}

void Reporter::generateTestPuzzles(const std::string& prefix, const std::vector<int>& m, const std::vector<int>& n, const std::vector<int>& p, const std::vector<int>& q) {
	Generator g;
	for(std::size_t i = 0; i < m.size(); ++i) {
		Puzzle puzzle = g.generate(m[i], n[i], p[i], q[i]);
writePuzzleToFile(prefix + std::to_string(i + 1) + ".txt", puzzle);
	}
}

Report Reporter::runHeuristicsAnalysis(const std::vector<std::string>& paths, const Heuristics& combination, double timeout) {
	/* Run the tests */
	Generator g;
	Report r;
	for(std::size_t i = 0; i < paths.size(); ++i) {
		Solver s(combination);
		try {

			Puzzle p = g.generate(paths[i]);
			std::cout << std::endl << "Loaded " << paths[i] << std::endl;
			std::cout << "Heuristic: " << combination << std::endl;
			std::cout << p << std::endl;
			Puzzle ps = s.solve(p, timeout);
			std::cout << ps << std::endl;
			std::cout << s.getLog() << std::endl;
		}
		catch(utils::Error e) {
			std::cout << s.getLog() << std::endl;
		}

		r.avg = r.avg + createReportFromLog(s.getLog());
		if(s.getLog().solvable) {
			++r.solvable;
		}
		else if(s.getLog().timeout) {
			++r.timeout;
		}
	}
	/* Set the averages */
	setAverage(r.avg, paths.size());
	r.solvable /= paths.size();
	r.timeout /= paths.size();
	r.completable = 1.0 - r.timeout;

	return r;
}

ReportInfo Reporter::createReportFromLog(const Log& l) {
	ReportInfo r;
	r.acpTime = l.acpTime;
	r.macTime = l.macTime;
	r.fcTime = l.fcTime;
	r.mrvTime = l.mrvTime;
	r.mdTime = l.mdTime;
	r.lcvTime = l.lcvTime;
	r.cppTime = l.cppTime;
	r.cpTime = l.cpTime;
	r.mduTime = l.mduTime;
	r.btTime = l.btTime;
	r.totalTime = l.totalTime;
	r.preTime = l.preTime;
	r.solveTime = l.solveTime;
	r.nodes = l.nodes;
	r.deadEnds = l.deadEnds;
	r.allNodes = l.allNodes;
	return r;
}

void Reporter::setAverage(ReportInfo& r, int n) {
	r.acpTime /= n;
	r.macTime /= n;
	r.fcTime /= n;
	r.mrvTime /= n;
	r.mdTime /= n;
	r.lcvTime /= n;
	r.cppTime /= n;
	r.cpTime /= n;
	r.mduTime /= n;
	r.btTime /= n;
	r.totalTime /= n;
	r.preTime /= n;
	r.solveTime /= n;
	r.nodes /= n;
	r.deadEnds /= n;
	r.allNodes /= n;
}

void Reporter::writePuzzleToFile(const std::string& path, const Puzzle& p) {
	std::ofstream out(path);
	if(out.is_open()) {
		out << p.getM() << " " << p.getN() << " " << p.getP() << " " << p.getQ() << std::endl;
		for(std::size_t x = 0; x < p.getSize(); ++x) {
			for(std::size_t y = 0; y < p.getSize(); ++y) {
				out << (p.isEmpty(x, y) ? '.' : utils::convertIndexToChar(p.access(x, y).getValue()));
			}
			out << std::endl;
		}
	}
}
