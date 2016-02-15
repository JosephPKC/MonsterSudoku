#include "bookkeeper.h"

BookKeeper::BookKeeper() {
    _recordLog = std::vector<std::pair<int,Variable>> ();
}

void BookKeeper::add (int level, Variable variable) {
    std::pair<int,Variable> P (level,variable);
    _recordLog.insert (_recordLog.end (),P);
}

bool BookKeeper::remove (std::size_t index) {
    if (index >= _recordLog.size()) {
        return false;
    }
    std::vector<std::pair<int,Variable>>::iterator it = _recordLog.begin();
    for (std::size_t i = 0; i <= index; ++i) {
        ++it;
    }
    _recordLog.erase (it);
    return true;
}

std::vector<Variable> BookKeeper::undo () {
//    cout << "In UNDO" << endl;
//    cout << "Record is of size=" << _recordLog.size () << endl;
//    cout << "Grabbing last level=";
    int lastLevel = _recordLog[_recordLog.size () - 1].first;
//    cout << lastLevel << endl;
    std::vector<Variable> lastChanges;
    while (_recordLog.size() > 0 && _recordLog[_recordLog.size () - 1].first == lastLevel) {
//        cout << "Size of Record Log=" << _recordLog.size () << endl;
//        cout << "Last entry of Record Log's level=" << _recordLog[_recordLog.size() - 1].first << endl;
        lastChanges.insert (lastChanges.end (), _recordLog[_recordLog.size () - 1].second);
//        _recordLog.erase (_recordLog.end ());
        _recordLog.pop_back();
    }
    return lastChanges;
}
