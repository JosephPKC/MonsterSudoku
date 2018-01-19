TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    sudokugenerator.cpp \
    sudokureader.cpp \
    sudokusolver.cpp \
    sudokupuzzle.cpp \
    alphabet.cpp \
    bookkeeper.cpp \
    logger.cpp

HEADERS += \
    sudokugenerator.h \
    sudokureader.h \
    sudokusolver.h \
    sudokupuzzle.h \
    alphabet.h \
    bookkeeper.h \
    logger.h
