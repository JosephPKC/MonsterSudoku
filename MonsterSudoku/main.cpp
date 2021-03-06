#include "generator.h"
#include "solver.h"
#include "reporter.h"

/* Some important links:
 * https://www.ics.uci.edu/~rickl/courses/AI-projects/sudoku-solver/SudokuProjectAssignments_20160209.pdf
 * http://www.ics.uci.edu/~rickl/courses/AI-projects/sudoku-solver/monster-sudoku/aiproject-monster-sudoku-generator-reader-specs.pdf
 * https://www.ics.uci.edu/~rickl/courses/AI-projects/sudoku-solver/monster-sudoku-final-report-template_rev1_20160310.pdf
 */

/* TODO:
 * More refactoring of Solver + Main
 * Backjumping - Given a failure at some position, backtrack to the most recent assignment that affected that position's domain.
 *		Given a position to backjump from:
 *		From the last record in recorder to the first:
 *		Search the record's propagation vector for the given position.
 *		If found, backtrack and return a special failure state called backjump to X.
 *		Keep failing and backtracking until you backtrack X.
 *		If not found, just backtrack once.
 * Implement DLX (Dancing Lights X) by converting Sudoku to a Doubly Linked List Sparse Matrix and into an Exact Cover Problem and performing Algorithm X on it.
 */

/* DLX Stuff
 * http://buzzard.ups.edu/talks/beezer-2010-AIMS-sudoku.pdf
 */

/* Sudoku to Exact Cover Sparse Matrix */
/* n^3 rows, each representing a unique pair of value and position
 * 4n^2 columns.
 * Each row must have exactly 4 1s.
 * The first set of columns: Each cell can only contain one value.
 *		The pattern is as follows: Every n rows have 1s in the same column, starting at the first column.
 *		After n rows, move up the column to the next one, and begin again with the next n rows.
 *		Repeat
 *		You should see cascading n 1s that move right every n 1s. with all columns having exactly n 1s in a column.
 * The second set: Values in each row must be distinct.
 *		Create a diagonal line of 1s, beginning at the left corner and moving right.
 *		Each line should be of n length.
 *		After n of these lines, move right n spaces, and begin again.
 * Third set: Values in each column must be distinct.
 *		Create a large diagonal line of 1s like above.
 *		The line should be of n^2 length.
 *		Repeat.
 *		This is the same as above, except after every n length line, you move right n spaces. If you hit the end, go back all the way left and start again.
 * Fourth set: Values in each block must be distinct
 *		Create a diagonal line of n 1s, just like above.
 *		Repeat q times, then move right n spaces.
 *		Do the above p times.
 *		By now, it should hit the end, so go back all the way to the left and repeat.
 */

struct Optionals {
	Heuristics heuristics;
	bool verbose;
	bool update;
	bool pause;
	bool sleep;

	Optionals() {
		verbose = false;
		update = false;
		pause = false;
		sleep = false;
	}
};

/* Signatures */
void primary(Generator gen);
utils::Error doCommand(Generator gen, std::vector<std::string> inputs);
void error(utils::Error e);
utils::Error getParameters(std::vector<std::string> inputs, int& m, int& n, int& p, int& q, int skip = 1);
utils::Error getOptionals(Optionals& options, std::vector<std::string> inputs, int skip = 2);

/* Debug and Test Globals */
#define TEST 0
#define PUZZLE_TEST TEST || 0
#define GEN_TEST TEST || 0
#define REC_TEST TEST || 0
#define SOLVE_TEST TEST || 0
#define GEN_SOLV_TEST TEST || 0

int main() {
#if PUZZLE_TEST
	int m = 5, n = 5, p = 5, q = 1;
	char** array = new char*[n];
	for(std::size_t i = 0; i < n; ++i) {
		array[i] = new char[n];
		for(std::size_t j = 0 ; j < n; ++j) {
			array[i][j] = '1';
		}
	}
	array[3][2] = '1';

	Puzzle P(array, m, n, p, q);
	std::cout << P << std::endl;

	std::cout << P.access(3, 2) << std::endl;
	P.set(0, 0, 2);
	std::cout << P << std::endl;
	std::cout << P.isComplete() << std::endl;

	{
		Puzzle Q(P);
		std::cout << Q << std::endl;
		Q.set(0, 0, 1);
		std::cout << Q << std::endl;
		std::cout << P << std::endl;

		Puzzle R(P);
		Q = R;
		std::cout << Q << std::endl;
	}
#endif
#if GEN_TEST
	Generator G;
	std::string path = "..\\MonsterSudoku\\inputs\\test.txt";
	try {
		Puzzle A = G.generate(path);
		std::cout << A << std::endl;

		Puzzle B = G.generate(45, 9, 3, 3);
		std::cout << B << std::endl;

		std::string save_path = "..\\MonsterSudoku\\inputs\\save_test.txt";
		G.saveToFile(B, save_path);
	}
	catch(utils::Error e) {
		error(e);
	}

#endif
#if REC_TEST
	Recorder r;
	r.add(Position(0, 0), 1, Domain(4));
	r.addPropagation(Position(0, 1), 4);
	std::cout << r << std::endl;
	Record l = r.undo();
	std::cout << l << std::endl;
	std::cout << r << std::endl;
#endif
#if SOLVE_TEST
	Generator g;
	Solver s;
	// For the given sudoku
	// No ACP ~0.49s.
	// With ACP ~0.27s.
	s.getHeuristics().hasCPP = true;
	try {
//		Puzzle p = g.generate(17, 9, 3, 3);
		Puzzle p = g.generate("..\\MonsterSudoku\\inputs\\test.txt");
		std::cout << p << std::endl;
		Puzzle ps = s.solve(p, 300);
		std::cout << ps << std::endl;
	}
	catch(utils::Error e) {
		error(e);
	}
	std::cout << s.getLog() << std::endl;

#endif
#if GEN_SOLV_TEST
	Generator g;
	std::cout << g.generateSolvable(17, 9, 3, 3) << std::endl;
#endif
	Generator gen;
	primary(gen);
	return 0;
}

