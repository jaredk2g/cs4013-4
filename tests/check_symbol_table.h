#include <stdlib.h>
#include <check.h>
#include "symbol_table.h"
#include "parser.h"
#include <stdio.h>

#ifndef CHECK_SYMBOL_TABLE_H
#define CHECK_SYMBOL_TABLE_H

struct ParserData *parser_data;

Suite * symbol_table_suite (void);

#endif