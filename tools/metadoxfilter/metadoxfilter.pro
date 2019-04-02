TEMPLATE     = app

CONFIG -= qt

DESTDIR      = $${QSK_OUT_ROOT}/tools/bin
PARSERDIR    = parser

TARGET = metadoxfilter

FLEXSOURCES += metadox.l
BISONSOURCES += metadox.y

HEADERS += \
    Interface.h \
    DoxDumper.h \
    ParserTree.h

SOURCES += \
    ParserTree.cpp \
    DoxDumper.cpp \
    main.cpp

#FLEXFLAGS = -d
#BISONFLAGS = -t

LIBS += -lfl -ly

INCLUDEPATH += $${PARSERDIR}
QMAKE_CFLAGS += -Wno-implicit-function-declaration -Wno-sign-compare -Wno-unused-function

flexc.name = Lexer generator
flexc.input = FLEXSOURCES
flexc.output = $${PARSERDIR}/${QMAKE_FILE_BASE}.lex.c
flexc.variable_out = SOURCES
flexc.commands = mkdir -p $${PARSERDIR};
flexc.commands += flex $${FLEXFLAGS} -o $${flexc.output} ${QMAKE_FILE_IN}
flexc.CONFIG += target_predeps no_link

flexh.name = Lexer Header generator
flexh.input = FLEXSOURCES
flexh.output = $${PARSERDIR}/${QMAKE_FILE_BASE}.lex.h
flexh.variable_out = HEADERS
flexh.commands = mkdir -p $${PARSERDIR};
flexh.commands += flex $${FLEXFLAGS} --header-file=$${flexh.output} -o /dev/null ${QMAKE_FILE_IN}
flexh.CONFIG += target_predeps no_link

bisonc.name = Parser generator
bisonc.input = BISONSOURCES
bisonc.output = $${PARSERDIR}/${QMAKE_FILE_BASE}.tab.c
bisonc.commands = mkdir -p $${PARSERDIR};
bisonc.commands += bison $${BISONFLAGS} -o $${bisonc.output} ${QMAKE_FILE_IN}
bisonc.variable_out = SOURCES
bisonc.CONFIG += target_predeps no_link

bisonh.name = Parser generator
bisonh.input = BISONSOURCES
bisonh.output = $${PARSERDIR}/${QMAKE_FILE_BASE}.tab.h
bisonh.commands = mkdir -p $${PARSERDIR};
bisonh.commands += bison $${BISONFLAGS} --defines=$${bisonh.output} -o /dev/null ${QMAKE_FILE_IN}
bisonh.variable_out = HEADERS
bisonh.CONFIG += target_predeps no_link

QMAKE_EXTRA_COMPILERS += flexh bisonh flexc bisonc