void primary(Generator gen) {
	bool cont = true;			// Control flag for the primary loop

	while(cont) {
		/* Get user input and split */
		std::string input;
		std::cout << ">";
		std::getline(std::cin, input);
		std::vector<std::string> input_tokens = utils::splitString (input, ' ');

		/* Check the length (At least one) */
		if(input_tokens.size() < 1) {
			error(utils::Error::No_Input);
			continue;
		}

		/* Do the command */
		utils::Error res = doCommand(gen, input_tokens);
		if(res != utils::Error::Success) {
			error(res);
			continue;
		}
	}
}

utils::Error doCommand(Generator gen, std::vector<std::string> inputs) {
	/* Check what the command is (first token always) */
	utils::vs_it it = inputs.begin();
	if(it->compare(utils::GEN_CMD) == 0) {
		/* Generate a new sudoku puzzle and save it as a text file */
		/* m, n, p, q, and file path required */
		int m, n, p, q;
		utils::Error res = getParameters (inputs, m, n, p, q);
		if(res != utils::Error::Success) {
			return res;
		}
		/* Get the file path */
		std::string file_path = *std::next(it, 5);

		/* Generate puzzle and save to file */
		Puzzle puzzle = gen.generate(m, n, p, q);
		try {
			gen.saveToFile(puzzle, file_path);
		}
		catch(utils::Error e) {
			return e;
		}

		/* Display puzzle */
		std::cout << puzzle << std::endl;

	}
	else if(it->compare(utils::SOLVE_CMD) == 0) {
		/* Solve the given puzzle */
		if(inputs.size() < 2) {
			return utils::Error::Not_Enough_Args;
		}
		Puzzle pz;
		Optionals options;
		/* First, check if we are given a file OR -g + 4 parameters */
		++it;
		if(it->compare(utils::GEN_ARG) == 0) {
			/* Generate a puzzle then solve it. */
			/* Get parameters */
			int m, n, p, q;
			utils::Error res = getParameters (inputs, m, n, p, q, 2);
			if(res != utils::Error::Success) {
				return res;
			}
			res = getOptionals(options, inputs, 6);
			if(res != utils::Error::Success) {
				return res;
			}
			/* Generate puzzle */
			Generator g;
			try {
				pz = g.generate(m, n, p, q);
			}
			catch(utils::Error e) {
				return e;
			}
		}
		else {
			/* Generate a puzzle from file */
			/* Get file path */
			std::string file_path = *it;
			Generator g;
			try {
				pz = g.generate(file_path);
			}
			catch(utils::Error e) {
				return e;
			}
			utils::Error res = getOptionals(options, inputs);
			if(res != utils::Error::Success) {
				return res;
			}
		}
		/* Display puzzle */
		std::cout << pz << std::endl;

		/* Solve puzzle */
		Solver s(options.heuristics);
		s.setUpdate(options.update);
		s.setPause(options.pause);
		s.setSleep(options.sleep);
		try {
			Puzzle ps = s.solve(pz, 500);

			/* Display solution & reports */
			std::cout << ps << std::endl;
			if(options.verbose) {
				std::cout << s.getLog() << std::endl;
			}
		}
		catch(utils::Error e) {
			if(options.verbose && (e == utils::Error::Timeout || e == utils::Error::No_More_Values)) {
				std::cout << s.getLog() << std::endl;
			}
			return e;
		}
	}
	else if(it->compare(utils::HELP_CMD) == 0) {
		std::cout << "gen m n p q file_path: Generate a random puzzle with parameters m (initially filled cells), n (size), p (block length), q (block width), and save to file_path. Constraints: 0 < m < n^2, p * q = n, m, n, p, q > 0." << std::endl;
		std::cout << "solve file_path | [-g m n p q] (-v): Solve a given puzzle. Either load a puzzle from a file via file_path, or generate a random puzzle via m, n, p, and q. Use -v to make the end report detailed." << std::endl;
	}
	else {
		return utils::Error::Unknown_Command;
	}
	return utils::Error::Success;
}

