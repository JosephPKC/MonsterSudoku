#include "sudokureader.h"


SudokuReader::SudokuReader() {
    setInputFileName(nullptr);
    setOutputFileName(nullptr);
}

SudokuReader::SudokuReader (char* inputFileName, char* outputFileName) {
    setInputFileName(inputFileName);
    setOutputFileName(outputFileName);
}

SudokuReader::SudokuReader (const SudokuReader& S) {
    setInputFileName(S.getInputFileName());
    setOutputFileName(S.getOutputFileName());
}

SudokuReader& SudokuReader::operator = (const SudokuReader& S) {
    destroyFileName(_inputFileName);
    destroyFileName(_outputFileName);
    setInputFileName(S.getInputFileName());
    setOutputFileName(S.getOutputFileName());
    return *this;
}

SudokuReader::~SudokuReader () {
}

void SudokuReader::setInputFileName (char* inputFileName) {
    _inputFileName = inputFileName;
}

void SudokuReader::setOutputFileName (char* outputFileName) {
    _outputFileName = outputFileName;
}

char* SudokuReader::getInputFileName () const {
    return _inputFileName;
}

char* SudokuReader::getOutputFileName () const {
    return _outputFileName;
}
char** SudokuReader::loadSudoku (int& m, int& n, int& p, int& q, long timeout) {
    std::ifstream input(_inputFileName);
    assert (input.is_open ());
    input >> m >> n >> p >> q;
    input.close ();
    try {
        SudokuGenerator S (m,n,p,q);
        return S.generateSudoku (timeout);
    }
    catch (const char e) {
        char** error = new char*[1];
        *error = new char[1];
        **error = e;
        return error;
    }
}

char** SudokuReader::loadSudoku (int& n, int& p, int& q, long timeout) {
    std::ifstream input(_inputFileName);
    assert (input.is_open ());
    input >> n >> p >> q;
    char** sudoku = new char*[n];
    for (std::size_t i = 0; i < n; ++i) {
        sudoku[i] = new char[n];
		for (std::size_t j = 0; j < n; ++j) {
			sudoku[i][j] = '0';
		}
    }
	char space;
	char temp;
    for (std::size_t x = 0; x < n; ++x) {
        for (std::size_t y = 0; y < n; ++y) {
			input >> temp;
			if (temp != '\0' && temp != ' ') {
				sudoku[x][y] = temp;
			}
        }
    }
	input.close();
	return sudoku;
}

void SudokuReader::saveSudoku (int m, int n, int p, int q, char** sudoku, char e) {
    std::ofstream output(_outputFileName);
    assert (output.is_open());
    if (e == Error::kInputError) {
        output << "error: invalid input parameters";
    }
    else if (e == Error::kTimeoutError) {
        output << "timeout";
    }
    else {
        output << n << " " << p << " " << q << "\n";
        for (std::size_t x = 0; x < (std::size_t) n; ++x) {
            for (std::size_t y = 0; y < (std::size_t) n; ++y) {
                output << sudoku[x][y] << " ";
            }
            output << "\n";
        }
    }
    output.close ();
}

void SudokuReader::destroyFileName(char*& fileName) {
    if (fileName) {
        delete[] fileName;
        fileName = nullptr;
    }
}
