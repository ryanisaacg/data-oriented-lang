#pragma once

#include "node.h"
#include "output.h"

c_ast_node analyze(rootnode root, char **cflags, int *cflag_capacity);
