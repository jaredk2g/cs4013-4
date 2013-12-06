#include "check_machine.h"

START_TEST (test_machine_ws)
{
	char *str = "\n\n\n\t\t\t\t\t     \t\t\n";
	MachineResult res = machine_whitespace(str);
	ck_assert(res.token->type == TOKEN_WHITESPACE);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));

	// invalid whitespace
	str = "notwhitespace";
	res = machine_whitespace(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);
}
END_TEST

START_TEST (test_machine_idres)
{
	// build a reserved word list
	ReservedWord *reserved_words = tokenize_reserved_word_str ("program\t1000\t0");
	ReservedWord *next = tokenize_reserved_word_str ("begin\t1007\t0");
	ReservedWord *next2 = tokenize_reserved_word_str ("while\t1013\t0");
	reserved_words->next = next;
	next->next = next2;

	// valid id
	char *str = "isthisanid";
	MachineResult res = machine_idres(str, reserved_words);
	ck_assert(res.token->type == TOKEN_ID);
	ck_assert(res.f == str + strlen(str));

	// too long id
	str = "thisidiswaytooooolong";
	res = machine_idres(str, reserved_words);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_ID_TOO_LONG);
	ck_assert(res.f == str + strlen(str));

	// invalid id
	str = "#*(*%(&not an id";
	res = machine_idres(str, reserved_words);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);

	// check for reserved words
	str = "while";
	res = machine_idres(str, reserved_words);
	ck_assert(res.token->type == TOKEN_WHILE);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));

	// check symbol table entries
	str = "someid";
	res = machine_idres(str, reserved_words);
	ck_assert(res.token->type == TOKEN_ID);
	ck_assert(res.f == str + strlen(str));

	str = "isthisanid";
	res = machine_idres(str, reserved_words);
	ck_assert(res.token->type == TOKEN_ID);
	ck_assert(res.f == str + strlen(str));

}
END_TEST

START_TEST (test_machine_int)
{
	// valid int
	char *str = "1234567890";
	MachineResult res = machine_int(str);
	ck_assert(res.token->type == TOKEN_NUM);
	ck_assert(res.token->attribute == ATTRIBUTE_INT);
	ck_assert_int_eq(1234567890, res.intval);
	ck_assert(res.f == str + strlen(str));

	// valid int
	str = "0";
	res = machine_int(str);
	ck_assert(res.token->type == TOKEN_NUM);
	ck_assert(res.token->attribute == ATTRIBUTE_INT);
	ck_assert_int_eq(0, res.intval);
	ck_assert(res.f == str + strlen(str));

	// invalid int
	str = "notanint";
	res = machine_int(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);

	// int too long
	str = "12345678901";
	res = machine_int(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_INT_TOO_LONG);
	ck_assert(res.f == str + strlen(str));

	// leading zero
	str = "007";
	res = machine_int(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NUM_LEADING_ZERO);
	ck_assert(res.f == str + strlen(str));
}
END_TEST

START_TEST (test_machine_real)
{
	// valid real
	char *str = "12.34567";
	MachineResult res = machine_real(str);
	ck_assert(res.token->type == TOKEN_NUM);
	ck_assert(res.token->attribute == ATTRIBUTE_REAL);
	ck_assert(res.f == str + strlen(str));

	// invalid real
	str = "12345.";
	res = machine_real(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);

	// invalid real
	str = "123408";
	res = machine_real(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);

	// real xx too long
	str = "123456.803";
	res = machine_real(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_REAL_XX_TOO_LONG);
	ck_assert(res.f == str + strlen(str));

	// real yy too long
	str = "12.345678";
	res = machine_real(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_REAL_YY_TOO_LONG);
	ck_assert(res.f == str + strlen(str));

	// leading zero
	str = "007.1";
	res = machine_real(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NUM_LEADING_ZERO);
	ck_assert(res.f == str + strlen(str));
}
END_TEST

START_TEST (test_machine_longreal)
{
	// valid longreal
	char *str = "12345.01E+1";
	MachineResult res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_NUM);
	ck_assert(res.token->attribute == ATTRIBUTE_LONGREAL);
	ck_assert(res.f == str + strlen(str));

	// valid longreal
	str = "12.34567E-22";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_NUM);
	ck_assert(res.token->attribute == ATTRIBUTE_LONGREAL);
	ck_assert(res.f == str + strlen(str));

	// valid longreal
	str = "12.34E13";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_NUM);
	ck_assert(res.token->attribute == ATTRIBUTE_LONGREAL);
	ck_assert(res.f == str + strlen(str));

	// valid longreal
	str = "12.E13";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_NUM);
	ck_assert(res.token->attribute == ATTRIBUTE_LONGREAL);
	ck_assert(res.f == str + strlen(str));

	// no match (int)
	str = "123408";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);

	// no match (real)
	str = "1234.08";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);

	// longreal xx too long
	str = "123456.803E+2";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_REAL_XX_TOO_LONG);
	ck_assert(res.f == str + strlen(str));

	// longreal yy too long
	str = "12.345678E+1";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_REAL_YY_TOO_LONG);
	ck_assert(res.f == str + strlen(str));

	// longreal zz too long
	str = "12345.803E-123";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_REAL_ZZ_TOO_LONG);
	ck_assert(res.f == str + strlen(str));

	// leading zero
	str = "007.1E19";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NUM_LEADING_ZERO);
	ck_assert(res.f == str + strlen(str));

	// Shenoi tests
	str = "3.E+";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);

	str = "3E+4";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);

	str = "3.4E+";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);

	str = "3E+";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);

	str = "3E4";
	res = machine_longreal(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);
}
END_TEST

