# Makefile
# Date: -1 Dec 1440
CXX=g++
CXXFLAGS=-ggdb -Wall
YACC=bison
YFLAGS=--report=state -W -d
LEX=flex++
LEXFLAGS=--warn

.PHONY: clean test

program6: program6.cpp program6_lex.cpp program6.tab.c program6.tab.h node.h node.cpp tables.h tables.cpp
	${CXX} ${CXXFLAGS} program6.cpp program6.tab.c program6_lex.cpp node.cpp tables.cpp -o program6

program6.tab.c: program6.y node.h
	${YACC} ${YFLAGS} program6.y

program6_lex.cpp: program6.lpp node.h
	${LEX} ${LEXFLAGS} program6.lpp

test: testrig.cpp tables.h tables.cpp
	${CXX} ${CXXFLAGS} testrig.cpp tables.cpp -o test
	./test

clean:
	/bin/rm -f program6 program6_lex.cpp program6.tab.c program6.tab.h program6.output test
	

