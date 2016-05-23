#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assemble_tokenizer.h"

struct instruction tokenize(char *instr)
{
	char *save;
	char *p;
	const char *delim = ":";

	for (p = strtok_r(instr, delim, &save);
	     p;
	     p = strtok_r(NULL, delim, &save))
		printf("chunk=%s\n", p);

	return tokens;
}
