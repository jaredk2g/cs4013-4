#include "machine.h"

MachineResult machine_omega(char *in, ReservedWord *reserved_words)
{
	/*
	This machine is the super machine that ties all
	of the other machines together. If no match is made
	then an unrecognized TOKEN_LEXERR token will be returned.
	There is an ordering to the machines according to:
	  i) efficiency rule
	  ii) tokenizing rule
	*/

	char *f = in;

	// start with whitespace machine
	MachineResult res = machine_whitespace(f);

	int i = 1;
	while (i < 6 && res.token->type == TOKEN_LEXERR && res.token->attribute == MACHINE_ERR_NO_MATCH)
	{
		switch (i)
		{
		case 1: res = machine_idres(f, reserved_words); break;
		case 2: res = machine_longreal(f); break;
		case 3: res = machine_real(f); break;
		case 4: res = machine_int(f); break;
		case 5: res = machine_catchall(f); break;
		}

		i++;
	}

	// calculate resulting lexeme
	res.lexeme = (char *)malloc(res.f - in + 1);
	strncpy(res.lexeme, in, res.f - in);
	res.lexeme[res.f - in] = '\0';	

	return res;
}

MachineResult machine_whitespace(char *in)
{
	char *f = in;
	int s = 1;
	MachineResult res;
	res.token = (Token *)malloc(sizeof(Token));
	res.token->type = TOKEN_EMPTY;
	res.token->attribute = 0;

	while (res.token->type == TOKEN_EMPTY)
	{
		switch (s)
		{
		case 0: // no match
			f--;
			res.token->type = TOKEN_LEXERR;
			res.token->attribute = MACHINE_ERR_NO_MATCH;
		break;
		case 1: // start
			if (is_whitespace(*f))	s++;
			else					s--;
		break;
		case 2:
			if (!is_whitespace(*f))
			{
				f--;
				res.token->type = TOKEN_WHITESPACE;
				res.token->attribute = 0;
			}
		break;
		}

		f++;
	}

	res.f = f;

	return res;
}

MachineResult machine_idres(char *in, ReservedWord *reserved_words)
{
	char *f = in;
	int s = 1;
	MachineResult res;
	res.token = (Token *)malloc(sizeof(Token));
	res.token->type = TOKEN_EMPTY;
	res.token->attribute = 0;

	while (res.token->type == TOKEN_EMPTY)
	{
		switch (s)
		{
		case 0: // no match
			f--;
			res.token->type = TOKEN_LEXERR;
			res.token->attribute = MACHINE_ERR_NO_MATCH;
		break;
		case 1: // start
			if (is_alpha(*f))	s++;
			else				s--;
		break;
		case 2: // letter
			if (!is_alpha_numeric(*f))
			{
				// check if reserved word
				char *word = (char *)malloc(f - in + 1);
				strncpy(word, in, f - in);
				word[f - in] = '\0';

				f--;

				ReservedWord *rw = is_reserved_word(word, reserved_words);
				if (rw != NULL)
					res.token = rw->token;
				else
				{
					// make sure id is not too long
					if (strlen(word) > MAX_ID_LEN)
					{
						res.token->type = TOKEN_LEXERR;
						res.token->attribute = MACHINE_ERR_ID_TOO_LONG;
					}
					else
					{
						res.token->type = TOKEN_ID;
					}
				}
			}
		break;
		}

		f++;
	}

	res.f = f;

	return res;		
}

MachineResult machine_int(char *in)
{
	char *f = in;
	int s = 1;
	MachineResult res;
	res.token = (Token *)malloc(sizeof(Token));
	res.token->type = TOKEN_EMPTY;
	res.token->attribute = 0;

	while (res.token->type == TOKEN_EMPTY)
	{
		switch (s)
		{
		case 0: // no match
			f--;
			res.token->type = TOKEN_LEXERR;
			res.token->attribute = MACHINE_ERR_NO_MATCH;
		break;
		case 1: // start
			if (is_numeric(*f))	s++;
			else				s--;
		break;
		case 2:
			if (!is_numeric(*f))
			{
				// make sure int is not too long
				int len = f - in;
				if (len > MAX_INT_LEN)
				{
					res.token->type = TOKEN_LEXERR;
					res.token->attribute = MACHINE_ERR_INT_TOO_LONG;
				}
				// make sure there is not a leading zero
				else if (*in == '0' && len > 1)
				{
					res.token->type = TOKEN_LEXERR;
					res.token->attribute = MACHINE_ERR_NUM_LEADING_ZERO;
				}
				else
				{
					res.token->type = TOKEN_NUM;
					res.token->attribute = ATTRIBUTE_INT;
				}

				f--;				
			}
		break;
		}

		f++;
	}

	res.f = f;

	return res;
}

