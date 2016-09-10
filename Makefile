C_FLAGS=-Wall -Werror -Wextra -Wfatal-errors -pedantic -std=c99 -Wno-unused-function -Ibuild
L_FLAGS=
run: build/parser.out
	build/./parser.out

build/parser.out: build/yacc.o build/lex.o
	gcc build/yacc.o build/lex.o -o build/parser.out

build/yacc.o: build/yacc.tab.c
	gcc -Wno-implicit-function-declaration build/yacc.tab.c -c -o build/yacc.o

build/lex.o: build/yacc.tab.c build/lex.c
	gcc build/lex.c -c -o build/lex.o

build/lex.c: build build/yacc.tab.h parser.l
	flex -obuild/lex.c parser.l
	
build/yacc.tab.c: build parser.y
	bison -d -bbuild/yacc parser.y

build:
	mkdir build
