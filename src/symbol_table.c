#include "symbol_table.h"

SymbolTable *new_symbol_table_entry(char *name, Attributes attrs)
{
	SymbolTable *new = (SymbolTable *)malloc(sizeof(SymbolTable));
	new->parent = NULL;
	new->child = NULL;
	new->prev = NULL;
	new->next = NULL;
	new->temp = NULL;

	new->symbol = (Symbol *)malloc(sizeof(Symbol));
	new->symbol->type = attrs.t;
	new->symbol->param = 0;
	new->symbol->count = 0;

	new->symbol->name = (char *)malloc(sizeof(name));
	strcpy(new->symbol->name, name);

	return new;
}

Symbol *check_enter_method(char *name, struct ParserData *parser_data)
{
	if (get_symbol(name, parser_data, 1) != NULL)
		semerr("Duplicate method names.", 0, parser_data);

	SymbolTable *new;
	Attributes attrs = ATTRIBUTES_DEFAULT;

	// program name if first entry to symbol table
	if (parser_data->sym_eye == NULL)
		attrs.t.std_type = PGNAME;

	new = new_symbol_table_entry(name, attrs);

	// otherwise function name
	if (parser_data->sym_eye != NULL)
		new->symbol->type.fun = 1;

	// new symbol table
	if (parser_data->symbol_table == NULL) {
		parser_data->symbol_table = new;
	// add entry
	} else if (parser_data->sym_eye != NULL) {
		if (parser_data->sym_turn) {
			parser_data->sym_eye->child = new;
			new->parent = parser_data->sym_eye;
		} else {
			parser_data->sym_eye->next = new;
			new->prev = parser_data->sym_eye;
			new->parent = parser_data->sym_eye->parent;
		}
	}

	parser_data->sym_eye = new;
	parser_data->sym_turn = 1;

	return new->symbol;
}

int check_exit_method(struct ParserData *parser_data)
{
	if (parser_data->sym_eye == NULL) {
		semerr("Cannot exit method before entering.", 0, parser_data);
		return 1;
	}

	// handles the case where we entered a function with no children
	// Problem: upon exit moves eye to the function's parent prematurely
	//          instead of to the function itself
	// Solution: perform a NOP for 1 exit cycle to avoid skipping over function
	if (parser_data->sym_eye->symbol->type.fun == 1 && parser_data->sym_eye->child == NULL) {
	//if () {
		// now we can move on with our lives..
		if (parser_data->sym_eye->temp != NULL)
			parser_data->sym_eye = parser_data->sym_eye->temp;
		// NOP
		else
			parser_data->sym_eye->temp = parser_data->sym_eye->parent;
	} else
		parser_data->sym_eye = parser_data->sym_eye->parent;

	parser_data->sym_turn = 0;

	return 0;
}

Symbol *check_add_prog_param(char *name, struct ParserData *parser_data)
{
	return check_add_var(name, (Attributes){{PGPARAM,0,0,0},0,TYPE_DEFAULT,""}, parser_data);
}

Symbol *check_add_fun_param(char *name, Attributes attrs, struct ParserData *parser_data)
{
	Symbol *symbol = check_add_var(name, attrs, parser_data);

	symbol->param = 1;

	return symbol;
}

Symbol *check_add_var(char *name, Attributes attrs, struct ParserData *parser_data)
{
	if (parser_data->sym_eye == NULL) {
		semerr("Cannot add variable before entering procedure.", 0, parser_data);
		return NULL;
	}
		
	if (get_symbol(name, parser_data, 0) != NULL)
		semerr("Duplicate variable names.", 0, parser_data);

	SymbolTable *new = new_symbol_table_entry(name, attrs);

	// perform a turn
	if (parser_data->sym_turn) {
		parser_data->sym_eye->child = new;
		new->parent = parser_data->sym_eye;
	// add as sibling
	} else {
		parser_data->sym_eye->next = new;
		new->prev = parser_data->sym_eye;
		new->parent = parser_data->sym_eye->parent;
	}

	parser_data->sym_eye = new;
	parser_data->sym_turn = 0;

	return new->symbol;
}

void set_method_type(Type type, struct ParserData *parser_data)
{
	type.fun = 1;

	// handle case where eye is looking at function
	if (parser_data->sym_eye->symbol->type.fun == 1)
		parser_data->sym_eye->symbol->type = type;
	else if (parser_data->sym_eye->parent != NULL)
		parser_data->sym_eye->parent->symbol->type = type;
}

void set_method_param_count(int count, struct ParserData *parser_data)
{
	// handle case where eye is looking at function
	if (parser_data->sym_eye->symbol->type.fun == 1)
		parser_data->sym_eye->symbol->count = count;
	else if (parser_data->sym_eye->parent != NULL)
		parser_data->sym_eye->parent->symbol->count = count;
}

