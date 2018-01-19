TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    cell.cpp \
    generator.cpp \
    puzzle.cpp \
    utility.cpp \
    recorder.cpp \
    solver.cpp

HEADERS += \
    cell.h \
    generator.h \
    puzzle.h \
    utility.h \
    recorder.h \
    solver.h

DISTFILES += \
    Test.java
