#include "check_symbol_table.h"

START_TEST (test_check_enter_method)
{
	Symbol *result;

	result = check_enter_method("hey", parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("hey", result->name);
	ck_assert(result->type.std_type == PGNAME);

	result = check_enter_method("test", parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("test", result->name);
	ck_assert_int_eq(1, result->fun);
	ck_assert(result->type.std_type == NONE);
}
END_TEST

START_TEST (test_check_exit_method)
{
	ck_assert(check_enter_method("program", parser_data) != NULL);

	ck_assert(check_enter_method("checkenter1", parser_data) != NULL);

	ck_assert_int_eq(0, check_exit_method(parser_data));

	ck_assert(check_enter_method("checkenter2", parser_data) != NULL);

	ck_assert(check_enter_method("checkenter3", parser_data) != NULL);

	ck_assert_int_eq(0, check_exit_method(parser_data));

	ck_assert_int_eq(0, check_exit_method(parser_data));
}
END_TEST

START_TEST (test_check_add_var)
{
	ck_assert(check_enter_method("checkaddvar", parser_data) != NULL);

	Symbol *result;

	Attributes attrs = ATTRIBUTES_DEFAULT;

	attrs.t.std_type = INT;
	result = check_add_var("int", attrs, parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("int", result->name);

	attrs.t.std_type = REAL;
	result = check_add_var("real", attrs, parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("real", result->name);

	ck_assert(check_exit_method(parser_data) == 0);
}
END_TEST

START_TEST (test_check_add_prog_param)
{
	ck_assert(check_enter_method("checkaddparam", parser_data) != NULL);

	Symbol *result;

	result = check_add_prog_param("param", parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("param", result->name);

	ck_assert(check_exit_method(parser_data) == 0);
}
END_TEST

START_TEST (test_check_add_fun_param)
{
	ck_assert(check_enter_method("checkaddfparam", parser_data) != NULL);

	Symbol *result;

	Attributes attrs = ATTRIBUTES_DEFAULT;
	attrs.t.std_type = INT;
	result = check_add_fun_param("fparam", attrs, parser_data);
	ck_assert(result != NULL);
	ck_assert_str_eq("fparam", result->name);
	ck_assert_int_eq(1, result->param);

	ck_assert(check_exit_method(parser_data) == 0);
}
END_TEST

START_TEST (test_get_symbol)
{
	SymbolTable *result = get_symbol("notfound", parser_data, 1);
	ck_assert(result == NULL);

	// program
	ck_assert(check_enter_method("program", parser_data) != NULL);

	result = get_symbol("program", parser_data, 1);
	ck_assert_str_eq("program", result->symbol->name);
	ck_assert(result->symbol->type.std_type == PGNAME);

	// program.pvar
	Attributes attrs = ATTRIBUTES_DEFAULT;
	attrs.t.std_type = REAL;
	ck_assert(check_add_var("pvar", attrs, parser_data) != NULL);	

	// program.fun
	ck_assert(check_enter_method("fun", parser_data) != NULL);

	result = get_symbol("fun", parser_data, 1);
	ck_assert_str_eq("fun", result->symbol->name);
	ck_assert_int_eq(1, result->symbol->fun);
	ck_assert(result->symbol->type.std_type == NONE);

	// check global search
	ck_assert(get_symbol("pvar", parser_data, 0) == NULL);

	result = get_symbol("pvar", parser_data, 1);
	ck_assert_str_eq("pvar", result->symbol->name);
	ck_assert(result->symbol->type.std_type == REAL);

	// exit program.fun
	ck_assert(check_exit_method(parser_data) == 0);

	// exit program
	ck_assert(check_exit_method(parser_data) == 0);
}
END_TEST

START_TEST (test_set_method_type)
{
	ck_assert(check_enter_method("program", parser_data) != NULL);

	set_method_type((Type){INT,0,0}, parser_data);
	ck_assert(get_type("program", parser_data).std_type == PGNAME);

	ck_assert(check_enter_method("fun", parser_data) != NULL);

	set_method_type((Type){INT,0,0}, parser_data);
	SymbolTable *fun = get_symbol("fun", parser_data, 1);
	ck_assert(fun != NULL);
	ck_assert(fun->symbol->type.std_type == INT);
	ck_assert(fun->symbol->fun == 1);
}
END_TEST

START_TEST (test_set_method_param_count)
{
	ck_assert(check_enter_method("program", parser_data) != NULL);

	ck_assert(check_enter_method("fun", parser_data) != NULL);

	set_method_param_count(123, parser_data);

	SymbolTable *fun = get_symbol("fun", parser_data, 1);
	ck_assert(fun != NULL);
	ck_assert_int_eq(123, fun->symbol->count);
}
END_TEST

START_TEST (test_get_type)
{
	ck_assert(check_enter_method("program", parser_data) != NULL);

	ck_assert(check_enter_method("fun", parser_data) != NULL);

	set_method_type((Type){INT,0,0}, parser_data);

	ck_assert(get_type("fun", parser_data).std_type == INT);
}
END_TEST

START_TEST (test_get_num_params)
{
	ck_assert(check_enter_method("program", parser_data) != NULL);

	ck_assert(check_enter_method("fun", parser_data) != NULL);

	set_method_param_count(100, parser_data);

	ck_assert_int_eq(100, get_num_params("fun", parser_data));
}
END_TEST

START_TEST (test_get_param_type)
{
	ck_assert(get_param_type("fun", 2, parser_data).std_type == NONE);

	ck_assert(check_enter_method("program", parser_data) != NULL);

	ck_assert(check_enter_method("fun", parser_data) != NULL);

	Attributes attrs = ATTRIBUTES_DEFAULT;
	attrs.t.std_type = INT;
	ck_assert(check_add_fun_param("x", attrs, parser_data) != NULL);
	attrs.t.std_type = REAL;
	ck_assert(check_add_fun_param("y", attrs, parser_data) != NULL);

	ck_assert(get_param_type("fun", 0, parser_data).std_type == INT);
	ck_assert(get_param_type("fun", 1, parser_data).std_type == REAL);
}
END_TEST

START_TEST (test_types_equal)
{
	Type a, b;

	a = (Type){BOOL,0,0}; 
	b = (Type){BOOL,0,0};
	ck_assert_int_eq(1, types_equal(a,b));

	a = (Type){INT,0,0}; 
	b = (Type){REAL,0,0};
	ck_assert_int_eq(0, types_equal(a,b));

	a = (Type){AINT,0,30}; 
	b = (Type){AINT,0,10};
	ck_assert_int_eq(0, types_equal(a,b));

	a = (Type){AINT,0,30}; 
	b = (Type){AINT,0,30};
	ck_assert_int_eq(1, types_equal(a,b));
}
END_TEST

START_TEST (test_type_size)
{
	Type t;

	t = (Type){INT,0,0};
	ck_assert_int_eq(4, type_size(t));

	t = (Type){REAL,0,0};
	ck_assert_int_eq(8, type_size(t));

	t = (Type){AREAL,4,8};
	ck_assert_int_eq(40, type_size(t));
}
END_TEST

Suite * symbol_table_suite (void)
{
	Suite *s = suite_create ("Symbol Table");

	/* Core test case */
	TCase *tc_core = tcase_create ("Core");

	// initalize symbol table
	parser_data = (ParserData *)malloc(sizeof(ParserData));
	parser_data->symbol_table = NULL;
	parser_data->sym_eye = NULL;

	tcase_add_test (tc_core, test_check_enter_method);
	tcase_add_test (tc_core, test_check_exit_method);
	tcase_add_test (tc_core, test_check_add_var);
	tcase_add_test (tc_core, test_check_add_prog_param);
	tcase_add_test (tc_core, test_check_add_fun_param);
	tcase_add_test (tc_core, test_get_symbol);
	tcase_add_test (tc_core, test_set_method_type);
	tcase_add_test (tc_core, test_set_method_param_count);
	tcase_add_test (tc_core, test_get_type);
	tcase_add_test (tc_core, test_get_num_params);
	tcase_add_test (tc_core, test_get_param_type);
	tcase_add_test (tc_core, test_types_equal);
	tcase_add_test (tc_core, test_type_size);

	suite_add_tcase (s, tc_core);

	return s;
}