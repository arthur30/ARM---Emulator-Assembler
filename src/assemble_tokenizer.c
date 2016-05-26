#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assemble_tokenizer.h"

#define NO_LABEL "NO LABEL SUPPLIED"

void tokenize(char *instr, struct instruction *tokens)
{
	int i = 0;
	char *save;
	char *replace = NULL;
	char *token;

	token = strtok_r(instr, ":", &save);
	tokens->label = strdup(token);

	if (strlen(save) == 0) {
		replace = token;
		tokens->label = NULL;
	}

	for (token = strtok_r(replace, " ,", &save), i = 0;
		token;
		token = strtok_r(NULL, " ,", &save), i++) {

		switch (i) {
		case 0:
			tokens->mnemonic = strdup(token);
			break;
		case 1:
			tokens->op1 = strdup(token);
			break;
		case 2:
			tokens->op2 = strdup(token);
			break;
		case 3:
			tokens->op3 = strdup(token);
			break;
		case 4:
			tokens->op4 = strdup(token);
			break;
		default:
			printf("ERROR: Instruction not well defined.");
			exit(EXIT_FAILURE);
			break;
		}
	}
}
