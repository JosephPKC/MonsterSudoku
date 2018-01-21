#ifndef GENERATOR_H
#define GENERATOR_H
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <ctime>
#include "puzzle.h"

/* Generator must be able to generate a puzzle from four params, or create a data class from a text file.
 * Generator must be able to write to a file
 * Generator will handle opening and closing of text files
 * Generator will not see if the puzzle is solvable
 * Generator will ensure that the initial state is legal
 */

/* 35 is the regular max for sudoku size. But larger ones will need to use special symbols and chars, or separate lower and upper.
 * Separate lower and upper gives us 61 max.
 * All unique keys found on a regular keyboard gives us 30 (33 - the 3 reserved ones) extra for a total of 91 max size.
 * If we begin doubling up our symbols so it can be like AA, this obviously exponentiates it by 2.
 */
class Generator {
public:
	Generator();
	Generator(int timeout, int gen = 20);

	bool checkFile(std::string path);
	Puzzle generate(std::string path);
	Puzzle generate(int m, int n, int p, int q);
	Puzzle generateSolvable(int m, int n, int p, int q);
	void saveToFile(const Puzzle& puzzle, std::string path);

private:
	int _timeout;
	int _gen;
	bool _areParamsLegal(int m, int n, int p, int q) const;
};

#endif // GENERATOR_H
