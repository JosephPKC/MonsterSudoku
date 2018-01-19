#ifndef BOOKKEEPER_H
#define BOOKKEEPER_H
#include "sudokupuzzle.h"
class BookKeeper {
private:
    std::vector<std::pair<int,Variable>> _recordLog;
public:
    BookKeeper();

    void add (int level, Variable variable);
    bool remove(std::size_t index);
    std::vector<Variable> undo ();
};

#endif // BOOKKEEPER_H
