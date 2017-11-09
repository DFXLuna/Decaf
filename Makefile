# Makefile
# Date: -1 Dec 1440
CXX=g++
CXXFLAGS=-ggdb -Wall
YACC=bison
YFLAGS=--report=state -W -d
LEX=flex++
LEXFLAGS=--warn

.PHONY: clean

program5: program5.cpp program5_lex.cpp program5.tab.c program5.tab.h node.h node.cpp
	${CXX} ${CXXFLAGS} program5.cpp program5.tab.c program5_lex.cpp node.cpp -o program5

program5.tab.c: program5.y node.h
	${YACC} ${YFLAGS} program5.y

program5_lex.cpp: program5.lpp node.h
	${LEX} ${LEXFLAGS} program5.lpp

clean:
	/bin/rm -f program5 program5_lex.cpp program5.tab.c program5.tab.h program5.output
	

