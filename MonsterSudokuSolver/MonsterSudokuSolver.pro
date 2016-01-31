TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    sudokugenerator.cpp \
    sudokureader.cpp \
    sudokusolver.cpp

HEADERS += \
    sudokugenerator.h \
    sudokureader.h \
    sudokusolver.h
