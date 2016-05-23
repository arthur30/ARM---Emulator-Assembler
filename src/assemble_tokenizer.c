#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assemble_tokenizer.h"

struct instruction tokenize(char *instr)
{
	char *save;
	char *token;
	int has_label = 0;

	token = strtok_r(instr, ":", &save);
	if (instr != NULL)
		has_label = 1;

	printf("chunk=%s\n", token);

	if (has_label) {
		for (token = strtok_r(NULL, " ,", &save);
			token;
			token = strtok_r(NULL, " ,", &save))
			printf("chunk=%s\n", token);
	}

	return tokens;
}