void error(utils::Error e) {
	switch(e) {
		case utils::Error::No_Input: {
			std::cout << "No input captured." << std::endl;
			break;
		}
		case utils::Error::Not_Enough_Args: {
			std::cout << "Not enough arguments." << std::endl;
			break;
		}
		case utils::Error::No_Integers: {
			std::cout << "Parameters must be integers." << std::endl;
			break;
		}
		case utils::Error::Unknown_Command: {
			std::cout << "Unknown command." << std::endl;
			break;
		}
		case utils::Error::File_Not_Found: {
			std::cout << "File not found." << std::endl;
			break;
		}
		case utils::Error::Bad_File_Format: {
			std::cout << "Wrong file format. Expected m n p q on the first line, and then an n x n array of ., numbers, or letters." << std::endl;
			break;
		}
		case utils::Error::Illegal_Puzzle: {
			std::cout << "Given puzzle violates constraints. Each cell must have a value that is unique in their row, column, and block." << std::endl;
			break;
		}
		case utils::Error::Bad_Parameters: {
			std::cout << "Given parameters violate constraints. All must be positive integers, m < n^2, and pq = n." << std::endl;
			break;
		}
		case utils::Error::Timeout: {
			std::cout << "Timeout occurred. " << std::endl;
			break;
		}
		case utils::Error::No_Records: {
			std::cout << "No records to undo." << std::endl;
			break;
		}
		case utils::Error::No_Empty_Cells: {
			std::cout << "No more empty cells." << std::endl;
			break;
		}
		case utils::Error::No_More_Values: {
			std::cout << "No more legal values for at least one cell." << std::endl;
			break;
		}
		default: {
			std::cout << "Unknown error." << std::endl;
		}
	}
}

utils::Error getParameters(std::vector<std::string> inputs, int& m, int& n, int& p, int& q, int skip) {
	if(inputs.size() < (unsigned int)4 + skip) {
		return utils::Error::Not_Enough_Args;
	}
	utils::vs_it it = std::next(inputs.begin(), skip);
	/* Get the four parameters */
	try {
		m = std::stoi(*it);
		n = std::stoi(*++it);
		p = std::stoi(*++it);
		q = std::stoi(*++it);
	}
	catch(const std::invalid_argument& e) {
		return utils::Error::No_Integers;
	}
	catch(const std::out_of_range& e) {
		std::cerr << "Went out of range when iterating through the inputs. Should not happen." << std::endl;
		std::exit(1);
	}
	return utils::Error::Success;
}

utils::Error getOptionals(Optionals& options, std::vector<std::string> inputs, int skip) {
	if(inputs.size() < (unsigned int)skip) {
		return utils::Error::Not_Enough_Args;
	}

	for(utils::vs_it it = std::next(inputs.begin(), skip); it != inputs.end(); ++it) {
		if(it->compare(utils::VERB_ARG) == 0) {
			options.verbose = true;
		}
		else if(it->compare(utils::UPD_ARG) == 0) {
			options.update = true;
		}
		else if(it->compare(utils::PAU_ARG) == 0) {
			options.pause = true;
		}
		else if(it->compare(utils::SLP_ARG) == 0) {
			options.sleep = true;
		}
		else if(it->compare(utils::MRV_ARG) == 0) {
			options.heuristics.hasMRV = true;
		}
		else if(it->compare(utils::LCV_ARG) == 0) {
			options.heuristics.hasLCV = true;
		}
		else if(it->compare(utils::MD_ARG) == 0) {
			options.heuristics.hasMD = true;
		}
		else if(it->compare(utils::ACP_ARG) == 0) {
			options.heuristics.hasACP = true;
		}
		else if(it->compare(utils::MAC_ARG) == 0) {
			options.heuristics.hasMAC = true;
		}
		else if(it->compare(utils::FC_ARG) == 0) {
			options.heuristics.hasFC = true;
		}
		else if(it->compare(utils::DO_ARG) == 0) {
			options.heuristics.hasMRV = true;
			options.heuristics.hasMD = true;
			options.heuristics.hasLCV = true;
		}
		else if(it->compare(utils::CP_ARG) == 0) {
			options.heuristics.hasACP = true;
			options.heuristics.hasMAC = true;
			options.heuristics.hasFC = true;
		}
		else if(it->compare(utils::ALL_ARG) == 0) {
			options.heuristics.toggleAll(true);
		}
	}
	return utils::Error::Success;
}
