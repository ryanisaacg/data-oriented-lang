C_FLAGS=-Wall -Werror -Wextra -Wfatal-errors -pedantic -std=c99 -Wno-unused-function
L_FLAGS=
run: build/parser.out
	build/./parser.out

build/parser.out: build/parser.yy.c
	gcc $(C_FLAGS) build/parser.yy.c -o build/parser.out

build/parser.yy.c: build parser.l
	flex -obuild/parser.yy.c parser.l
	
build:
	mkdir build