START_TEST (test_machine_catchall)
{
	char *str = "+";
	MachineResult res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_ADDOP);
	ck_assert(res.token->attribute == '+');
	ck_assert(res.f == str + strlen(str));

	str = "-";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_ADDOP);
	ck_assert(res.token->attribute == '-');
	ck_assert(res.f == str + strlen(str));

	str = "*";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_MULOP);
	ck_assert(res.token->attribute == '*');
	ck_assert(res.f == str + strlen(str));

	str = "/";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_MULOP);
	ck_assert(res.token->attribute == '/');
	ck_assert(res.f == str + strlen(str));

	str = ",";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_COMMA);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));

	str = ".";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_PERIOD);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));

	str = ";";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_SEMICOLON);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));

	str = "(";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_LPAREN);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));

	str = ")";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_RPAREN);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));

	str = "[";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_LBRACKET);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));

	str = "]";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_RBRACKET);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));

	str = ":";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_COLON);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));

	str = ":=";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_ASSIGNOP);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));

	str = ">";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_RELOP);
	ck_assert(res.token->attribute == '>');
	ck_assert(res.f == str + strlen(str));

	str = ">=";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_RELOP);
	ck_assert(res.token->attribute == ATTRIBUTE_GE);
	ck_assert(res.f == str + strlen(str));

	str = "<";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_RELOP);
	ck_assert(res.token->attribute == '<');
	ck_assert(res.f == str + strlen(str));

	str = "<=";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_RELOP);
	ck_assert(res.token->attribute == ATTRIBUTE_LE);
	ck_assert(res.f == str + strlen(str));

	str = "<>";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_RELOP);
	ck_assert(res.token->attribute == ATTRIBUTE_NE);
	ck_assert(res.f == str + strlen(str));

	str = "=";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_RELOP);
	ck_assert(res.token->attribute == '=');
	ck_assert(res.f == str + strlen(str));

	// invalid
	str = "thisdoesnotwork";
	res = machine_catchall(str);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);
}
END_TEST

START_TEST (test_machine_omega)
{
	/* test each type of machine in the super machine */

	// build a reserved word list
	ReservedWord *reserved_words = tokenize_reserved_word_str ("program\t1000\t0");
	ReservedWord *next = tokenize_reserved_word_str ("begin\t1007\t0");
	ReservedWord *next2 = tokenize_reserved_word_str ("while\t1013\t0");
	reserved_words->next = next;
	next->next = next2;	

	// whitespace
	char *str = "\n\n\n\n   \t\t\t\t\t";
	MachineResult res = machine_omega(str, reserved_words);
	ck_assert(res.token->type == TOKEN_WHITESPACE);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));
	ck_assert(strcmp(res.lexeme, str) == 0);

	// id
	str = "nahnahnah";
	res = machine_omega(str, reserved_words);
	ck_assert(res.token->type == TOKEN_ID);
	ck_assert(res.f == str + strlen(str));
	ck_assert(strcmp(res.lexeme, str) == 0);

	// reserved word
	str = "while";
	res = machine_omega(str, reserved_words);
	ck_assert(res.token->type == TOKEN_WHILE);
	ck_assert(res.token->attribute == 0);
	ck_assert(res.f == str + strlen(str));
	ck_assert(strcmp(res.lexeme, str) == 0);

	// int
	str = "123840";
	res = machine_omega(str, reserved_words);
	ck_assert(res.token->type == TOKEN_NUM);
	ck_assert(res.token->attribute == ATTRIBUTE_INT);
	ck_assert(res.f == str + strlen(str));
	ck_assert(strcmp(res.lexeme, str) == 0);

	// real
	str = "23480.8334";
	res = machine_omega(str, reserved_words);
	ck_assert(res.token->type == TOKEN_NUM);
	ck_assert(res.token->attribute == ATTRIBUTE_REAL);
	ck_assert(res.f == str + strlen(str));
	ck_assert(strcmp(res.lexeme, str) == 0);

	// longreal
	str = "23480.8334E-83";
	res = machine_omega(str, reserved_words);
	ck_assert(res.token->type == TOKEN_NUM);
	ck_assert(res.token->attribute == ATTRIBUTE_LONGREAL);
	ck_assert(res.f == str + strlen(str));
	ck_assert(strcmp(res.lexeme, str) == 0);

	// catchall
	str = "+";
	res = machine_omega(str, reserved_words);
	ck_assert(res.token->type == TOKEN_ADDOP);
	ck_assert(res.token->attribute == '+');
	ck_assert(res.f == str + strlen(str));
	ck_assert(strcmp(res.lexeme, str) == 0);

	// no match
	str = "&YDF(NSDFU9s*^(&)))";
	res = machine_omega(str, reserved_words);
	ck_assert(res.token->type == TOKEN_LEXERR);
	ck_assert(res.token->attribute == MACHINE_ERR_NO_MATCH);
	ck_assert(res.f == str + 1);
	ck_assert(strcmp(res.lexeme, "&") == 0);
}
END_TEST

Suite * machine_suite (void)
{
	Suite *s = suite_create ("Machines");

	/* Core test case */
	TCase *tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_machine_ws);
	tcase_add_test (tc_core, test_machine_idres);
	tcase_add_test (tc_core, test_machine_int);
	tcase_add_test (tc_core, test_machine_real);
	tcase_add_test (tc_core, test_machine_longreal);
	tcase_add_test (tc_core, test_machine_catchall);
	tcase_add_test (tc_core, test_machine_omega);

	suite_add_tcase (s, tc_core);

	return s;
}