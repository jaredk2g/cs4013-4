#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "symbol_table.h"
#include "token.h"
#include "errors.h"

typedef enum Production
{
	PRODUCTION_PROGRAM,
	PRODUCTION_PROGRAM_,
	PRODUCTION_PROGRAM__,
	PRODUCTION_ID_LIST,
	PRODUCTION_ID_LIST_,
	PRODUCTION_DECLARATIONS,
	PRODUCTION_DECLARATIONS_,
	PRODUCTION_TYPE,
	PRODUCTION_STD_TYPE,
	PRODUCTION_SUBPROGRAM_DECLARATIONS,
	PRODUCTION_SUBPROGRAM_DECLARATIONS_,
	PRODUCTION_SUBPROGRAM_DECLARATION,
	PRODUCTION_SUBPROGRAM_DECLARATION_,
	PRODUCTION_SUBPROGRAM_DECLARATION__,
	PRODUCTION_SUBPROGRAM_HEAD,
	PRODUCTION_SUBPROGRAM_HEAD_,
	PRODUCTION_ARGUMENTS,
	PRODUCTION_PARAM_LIST,
	PRODUCTION_PARAM_LIST_,
	PRODUCTION_COMPOUND_STATEMENT,
	PRODUCTION_COMPOUND_STATEMENT_,
	PRODUCTION_OPTIONAL_STATEMENTS,
	PRODUCTION_STATEMENT_LIST,
	PRODUCTION_STATEMENT_LIST_,
	PRODUCTION_STATEMENT,
	PRODUCTION_STATEMENT_,
	PRODUCTION_VAR,
	PRODUCTION_VAR_,
	PRODUCTION_EXPR_LIST,
	PRODUCTION_EXPR_LIST_,
	PRODUCTION_EXPR,
	PRODUCTION_EXPR_,
	PRODUCTION_SIMPLE_EXPR,
	PRODUCTION_SIMPLE_EXPR_,
	PRODUCTION_TERM,
	PRODUCTION_TERM_,
	PRODUCTION_FACTOR,
	PRODUCTION_FACTOR_,
	PRODUCTION_SIGN
} Production;

struct MachineResult;
struct ReservedWord;
enum TokenType;
struct SymbolTable;
struct Attributes;

typedef struct ParserData
{
	FILE *listing;
	FILE *source;
	FILE *tokens;
	FILE *symbols;
	struct ReservedWord *reserved_words;
	struct SymbolTable *symbol_table;
	int result;
	struct SymbolTable *sym_eye;
	int sym_turn;
} ParserData;

#define PARSER_RESULT_OK 0
#define PARSER_RESULT_LEXERR 1
#define PARSER_RESULT_SYNERR 2
#define PARSER_RESULT_SEMERR 4

void parse(ParserData *parser_data);
struct MachineResult *match(enum TokenType t, ParserData *parser_data);

void parse_program(ParserData *parser_data);
void parse_program_(ParserData *parser_data);
void parse_program__(ParserData *parser_data);
void parse_id_list(ParserData *parser_data);
void parse_id_list_(ParserData *parser_data);
void parse_declarations(ParserData *parser_data);
void parse_declarations_(ParserData *parser_data);
struct Attributes parse_type(ParserData *parser_data);
struct Attributes parse_std_type(ParserData *parser_data);
void parse_subprogram_declarations(ParserData *parser_data);
void parse_subprogram_declarations_(ParserData *parser_data);
void parse_subprogram_declaration(ParserData *parser_data);
void parse_subprogram_declaration_(ParserData *parser_data);
void parse_subprogram_declaration__(ParserData *parser_data);
void parse_subprogram_head(ParserData *parser_data);
struct Attributes parse_subprogram_head_(ParserData *parser_data);
struct Attributes parse_arguments(ParserData *parser_data);
struct Attributes parse_param_list(ParserData *parser_data);
void parse_param_list_(ParserData *parser_data, struct Attributes *pl_);
void parse_compound_statement(ParserData *parser_data);
void parse_compound_statement_(ParserData *parser_data);
void parse_optional_statements(ParserData *parser_data);
void parse_statement_list(ParserData *parser_data);
void parse_statement_list_(ParserData *parser_data);
struct Attributes parse_statement(ParserData *parser_data);
void parse_statement_(ParserData *parser_data);
struct Attributes parse_var(ParserData *parser_data);
void parse_var_(ParserData *parser_data, struct Attributes *v_);
void parse_expr_list(ParserData *parser_data, struct Attributes *el);
void parse_expr_list_(ParserData *parser_data, struct Attributes *el_);
struct Attributes parse_expr(ParserData *parser_data);
void parse_expr_(ParserData *parser_data, struct Attributes *e_);
struct Attributes parse_simple_expr(ParserData *parser_data);
void parse_simple_expr_(ParserData *parser_data, struct Attributes *se_);
struct Attributes parse_term(ParserData *parser_data);
void parse_term_(ParserData *parser_data, struct Attributes *t_);
struct Attributes parse_factor(ParserData *parser_data);
void parse_factor_(ParserData *parser_data, struct Attributes *f_);
void parse_sign(ParserData *parser_data);

void synch(Production prod, struct MachineResult *tok, ParserData *parser_data);

#endif