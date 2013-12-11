#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "errors.h"

typedef enum StdType
{
	NONE,
	PGNAME,
	PGPARAM,
	INT,
	REAL,
	AINT,
	AREAL,
	BOOL,
	ERR
} StdType;

typedef struct Type
{
	StdType std_type;
	int start;
	int end;
	int fun;
} Type;

typedef struct Symbol
{
	char *name;
	Type type;
	int count;
	int param;
} Symbol;

typedef struct SymbolTable
{
	Symbol *symbol;
	struct SymbolTable *next;
	struct SymbolTable *prev;
	struct SymbolTable *parent;
	struct SymbolTable *child;
	struct SymbolTable *temp;
} SymbolTable;

typedef struct Attributes
{
	Type t;
	int c;
	Type in;
	char *idptr;
} Attributes;

#define TYPE_DEFAULT (Type){NONE,0,0,0}
#define ATTRIBUTES_DEFAULT (Attributes){TYPE_DEFAULT,0,TYPE_DEFAULT,""}

struct ParserData;

Symbol *check_enter_method(char *name, struct ParserData *parser_data);
int check_exit_method(struct ParserData *parser_data);
Symbol *check_add_prog_param(char *name, struct ParserData *parser_data);
Symbol *check_add_fun_param(char *name, Attributes attrs, struct ParserData *parser_data);
Symbol *check_add_var(char *name, Attributes attrs, struct ParserData *parser_data);
void set_method_type(Type type, struct ParserData *parser_data);
void set_method_param_count(int count, struct ParserData *parser_data);

SymbolTable *get_symbol(char *name, struct ParserData *parser_data, int global_scope);
Type get_type(char *name, struct ParserData *parser_data);
int get_num_params(char *name, struct ParserData *parser_data);
Type get_param_type(char *name, int n, struct ParserData *parser_data);

int type_size(Type t);
int types_equal(Type a, Type b, int check_fun_equal);
char *type_to_str(Type type);
char *symbol_type_to_str(Symbol *symbol);
void fprint_symbol_table(FILE *f, SymbolTable *symbol_table);

#endif