#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assemble_tokenizer.h"

#define NO_LABEL "NO LABEL SUPPLIED"

struct instruction tokenize(char *instr)
{
	int i = 0;
	char nolabel[] = NO_LABEL;
	char *save;
	char *replace = NULL;
	char *token;

	token = strtok_r(instr, ":", &save);
	if (strlen(save) == 0) {
		replace = token;
		tokens.label = nolabel;
	} else
		tokens.label = token;

	for (token = strtok_r(replace, " ,", &save), i = 0;
		token;
		token = strtok_r(NULL, " ,", &save), i++) {

		switch (i) {
		case 0:
			tokens.mnemonic = token;
			break;
		case 1:
			tokens.op1 = token;
			break;
		case 2:
			tokens.op2 = token;
			break;
		case 3:
			tokens.op3 = token;
			break;
		case 4:
			tokens.op4 = token;
			break;
		default:
			printf("ERROR: Instruction not well defined.");
			exit(EXIT_FAILURE);
			break;
		}
	}

	return tokens;
}
