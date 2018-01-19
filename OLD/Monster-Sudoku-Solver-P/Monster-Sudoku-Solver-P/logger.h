#ifndef LOGGER_H
#define LOGGER_H
#include "sudokupuzzle.h"
#include <fstream>
#include <cassert>
struct LogBundle {
    long _totalStart;
    long _preStart;
    long _preDone;
    long _searchStart;
    long _searchDone;
    long _solutionTime;
    int _status;
    std::vector<std::pair<Position,char>> _solution;
    int _nodes;
    int _deadends;
    LogBundle () {
        _totalStart = 0;
        _preStart = 0;
        _preDone = 0;
        _searchStart = 0;
        _searchDone = 0;
        _solutionTime = (_preDone - _preStart) + (_searchDone - _searchStart);
        _status = 0;
        _nodes = 0;
        _deadends = 0;
    }
    LogBundle (long totalStart, long preStart, long preDone, long searchStart, long searchDone, int status, std::vector<std::pair<Position,char>> solution, int nodes, int deadends) {
        _totalStart = totalStart;
        _preStart = preStart;
        _preDone = preDone;
        _searchStart = searchStart;
        _searchDone = searchDone;
        _solutionTime = (_preDone - _preStart) + (_searchDone - _searchStart);
        _status = status;
        _solution = solution;
        _nodes = nodes;
        _deadends = deadends;
    }
};


class Logger {
private:
    LogBundle _log;
public:
    Logger();
    Logger (LogBundle log);
    void logInfo (char* filename);
    void printInfo ();
};

#endif // LOGGER_H
