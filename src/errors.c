#include "errors.h"

void lexerr(struct MachineResult *result, struct ParserData *parser_data)
{
	// output to console
	fprintf (stderr, "%-8s%s on line %d: \"%s\"\n", "LEXERR", attribute_to_str(result->token->attribute), result->line_no, result->lexeme);

	// listing file
	if (parser_data->listing != NULL)
		fprintf (parser_data->listing, "%-8s%-30s%s\n", "LEXERR", attribute_to_str(result->token->attribute), result->lexeme);

	parser_data->result |= PARSER_RESULT_LEXERR;
}

void output_synerr(enum TokenType *expected, int len, struct MachineResult *found, FILE *out)
{
	fprintf (out, "%-8sExpecting ", "SYNERR" );
	for (int i = 0; i < len; i++) {
		fprintf (out, "\"%s\"", token_type_to_str(expected[i]));
		if (i < len - 1) {
			if (len > 2)
				fprintf (out, ", ");
			if (i == len - 2)
				fprintf (out, " or ");
		}
	}
	fprintf (out, " but received \"%s\"", token_type_to_str(found->token->type));

	if (out == stderr)
		fprintf (out, " on line %d", found->line_no);
	
	fprintf (out, "\n");
}

void synerr(enum TokenType *expected, int len, struct MachineResult *found, struct ParserData *parser_data)
{
	// output to console
	output_synerr(expected, len, found, stderr);

	// output to listing file
	output_synerr(expected, len, found, parser_data->listing);

	parser_data->result |= PARSER_RESULT_SYNERR;
}

void semerr(char *err, int line_no, struct ParserData *parser_data)
{
	// output to console
	fprintf (stderr, "%-8s%s on line %d\n", "SEMERR", err, line_no);

	// listing file
	if (parser_data->listing != NULL)
		fprintf (parser_data->listing, "%-8s%s\n", "SEMERR", err);

	parser_data->result |= PARSER_RESULT_SEMERR;
}