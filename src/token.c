#include "token.h"
#include "machine.h"

void chomp(char *s) {
    while(*s && *s != '\n' && *s != '\r') s++;
 
    *s = 0;
}

char *get_next_line(FILE *source)
{
	char *line = (char *)malloc(MAX_LINE_LENGTH_1);

	if (feof(source))
	{
		line[0] = EOF;
		line[1] = '\0';
	}
	else
	{
		fgets (line, MAX_LINE_LENGTH, source);

		// remove \n character
		chomp(line);
	}
	
	return line;
}

MachineResult *get_next_token(ParserData *parser_data, int options)
{
	// remember where we were last by saving source file pointer
	static FILE *s;

	// save current line
	static char l[MAX_LINE_LENGTH_1];
	static char *f;
	static int i = 0;

	// options for things like NOP
	static int o = 0;
	static MachineResult *r;

	if (o & TOKEN_OPTION_NOP && r != NULL) {
		 o = options;
		return r;
	}

	// token file header
	if (s == NULL && parser_data->tokens != NULL)
		fprintf (parser_data->tokens, "%-10s%-20s%-20s%s\n", "Line No.", "Lexeme", "TOKEN-TYPE", "ATTRIBUTE");

	// grab another line
	if (s != parser_data->source || f - l > MAX_LINE_LENGTH || *f == 0) {
		s = parser_data->source;

		char *line = get_next_line(parser_data->source);

		strcpy(l, line);
		f = l;
		i++;

		// output line to listing file
		if (parser_data->listing != NULL)
			fprintf (parser_data->listing, "%-8d%s\n", i, line);

		// skip blank lines
		if (strcmp(l, "") == 0)
			return get_next_token(parser_data, options);
	}

	MachineResult result = machine_omega(f, parser_data->reserved_words);

	// do not increment line counter for eof
	if (result.token->type == TOKEN_EOF) i--;

	result.line_no = i;

	MachineResult *resultPtr = (MachineResult *)malloc(sizeof(MachineResult));
	memcpy(resultPtr, &result, sizeof(MachineResult));

	// advance our internal pointer
	f = result.f;

	if (resultPtr->token->type == TOKEN_WHITESPACE)
		return get_next_token(parser_data, options);

	// write token to tokens file
	if (parser_data->tokens != NULL) {
		fprintf (parser_data->tokens, "%-10d%-20s%-20s%-6d(%s)\n", i, resultPtr->lexeme, token_type_to_str(resultPtr->token->type), resultPtr->token->attribute, attribute_to_str(resultPtr->token->attribute));
	}

	// handle lexical errors
	if (resultPtr->token->type == TOKEN_LEXERR) {
		// output errors to listing file
		if (!(options & TOKEN_OPTION_SQUASH_ERRS))
			lexerr(resultPtr, parser_data);

		// do not return lexerr tokens
		return get_next_token(parser_data, options);
	}

	// check for a nop
	if (options & TOKEN_OPTION_NOP)
		r = resultPtr;

	o = options;

	return resultPtr;
}

ReservedWord *tokenize_reserved_word_str (char *in)
{
	ReservedWord *word = (ReservedWord *)malloc(sizeof(ReservedWord));

	char line[200];
	strcpy(line, in);

    // tokenize the line by tabs
    char pieces[3][MAX_LINE_LENGTH];
    char *piece;
    int i = -1;

	piece = strtok (line, RESERVED_WORD_DELIM);
	while (piece != NULL && i++ < 3)
	{
		strcpy (pieces[i], piece);
	    piece = strtok (NULL, RESERVED_WORD_DELIM);
	}

	word->name = malloc(MAX_LINE_LENGTH);
	strcpy(word->name, pieces[0]);

	word->token = (Token *)malloc(sizeof(Token));
	word->token->type = int_to_token_type(strtol(pieces[1], (char **)NULL, 10));
	word->token->attribute = strtol(pieces[2], (char **)NULL, 10);

	word->next = NULL;

	return word;
}

TokenType int_to_token_type (int id)
{
	if (id == 1000) return TOKEN_PROGRAM;
	else if (id == 1001) return TOKEN_VAR;
	else if (id == 1002) return TOKEN_ARRAY;
	else if (id == 1003) return TOKEN_OF;
	else if (id == 1004) return TOKEN_INTEGER;
	else if (id == 1005) return TOKEN_REAL;
	else if (id == 1006) return TOKEN_PROCEDURE;
	else if (id == 1007) return TOKEN_FUNCTION;
	else if (id == 1008) return TOKEN_BEGIN;
	else if (id == 1009) return TOKEN_END;
	else if (id == 1010) return TOKEN_IF;
	else if (id == 1011) return TOKEN_THEN;
	else if (id == 1012) return TOKEN_ELSE;
	else if (id == 1013) return TOKEN_WHILE;
	else if (id == 1014) return TOKEN_DO;
	else if (id == 1015) return TOKEN_NOT;
	else if (id == 1016) return TOKEN_ADDOP; // or
	else if (id == 1017) return TOKEN_MULOP; // div
	else if (id == 1018) return TOKEN_MULOP; // mod
	else if (id == 1019) return TOKEN_MULOP; // and
	
	return ERR_TOKEN_NOT_FOUND;
}

