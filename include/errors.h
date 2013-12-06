#ifndef ERRORS_H
#define ERRORS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "machine.h"
#include "token.h"
#include "parser.h"

struct MachineResult;
struct ParserData;
enum TokenType;

void lexerr(struct MachineResult *result, struct ParserData *parser_data);
void synerr(enum TokenType *expected, int len, struct MachineResult *found, struct ParserData *parser_data);
void semerr(char *err, int line, struct ParserData *parser_data);

#endif