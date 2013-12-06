#include "check_token.h"

START_TEST (test_reserved_words)
{
	ReservedWord *word = tokenize_reserved_word_str ("program\t1000\t0");

	ck_assert(strcmp(word->name, "program") == 0);
	ck_assert(word->token != NULL);
	ck_assert(word->token->type == TOKEN_PROGRAM);
	ck_assert(word->token->attribute == 0);

	word = tokenize_reserved_word_str ("and\t1019\t100");

	ck_assert(strcmp(word->name, "and") == 0);
	ck_assert(word->token != NULL);
	ck_assert(word->token->type == TOKEN_MULOP);
	ck_assert(word->token->attribute == 100);
}
END_TEST

START_TEST (test_get_token)
{
	ParserData *parser_data = (ParserData *)malloc(sizeof(ParserData));
	parser_data->tokens = NULL;
	parser_data->listing = NULL;
	parser_data->symbols = NULL;
	parser_data->source = fopen("tests/test_get_token.pas", "r");

	ck_assert(parser_data->source != NULL);

	// build a reserved word list
	ReservedWord *reserved_words = tokenize_reserved_word_str ("program\t1000\t0");
	ReservedWord *next = tokenize_reserved_word_str ("begin\t1007\t0");
	ReservedWord *next2 = tokenize_reserved_word_str ("while\t1013\t0");
	reserved_words->next = next;
	next->next = next2;	
	parser_data->reserved_words = reserved_words;

	// start checking tokens - program
	MachineResult *res = get_next_token (parser_data, TOKEN_OPTION_SQUASH_ERRS);
	ck_assert(res != NULL);
	ck_assert(res->token->type == TOKEN_PROGRAM);
	ck_assert(res->token->attribute == 0);

	// test
	res = get_next_token (parser_data, TOKEN_OPTION_NOP | TOKEN_OPTION_SQUASH_ERRS);
	ck_assert(res != NULL);
	ck_assert(res->token->type == TOKEN_ID);

	// NOP
	for (int i = 0; i < 5; i++) {
		res = get_next_token (parser_data, TOKEN_OPTION_NOP | TOKEN_OPTION_SQUASH_ERRS);
		ck_assert(res != NULL);
		ck_assert(res->token->type == TOKEN_ID);
	}

	res = get_next_token (parser_data, TOKEN_OPTION_SQUASH_ERRS);
	ck_assert(res != NULL);
	ck_assert(res->token->type == TOKEN_ID);

/*
	// #
	res = get_next_token (parser_data, TOKEN_OPTION_SQUASH_ERRS);
	ck_assert(res != NULL);
	ck_assert(res->token->type == TOKEN_LEXERR);
	ck_assert(res->token->attribute == MACHINE_ERR_NO_MATCH);

	// @
	res = get_next_token (parser_data, TOKEN_OPTION_SQUASH_ERRS);
	ck_assert(res != NULL);
	ck_assert(res->token->type == TOKEN_LEXERR);
	ck_assert(res->token->attribute == MACHINE_ERR_NO_MATCH);

	// integer
	res = get_next_token (parser_data, TOKEN_OPTION_SQUASH_ERRS);
	ck_assert(res != NULL);
	ck_assert(res->token->type == TOKEN_LEXERR);
	ck_assert(res->token->attribute == MACHINE_ERR_INT_TOO_LONG);

	// real
	res = get_next_token (parser_data, TOKEN_OPTION_SQUASH_ERRS);
	ck_assert(res != NULL);
	ck_assert(res->token->type == TOKEN_LEXERR);
	ck_assert(res->token->attribute == MACHINE_ERR_REAL_YY_TOO_LONG);
*/
	
	// eof
	res = get_next_token (parser_data, TOKEN_OPTION_SQUASH_ERRS);
	ck_assert(res != NULL);
	ck_assert(res->token->type == TOKEN_EOF);
	ck_assert(res->token->attribute == 0);

	// eof - multiple times
	for (int i = 0; i < 5; i++) {
		res = get_next_token (parser_data, TOKEN_OPTION_SQUASH_ERRS);
		ck_assert(res != NULL);
		ck_assert(res->token->type == TOKEN_EOF);
		ck_assert(res->token->attribute == 0);
	}
}
END_TEST

Suite * token_suite (void)
{
	Suite *s = suite_create ("Tokens");

	/* Core test case */
	TCase *tc_core = tcase_create ("Core");
	tcase_add_test (tc_core, test_reserved_words);
	tcase_add_test (tc_core, test_get_token);
	suite_add_tcase (s, tc_core);

	return s;
}