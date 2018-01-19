#pragma once
#include <iostream>
#include <sstream>
#include <vector>
namespace utils {
#define DEBUG 0
#define VERBOSE DEBUG || 0
#define TIME DEBUG || 1
	enum Error {
		Success,
		No_Input,
		Not_Enough_Args,
		No_Integers,
		Unknown_Command,
		File_Not_Found,
		Bad_File_Format,
		Illegal_Puzzle,
		Bad_Parameters,
		Timeout,
		No_Records,
		No_Empty_Cells,
		No_More_Values
	};

	typedef std::vector<std::string>::iterator vs_it;	//Typedef for easier typing
	typedef std::vector<bool>::iterator vb_it;
	typedef std::vector<bool>::const_iterator vb_cit;

	const std::string GEN_ARG = "-g";			// Argument to gen puzzles
	const std::string VERB_ARG = "-v";			// Argument to make verbose reports

	const std::string GEN_CMD = "gen";			// Command to gen a puzzle to file
	const std::string SOLVE_CMD = "solve";		// Command to solve puzzle
	const std::string HELP_CMD = "help";		// Command to display usages

	std::vector<std::string> splitString(const std::string& s, char delimiter);
	int convertCharToIndex(char c);
	char convertIndexToChar(std::size_t i);
	std::vector<bool> getDefaultBools(int size, bool val);
}
