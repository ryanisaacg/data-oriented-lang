C_FLAGS=-Wall -Werror -Wextra -Wfatal-errors -pedantic -std=c99 -Wno-unused-function -Ibuild
L_FLAGS=
all: build/parser.out

clean:
	rm -r build
	rm lex.c yacc.tab.c

build/parser.out: build build/yacc.o build/lex.o build/node.o build/printing.o build/table.o
	gcc build/yacc.o build/lex.o build/node.o build/printing.o build/table.o -o build/parser.out

build/table.o: table.h table.c
	gcc $(C_FLAGS) table.c -c -o build/table.o
	
build/printing.o: printing.h printing.c build/node.o
	gcc $(C_FLAGS) printing.c -c -o build/printing.o

build/node.o: node.c node.h 
	gcc $(C_FLAGS) node.c -c -o build/node.o

build/yacc.o: yacc.tab.c
	gcc -I. -Wno-implicit-function-declaration yacc.tab.c -c -o build/yacc.o

build/lex.o: yacc.tab.c lex.c
	gcc lex.c -c -o build/lex.o

lex.c: yacc.tab.h parser.l
	flex -olex.c parser.l
	
yacc.tab.c: parser.y
	bison -d -byacc parser.y -v

build:
	mkdir build
