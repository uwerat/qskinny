CONFIG += testcase qskinny
TARGET = tst_skinhints
SOURCES += SkinHints.cpp
QT += testlib

INCLUDEPATH *= $${QSK_DIRS}