MachineResult machine_real(char *in)
{
	char *f = in;
	char *dot;
	int s = 1;
	MachineResult res;
	res.token = (Token *)malloc(sizeof(Token));
	res.token->type = TOKEN_EMPTY;
	res.token->attribute = 0;

	while (res.token->type == TOKEN_EMPTY)
	{
		switch (s)
		{
		case 0: // no match
			f--;
			res.token->type = TOKEN_LEXERR;
			res.token->attribute = MACHINE_ERR_NO_MATCH;
		break;
		case 1: // start
			if (is_numeric(*f))	s++;
			else				s--;
		break;
		case 2: // digit
			if (!is_numeric(*f))
			{
				if (*f == '.')
				{
					s++;
					dot = f;
				}
				else
					s = 0;
			}
		break;
		case 3: // period
			if (is_numeric(*f))	s++;
			else {
				s = 0;
				f--;
			}
		break;
		case 4: // digit
			if (!is_numeric(*f))	s++;
		break;
		case 5:
			f--;

			// make sure the int is not too long
			if (dot - in > MAX_REAL_XX_LEN)
			{
				res.token->type = TOKEN_LEXERR;
				res.token->attribute = MACHINE_ERR_REAL_XX_TOO_LONG;
			}
			// make sure fraction is not too long
			else if (f - dot - 1 > MAX_REAL_YY_LEN)
			{
				res.token->type = TOKEN_LEXERR;
				res.token->attribute = MACHINE_ERR_REAL_YY_TOO_LONG;
			}
			// make sure there is not a leading zero
			else if (*in == '0')
			{
				res.token->type = TOKEN_LEXERR;
				res.token->attribute = MACHINE_ERR_NUM_LEADING_ZERO;
			}
			else
			{
				res.token->type = TOKEN_NUM;
				res.token->attribute = ATTRIBUTE_REAL;
			}

			f--;
		break;
		}

		f++;
	}

	res.f = f;

	return res;
}

MachineResult machine_longreal(char *in)
{
	char *f = in;
	char *dot;
	char *e;
	int sign = 0;
	int s = 1;
	MachineResult res;
	res.token = (Token *)malloc(sizeof(Token));
	res.token->type = TOKEN_EMPTY;
	res.token->attribute = 0;

	while (res.token->type == TOKEN_EMPTY)
	{
		switch (s)
		{
		case 0: // no match
			f--;
			res.token->type = TOKEN_LEXERR;
			res.token->attribute = MACHINE_ERR_NO_MATCH;
		break;
		case 1: // start
			if (is_numeric(*f))	s++;
			else				s--;
		break;
		case 2: // digit
			if (!is_numeric(*f))
			{
				if (*f == '.')
				{
					dot = f;
					s++;
				}
				else
					s = 0;
			}
		break;
		case 3: // period
			if (!is_numeric(*f))
			{
				if (*f == 'E')
				{
					e = f;
					s++;
				}
				else
					s = 0;
			}
		break;
		case 4: // digit
			if (is_plus_or_minus(*f))	s++;
			else if (is_numeric(*f))	s = 6;
			else						s = 0;
		break;
		case 5: // sign
			sign = 1;

			if (is_numeric(*f))
				s++;
			else
				s = 0;
		break;
		case 6: // digit
			if (!is_numeric(*f)) s++;
		break;
		case 7:
			f--;

			if (dot - in > MAX_REAL_XX_LEN)
			{
				res.token->type = TOKEN_LEXERR;
				res.token->attribute = MACHINE_ERR_REAL_XX_TOO_LONG;
			}
			// make sure fraction is not too long
			else if (e - dot - 1 > MAX_REAL_YY_LEN)
			{
				res.token->type = TOKEN_LEXERR;
				res.token->attribute = MACHINE_ERR_REAL_YY_TOO_LONG;
			}
			// make sure exponent is not too long
			else if (f - e - sign - 1 > MAX_REAL_ZZ_LEN)
			{
				res.token->type = TOKEN_LEXERR;
				res.token->attribute = MACHINE_ERR_REAL_ZZ_TOO_LONG;
			}
			// make sure there is not a leading zero
			else if (*in == '0')
			{
				res.token->type = TOKEN_LEXERR;
				res.token->attribute = MACHINE_ERR_NUM_LEADING_ZERO;
			}
			else
			{
				res.token->type = TOKEN_NUM;
				res.token->attribute = ATTRIBUTE_LONGREAL;
			}

			f--;
		break;
		}

		f++;
	}

	res.f = f;

	return res;
}