SymbolTable *get_symbol(char *name, struct ParserData *parser_data, int global_scope)
{
	if (parser_data->sym_eye == NULL)
		return NULL;

	// STOP conditions:
	// i) !global_scope ^ reached parent of green node
	// ii) global_scope ^ reached NULL
	// iii) always stop on NULL. always.

	SymbolTable *curr = parser_data->sym_eye;

	SymbolTable *stop_condition = NULL;

	if (global_scope == 0) {
		// Avoid jumping up to a new scope when doing a local search
		// on a function after a turn with no children
		if (curr->symbol->type.fun == 1 && curr->child == NULL)
			stop_condition = curr;
		else
			stop_condition = curr->parent;
	}

	// make sure starting at end of scope
	while (curr->next != NULL)
		curr = curr->next;

	while (curr != NULL && curr != stop_condition) {
		if (strcmp(curr->symbol->name, name) == 0)
			return curr;

		// go to prev until null, then start again at parent
		if (curr->prev != NULL)
			curr = curr->prev;
		else {
			curr = curr->parent;

			if (curr != stop_condition) {
				// make sure starting at end of scope
				while (curr->next != NULL)
					curr = curr->next;
			}
		}
	}

	return NULL;
}

Type get_type(char *name, struct ParserData *parser_data)
{
	SymbolTable *symbol_entry = get_symbol(name, parser_data, 1);

	if (symbol_entry != NULL)
		return symbol_entry->symbol->type;

	return TYPE_DEFAULT;
}

int get_num_params(char *name, struct ParserData *parser_data)
{
	SymbolTable *symbol_entry = get_symbol(name, parser_data, 1);

	if (symbol_entry != NULL)
		return symbol_entry->symbol->count;

	return 0;
}

Type get_param_type(char *name, int n, struct ParserData *parser_data)
{
	SymbolTable *symbol_entry = get_symbol(name, parser_data, 1);

	if (symbol_entry != NULL) {
		SymbolTable *curr = symbol_entry->child;

		// find the Nth child after function declaration (zero-indexed)
		// the assumption is made that parameters come immediately after method
		int i = 0;
		while (i++ < n && curr != NULL)
			curr = curr->next;

		if (curr != NULL && curr->symbol->param == 1)
			return curr->symbol->type;
	}

	return TYPE_DEFAULT;
}

int type_size(Type t)
{
	int unit_size = 0;

	if (t.std_type == INT || t.std_type == AINT) unit_size = 4;
	if (t.std_type == REAL || t.std_type == AREAL) unit_size = 8;

	int multiplier = (t.std_type == AINT || t.std_type == AREAL) ? (t.end - t.start + 1) : 1;

	return unit_size * multiplier;
}

int types_equal(Type a, Type b, int check_fun_equal)
{
	if (a.std_type == b.std_type) {
		if ((a.std_type == AINT || a.std_type == AREAL) && (a.start != b.start || a.end != b.end))
			return 0;

		if (check_fun_equal == 1 && a.fun != b.fun)
			return 0;

		return 1;
	}

	return 0;
}

char *type_to_str(Type type)
{
	char *str, *str2, *str3;
	switch (type.std_type)
	{
	case PGNAME: str2 = "pgm"; break;
	case PGPARAM: str2 = "param"; break;
	case INT: str2 = "int"; break;
	case REAL: str2 = "real"; break;
	case BOOL: str2 = "bool"; break;
	case AINT:
	case AREAL:
		str = (type.std_type == AINT) ? "int" : "real";
		str2 = (char *)malloc(100);
		sprintf(str2, "array [%d..%d] of %s", type.start, type.end, str);
	break;
	case ERR: return "err";
	case NONE: return "none";
	default: return "";
	}

	str3 = str2;
	if (type.fun == 1) {
		str3 = (char *)malloc(100);
		sprintf(str3, "%s, fun", str2);
	}

	return str3;
}

char *symbol_type_to_str(Symbol *symbol)
{
	char *str = type_to_str(symbol->type);
	char *str2 = str;

	if (symbol->type.fun == 1) {
		str2 = (char *)malloc(100);
		sprintf(str2, "%s, %d arg(s)", str, symbol->count);
	} if (symbol->param == 1) {
		str2 = (char *)malloc(100);
		sprintf(str2, "%s, fun param", str);
	}

	return str2;
}

void output_symbol_table(FILE *f, SymbolTable *symbol_table, int level)
{
	SymbolTable *s = symbol_table;
	int offset = 0;

	while (s != NULL && s->symbol != NULL)
	{
		// indent
		for (int l = 0; l < level; l++) fprintf (f, "  ");

		Type t = s->symbol->type;

		char *offset_str = (char *)malloc(100);
		if (s->symbol->param == 0 && t.fun == 0 && t.std_type != PGNAME && t.std_type != PGPARAM) {
			sprintf(offset_str, "offset %d", offset);

			// compute new offset
			offset += type_size(t);
		}

		fprintf (f, "%s (%s) %s\n", s->symbol->name, symbol_type_to_str(s->symbol), offset_str);

		// recurse children
		output_symbol_table(f, s->child, level + 1);

		s = s->next;
	}
}

void fprint_symbol_table(FILE *f, SymbolTable *symbol_table)
{
	output_symbol_table(f, symbol_table, 0);
}