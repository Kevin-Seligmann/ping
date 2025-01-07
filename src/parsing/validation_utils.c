#include "parsing.h"

unsigned long get_numeric(char *arg)
{
	unsigned long number;
	char *ptr;

	number = strtoul(arg, ptr, 0);
	if (*ptr)
	{
		fprintf(stderr, "invalid value (`%s' near `%s')", arg, ptr);
		exit(1);
	}
	return number;
}