int token_type_to_int (TokenType type)
{
	if (type == TOKEN_PROGRAM) return 1000;
	else if (type == TOKEN_VAR) return 1001;
	else if (type == TOKEN_ARRAY) return  1002;
	else if (type == TOKEN_OF) return  1003;
	else if (type == TOKEN_INTEGER) return  1004;
	else if (type == TOKEN_REAL) return  1005;
	else if (type == TOKEN_PROCEDURE) return  1006;
	else if (type == TOKEN_FUNCTION) return  1007;
	else if (type == TOKEN_BEGIN) return  1008;
	else if (type == TOKEN_END) return  1009;
	else if (type == TOKEN_IF) return  1010;
	else if (type == TOKEN_THEN) return  1011;
	else if (type == TOKEN_ELSE) return 1012;
	else if (type == TOKEN_WHILE) return 1013;
	else if (type == TOKEN_DO) return 1014;
	else if (type == TOKEN_NOT) return 1015;
	else if (type == TOKEN_ADDOP) return 1016;
	else if (type == TOKEN_MULOP) return 1018;
	else return 0;
}

char *token_type_to_str (TokenType type)
{
	if (type == TOKEN_WHITESPACE) return "WHITESPACE";
	else if (type == TOKEN_ID) return "ID";
	else if (type == TOKEN_PROGRAM) return "PROGRAM";
	else if (type == TOKEN_VAR) return "VAR";
	else if (type == TOKEN_ARRAY) return "ARRAY";
	else if (type == TOKEN_OF) return "OF";
	else if (type == TOKEN_INTEGER) return "INT";
	else if (type == TOKEN_REAL) return "REAL";
	else if (type == TOKEN_PROCEDURE) return "PROCEDURE";
	else if (type == TOKEN_FUNCTION) return "FUNCTION";
	else if (type == TOKEN_BEGIN) return "BEGIN";
	else if (type == TOKEN_END) return "END";
	else if (type == TOKEN_IF) return "IF";
	else if (type == TOKEN_THEN) return "THEN";
	else if (type == TOKEN_ELSE) return "ELSE";
	else if (type == TOKEN_WHILE) return "WHILE";
	else if (type == TOKEN_DO) return "DO";
	else if (type == TOKEN_NOT) return "NOT";
	else if (type == TOKEN_NUM) return "NUM";
	else if (type == TOKEN_RELOP) return "RELOP";
	else if (type == TOKEN_MULOP) return "MULOP";
	else if (type == TOKEN_ADDOP) return "ADDOP";
	else if (type == TOKEN_ASSIGNOP) return "ASSIGNOP";
	else if (type == TOKEN_SEMICOLON) return "SEMICOLON";
	else if (type == TOKEN_COLON) return "COLON";
	else if (type == TOKEN_COMMA) return "COMMA";
	else if (type == TOKEN_PERIOD) return "PERIOD";
	else if (type == TOKEN_LPAREN) return "LPAREN";
	else if (type == TOKEN_RPAREN) return "RPAREN";
	else if (type == TOKEN_LBRACKET) return "LBRACKET";
	else if (type == TOKEN_RBRACKET) return "RBRACKET";
	else if (type == TOKEN_EOF) return "EOF";
	else if (type == TOKEN_LEXERR) return "LEXERR";

	return "NULL";
}

char *attribute_to_str (int attr)
{
	if (attr == 1) return "Unrecognized Symbol";
	else if (attr == 2) return "Extra Long ID";
	else if (attr == 3 || attr == 4) return "Extra Long Integer";
	else if (attr == 5) return "Extra Long Fractional Part";
	else if (attr == 6) return "Extra Long Exponent Part";
	else if (attr == 7) return "Integer Leading Zero";
	else if (attr == 990) return "MOD";
	else if (attr == 991) return "DIV";
	else if (attr == 992) return "OR";
	else if (attr == 993) return "AND";
	else if (attr == 994) return "<>";
	else if (attr == 995) return "<=";
	else if (attr == 996) return ">=";
	else if (attr == 997) return "LONGREAL";
	else if (attr == 998) return "REAL";
	else if (attr == 999) return "INT";
	else if (attr >= SYM_TABLE_START_ADDR)
	{
		char *str = (char *)malloc(20);
		snprintf(str, 20, "loc%d in SYMTABLE", attr - SYM_TABLE_START_ADDR);
		return str;
	}

	char *str = (char *)malloc(1);
	snprintf(str, 1, "%d", attr);
	return str;
}