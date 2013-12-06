#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "parser.h"

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		fprintf (stdout, "usage: %s reserved_words_file source_file output_dir\n", argv[0]);
		exit(1);
	}

	FILE *f;

	ParserData *parser_data = (ParserData *)malloc(sizeof(ParserData));

	/* tokenize reserved words file */

	char line[200];
	ReservedWord *head, *curr;
	head = NULL;

	f = fopen (argv[1], "r");

	if (f == NULL)
	{
		fprintf (stderr, "Can't open reserved words file!\n");
		exit(1);
	}

	fprintf(stdout, "Parsing reserved words file.. ");

	while (fgets (line, 200, f) != NULL)
	{
		curr = tokenize_reserved_word_str (line);
		curr->next = head;
		head = curr;
	}

	parser_data->reserved_words = head;
	fclose(f);

	fprintf(stdout, "ok\n");

	/* tokenize input source file */

	parser_data->source = fopen (argv[2], "r");

	if (parser_data->source == NULL)
	{
		fprintf (stderr, "Can't open source file!\n");
		exit(1);
	}

	char *output_dir = malloc(strlen(argv[3]));
	strcpy (output_dir, argv[3]);

	// ensure the output directory is present  
	struct stat fileStat;
	if (stat(output_dir, &fileStat) < 0)
	{
		mode_t process_mask = umask(0);
		mkdir(output_dir, S_IRWXU | S_IRWXG | S_IRWXO);
		umask(process_mask);

		if (stat(output_dir, &fileStat) < 0)
		{
			fprintf (stderr, "Output directory does not exist!\n");
			exit(1);
		}
	}

	// strip trailing "/" from output dir
	if (strcmp (&output_dir[strlen(output_dir)-1], "\\") == 0)
		output_dir[strlen(output_dir)-1] = 0;

	// open listing file for writing
	char *listing_filename = malloc(strlen(output_dir) + 9);
	sprintf(listing_filename, "%s/listing", output_dir);
	parser_data->listing = fopen (listing_filename, "w");

	if (parser_data->listing == NULL)
	{
		fprintf (stderr, "Can't create listing file at %s!\n", listing_filename);
		exit(1);
	}

	// open tokens file for writing
	char *tokens_filename = malloc(strlen(output_dir) + 9);
	sprintf(tokens_filename, "%s/tokens", output_dir);
	parser_data->tokens = fopen (tokens_filename, "w");

	if (parser_data->tokens == NULL)
	{
		fprintf (stderr, "Can't create tokens file at %s!\n", tokens_filename);
		exit(1);
	}

	// open symbol table file for writing
	char *symtable_filename = malloc(strlen(output_dir) + 9);
	sprintf(symtable_filename, "%s/symtable", output_dir);
	parser_data->symbols = fopen (symtable_filename, "w");

	if (parser_data->symbols == NULL)
	{
		fprintf (stderr, "Can't create symbol table file at %s!\n", symtable_filename);
		exit(1);
	}

	// initalize symbol table
	parser_data->symbol_table = NULL;
	parser_data->sym_eye = NULL;

	fprintf(stdout, "Parsing source file..\n");

	/* most important line in this file */
	parse(parser_data);

	if (parser_data->result > 0)
		fprintf(stderr, "Parsing was unsuccessful.\n");
	else
		fprintf(stdout, "Success\n");

	// close up shop
	fclose(parser_data->source);
	fclose(parser_data->listing);
	fclose(parser_data->tokens);
	fclose(parser_data->symbols);

	return parser_data->result;
}