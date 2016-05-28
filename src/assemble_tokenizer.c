#include "assemble_tokenizer.h"
#include "assemble_dictionary.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static void init_dpi(struct instruction *tokens)
{
	int j = 0;
	char *token;

	tokens->instr.dpi.opcode = tokens->code;
	tokens->instr.dpi.setcond = false;

	switch (tokens->code) {
	case 8:
	case 9:
	case 10:
		tokens->instr.dpi.setcond = true;
		tokens->instr.dpi.rd = 0;
		tokens->instr.dpi.rn = atoi(strtok(NULL, " ,") + 1);
		token = strtok(NULL, " ,");
		tokens->instr.dpi.op2.immediate = token[0] == '#';
		j = atoi(token + 1);

		if (tokens->instr.dpi.op2.immediate)
			tokens->instr.dpi.op2.offset.imm.imm = j;
		else
			tokens->instr.dpi.op2.offset.reg.rm = j;
		break;
	case 13:
		tokens->instr.dpi.rn = 0;
		tokens->instr.dpi.rd = atoi(strtok(NULL, " ,") + 1);
		token = strtok(NULL, " ,");
		printf("%s\n", token);
		tokens->instr.dpi.op2.immediate = token[0] == '#';
		j = atoi(token + 1);

		if (tokens->instr.dpi.op2.immediate)
			tokens->instr.dpi.op2.offset.imm.imm = j;
		else
			tokens->instr.dpi.op2.offset.reg.rm = j;
		break;
	default:
		tokens->instr.dpi.rd = atoi(strtok(NULL, " ,") + 1);
		tokens->instr.dpi.rn = atoi(strtok(NULL, " ,") + 1);
		token = strtok(NULL, " ,");
		tokens->instr.dpi.op2.immediate = token[0] == '#';
		j = atoi(token + 1);

		if (tokens->instr.dpi.op2.immediate)
			tokens->instr.dpi.op2.offset.imm.imm = j;
		else
			tokens->instr.dpi.op2.offset.reg.rm = j;
		break;
	}

}

static void init_mult(struct instruction *tokens)
{
	tokens->instr.mult.accumulate = tokens->code;
	tokens->instr.mult.setcond = false;
	tokens->instr.mult.rd = atoi(strtok(NULL, " ,") + 1);
	tokens->instr.mult.rm = atoi(strtok(NULL, " ,") + 1);
	tokens->instr.mult.rs = atoi(strtok(NULL, " ,") + 1);

	if (tokens->code)
		tokens->instr.mult.rn = atoi(strtok(NULL, " ,") + 1);
}

static void init_sdt(struct instruction *tokens)
{
	(void) tokens;
}

static void init_branch(struct instruction *tokens)
{
	(void) tokens;
}

void tokenize(char *orig_instr, struct instruction *tokens)
{
	char *token;
	char *instr = strdup(orig_instr);

	token = strtok(instr, ":");
	tokens->label = token;

	if (!strcmp(token, orig_instr))
		tokens->label = NULL;


	if (tokens->label)
		token = strtok(NULL, " ");
	else
		token = strtok(instr, " ");

	if (token) {
		tokens->mnemonic = true;
		tokens->type = classify_instr(token);
		printf("%i\t", tokens->type);
		tokens->code = instr_code(token, tokens->type);
		printf("%i\t", tokens->code);

		switch (tokens->type) {
		case 0:
			init_dpi(tokens);
			break;
		case 1:
			init_mult(tokens);
			break;
		case 2:
			init_sdt(tokens);
			break;
		case 3:
			init_branch(tokens);
			break;
		}
	}
}
