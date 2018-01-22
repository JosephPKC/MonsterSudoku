#pragma once
#include <iostream>
#include <sstream>
#include <vector>
namespace utils {
	/* Testing comments control */
#define GLOBAL 0
#define PAUSE GLOBAL || 0
#define DEBUG GLOBAL || 0
#define VERBOSE GLOBAL || 0
#define TIME GLOBAL || 0
	/* Error enumeration */
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
	/* Typedefs */
	typedef std::vector<std::string>::iterator vs_it;	//Typedef for easier typing
	typedef std::vector<bool>::iterator vb_it;
	typedef std::vector<bool>::const_iterator vb_cit;

	/* Argument and Command Constants */
	const std::string GEN_ARG = "-g";
	const std::string VERB_ARG = "-v";
	const std::string MRV_ARG = "-mrv";
	const std::string LCV_ARG = "-lcv";
	const std::string MD_ARG = "-md";
	const std::string ACP_ARG = "-acp";
	const std::string MAC_ARG = "-mac";
	const std::string FC_ARG = "-fc";
	const std::string ALL_ARG = "-all";

	const std::string GEN_CMD = "gen";
	const std::string SOLVE_CMD = "solve";
	const std::string HELP_CMD = "help";

	/* Utility methods */
	std::vector<std::string> splitString(const std::string& s, char delimiter);
	int convertCharToIndex(char c);
	char convertIndexToChar(std::size_t i);
	std::vector<bool> getDefaultBools(int size, bool val);

	/* Template methods */
	template <typename T>
	T* create(std::size_t size);
	template <typename T>
	T** create(std::size_t size, std::size_t inner);
	template <typename T>
	T* makeCopy(T* source, std::size_t size);
	template <typename T>
	T** makeCopy(T** source, std::size_t size);
	template <typename T>
	void deleteArray(T**& array, std::size_t size);
}

template <typename T>
T* utils::create(std::size_t size) {
	return new T[size];
}

template <typename T>
T** utils::create(std::size_t size, std::size_t inner) {
	T** array = new T*[size];
	for(std::size_t i = 0; i < size; ++i) {
		array[i] = create<T>(inner);
	}
	return array;
}

template <typename T>
T* utils::makeCopy(T* source, std::size_t size) {
	T* copy = create<T>(size);
	for(std::size_t i = 0; i < size; ++i) {
		copy[i] = source[i];
	}
	return copy;
}

template <typename T>
T** utils::makeCopy(T** source, std::size_t size) {
	T** copy = create<T>(size, size);
	for(std::size_t i = 0; i < size; ++i) {
		copy[i] = makeCopy(source[i], size);
	}
	return copy;
}

template <typename T>
void utils::deleteArray(T**& array, std::size_t size) {
	for(std::size_t i = 0; i < size; ++i) {
		delete[] array[i];
		array[i] = nullptr;
	}
	delete[] array;
	array = nullptr;
}
