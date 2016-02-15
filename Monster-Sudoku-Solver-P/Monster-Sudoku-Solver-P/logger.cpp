#include "logger.h"

Logger::Logger()
{

}

Logger::Logger (LogBundle log) {
    _log = log;
}

void Logger::logInfo (char* filename) {
    std::ofstream output (filename);
    if (output.is_open ()) {
//        output << "DEBUG" << std::endl;
        output << "TOTAL_START=" << _log._totalStart << std::endl;
        output << "PREPROCESSING_START=" << _log._preStart << std::endl;
        output << "PREPROCESSING_DONE=" << _log._preDone << std::endl;
        output << "SEARCH_START=" << _log._searchStart << std::endl;
        output << "SEARCH_DONE=" << _log._searchDone << std::endl;
        output << "SOLUTION_TIME=" << _log._solutionTime << std::endl;
        output << "STATUS=";
        switch (_log._status) {
            case -1: {
                output << "timeout";
                break;
            }
            case 0: {
                output << "error";
                break;
            }
            case 1: {
                output << "success";
                break;
            }
        }
        output << std::endl;
        output << "SOLUTION=(";
        for (std::size_t i = 0; i < _log._solution.size(); ++i) {
            output << _log._solution[i].second;
            if (i != _log._solution.size() - 1) {
                output << ",";
            }
        }
        output << ")" << std::endl;
        output << "COUNT_NODES=" << _log._nodes << std::endl;
        output << "COUNT_DEADENDS=" << _log._deadends << std::endl;
    }
    output.close ();
}

void Logger::printInfo() {
//        std::cout << "DEBUG" << std::endl;
    std::cout << "TOTAL_START=" << _log._totalStart << std::endl;
    std::cout << "PREPROCESSING_START=" << _log._preStart << std::endl;
    std::cout << "PREPROCESSING_DONE=" << _log._preDone << std::endl;
    std::cout << "SEARCH_START=" << _log._searchStart << std::endl;
    std::cout << "SEARCH_DONE=" << _log._searchDone << std::endl;
    std::cout << "SOLUTION_TIME=" << _log._solutionTime << std::endl;
    std::cout << "STATUS=";
    switch (_log._status) {
        case -1: {
            std::cout << "timeout";
            break;
        }
        case 0: {
            std::cout << "error";
            break;
        }
        case 1: {
            std::cout << "success";
            break;
        }
    }
    std::cout << std::endl;
    std::cout << "SOLUTION=(";
    for (std::size_t i = 0; i < _log._solution.size(); ++i) {
        std::cout << _log._solution[i].second;
        if (i != _log._solution.size() - 1) {
            std::cout << ",";
        }
    }
    std::cout << ")" << std::endl;
    std::cout << "COUNT_NODES=" << _log._nodes << std::endl;
    std::cout << "COUNT_DEADENDS=" << _log._deadends << std::endl;
}
