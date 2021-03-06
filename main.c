#include "analyzer.h"
#include "node.h"
#include "output.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "yacc.tab.h"

#ifndef __MINGW64__
#ifdef _WIN32
	extern FILE *_popen();
	extern void _pclose(FILE*);
	#define popen _popen
	#define pclose _pclose
#endif
#endif


extern FILE *popen();
extern void _pclose(FILE*);
extern int yyparse(node *root, char *filename);
extern FILE *yyin;

int main(void) {
	char *filename = "current.acc";
	FILE *input = fopen(filename, "r");
	yyin = input;
	node root;
	root.type = ROOT;
	root.data.root.ext_list = new_list_node(10, (origin){filename, 0});
	root.data.root.struct_list = new_list_node(10, (origin){filename, 0});
	root.data.root.func_list = new_list_node(10, (origin){filename, 0});
	root.data.root.main_list = new_list_node(10, (origin){filename, 0});
	yyparse(&root, filename);
	fclose(input);
	char *cflags = "";
	int cflags_capacity = 0;
	c_ast_node result = analyze(root.data.root, &cflags, &cflags_capacity);
	char *command = "gcc -fno-builtin -o build/output.out -xc -";
	char *resulting_command = malloc(strlen(command) + strlen(cflags) + 1);
	resulting_command[0] = '\0';
	strcat(resulting_command, command);
	strcat(resulting_command, cflags);
	printf("%s\n", resulting_command);
	FILE *out = popen(resulting_command, "w");
	c_write(out, &result);
	c_write(stdout, &result);
	pclose(out);
	return 0;
}