MachineResult machine_catchall(char *in)
{
	char *f = in;
	int s = 1;
	MachineResult res;
	res.token = (Token *)malloc(sizeof(Token));
	res.token->type = TOKEN_EMPTY;
	res.token->attribute = 0;

	while (res.token->type == TOKEN_EMPTY)
	{
		switch (s)
		{
		case 0: // no match
			f--;
			res.token->type = TOKEN_LEXERR;
			res.token->attribute = MACHINE_ERR_NO_MATCH;
		break;
		case 1: // start
			if (*f == '+')
			{
				res.token->type = TOKEN_ADDOP;
				res.token->attribute = *f;
			}
			else if (*f == '-')
			{
				res.token->type = TOKEN_ADDOP;
				res.token->attribute = *f;
			}
			else if (*f == '*')
			{
				res.token->type = TOKEN_MULOP;
				res.token->attribute = *f;
			}
			else if (*f == '/')
			{
				res.token->type = TOKEN_MULOP;
				res.token->attribute = *f;
			}
			else if (*f == ',')
			{
				res.token->type = TOKEN_COMMA;
				res.token->attribute = 0;
			}
			else if (*f == ';')
			{
				res.token->type = TOKEN_SEMICOLON;
				res.token->attribute = 0;
			}
			else if (*f == '(')
			{
				res.token->type = TOKEN_LPAREN;
				res.token->attribute = 0;
			}
			else if (*f == ')')
			{
				res.token->type = TOKEN_RPAREN;
				res.token->attribute = 0;
			}
			else if (*f == '[')
			{
				res.token->type = TOKEN_LBRACKET;
				res.token->attribute = 0;
			}
			else if (*f == ']')
			{
				res.token->type = TOKEN_RBRACKET;
				res.token->attribute = 0;
			}			
			else if (*f == '.')
			{
				res.token->type = TOKEN_PERIOD;
				res.token->attribute = 0;
			}
			else if (*f == ',')
			{
				res.token->type = TOKEN_COMMA;
				res.token->attribute = 0;
			}
			else if (*f == EOF)
			{
				res.token->type = TOKEN_EOF;
				res.token->attribute = 0;
			}
			else if (*f == ':')	s = 2;
			else if (*f == '>')		s = 3;
			else if (*f == '<')	s = 4;
			else if (*f == '=')	s = 5;
			else				s--;
		break;
		case 2: // :
			if (*f == '=')
			{
				res.token->type = TOKEN_ASSIGNOP;
				res.token->attribute = 0;
			}
			else
			{
				f--;
				res.token->type = TOKEN_COLON;
				res.token->attribute = 0;
			}
		break;
		case 3: // >
			if (*f == '=')
			{
				res.token->type = TOKEN_RELOP;
				res.token->attribute = ATTRIBUTE_GE;
			}
			else
			{
				f--;
				res.token->type = TOKEN_RELOP;
				res.token->attribute = *f;
			}
		break;
		case 4: // <
			if (*f == '=')
			{
				res.token->type = TOKEN_RELOP;
				res.token->attribute = ATTRIBUTE_LE;				
			}
			else if (*f == '>')
			{
				res.token->type = TOKEN_RELOP;
				res.token->attribute = ATTRIBUTE_NE;				
			}
			else
			{
				f--;
				res.token->type = TOKEN_RELOP;
				res.token->attribute = *f;
			}
		break;
		case 5:
			f--;
			res.token->type = TOKEN_RELOP;
			res.token->attribute = *f;
		break;
		}

		f++;
	}

	res.f = f;

	return res;
}

/* Useful checks */

int is_alpha(char c)
{
	return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' );
}

int is_alpha_numeric(char c)
{
	return is_alpha (c) || is_numeric (c);
}

int is_numeric(char c)
{
	return c >= '0' && c <= '9';
}

int is_plus_or_minus(char c)
{
	return c == '+' || c == '-';
}

ReservedWord *is_reserved_word(char *word, ReservedWord *reserved_words)
{
	// traverse reserved words list
	ReservedWord *curr = reserved_words;
	while (curr != NULL)
	{
		if (strcmp(word, curr->name) == 0)
			return curr;

		curr = curr->next;
	}

	return NULL;
}

int is_whitespace(char c)
{
	return c == '\n' || c == '\r' || c == '\t' || c == ' ';
}