#include <stdlib.h>
#include <check.h>

#include "check_machine.h"
#include "check_token.h"
#include "check_symbol_table.h"

int main (void)
{
	Suite *suites[] = {
		machine_suite(),
		token_suite(),
		symbol_table_suite()
	};

	int retval = EXIT_SUCCESS;

	int i;
	for (i = 0; i < sizeof(suites)/sizeof(suites[0]); i++)
	{
		int number_failed;
		Suite *s = suites[i];
		SRunner *sr = srunner_create (s);
		srunner_run_all (sr, CK_NORMAL);
		number_failed = srunner_ntests_failed (sr);
		srunner_free (sr);
		if (number_failed > 0)
			retval = EXIT_FAILURE;
	}

	return retval;
}