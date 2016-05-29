#include "assemble_tokenizer.h"
#include "assemble_dictionary.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

static void generate_op2(long int op2, uint8_t *imm, int *shift)
{
	long int temp = op2;
	int s;

	if (op2 <= 255) {
		*imm = op2;
		*shift = 0;
	} else {
		while (!(temp & 1)) {
			temp >>= 1;
			s++;
		}

		if (temp > 255) {
			fprintf(stderr, "Operand2 value doesn't fit.");
			exit(EXIT_FAILURE);
		}

		*imm = temp;
		*shift = (31 - s)/2;
	}
}

static void init_dpi(struct instruction *tokens)
{
	long int j = 0;
	char *token;
	uint8_t imm;
	int shift;

	tokens->instr.dpi.opcode = tokens->code;
	tokens->instr.dpi.setcond = false;

	switch (tokens->code) {
	case 8:
	case 9:
	case 10:
		tokens->instr.dpi.setcond = true;
		tokens->instr.dpi.rd = 0;
		tokens->instr.dpi.rn = atoi(strtok(NULL, " ,") + 1);
		break;
	case 13:
		tokens->instr.dpi.rn = 0;
		tokens->instr.dpi.rd = atoi(strtok(NULL, " ,") + 1);
		break;
	default:
		tokens->instr.dpi.rd = atoi(strtok(NULL, " ,") + 1);
		tokens->instr.dpi.rn = atoi(strtok(NULL, " ,") + 1);
		break;
	}

	token = strtok(NULL, " ,");
	tokens->instr.dpi.op2.immediate = token[0] == '#';
	j = strtol(token + 1, NULL, 0);
	generate_op2(j, &imm, &shift);

	if (tokens->instr.dpi.op2.immediate) {
		tokens->instr.dpi.op2.offset.imm.imm = imm;
		tokens->instr.dpi.op2.offset.imm.rotate = shift;
	} else
		tokens->instr.dpi.op2.offset.reg.rm = j;
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
	char *token;
	uint32_t add;
	int d;

	tokens->instr.sdt.load = !tokens->code;
	tokens->instr.sdt.up = true;
	d = atoi(strtok(NULL, " ,") + 1);
	tokens->instr.sdt.rd = d;
	tokens->instr.sdt.preindexing = true;
	tokens->sdt_offset = 0;

	token = strtok(NULL, "");

	if (token[0] == '=') {
		add = strtol(token + 1, NULL, 0);

		if (add >> 12) {
			tokens->type = 0;
			tokens->code = 13;
			tokens->instr.dpi.rn = 0;
			tokens->instr.dpi.rd = d;
			/* TODO: Rotation should be implemented here.*/
			tokens->instr.dpi.op2.immediate = true;
			tokens->instr.dpi.op2.offset.imm.imm = add;
		} else
			tokens->sdt_offset = add;

	} else if (token[3] == ']') {
		tokens->instr.sdt.rn = atoi(strtok(token, "[]") + 1);
		token = strtok(NULL, "# ,");

		if (token) {
			tokens->instr.sdt.preindexing = false;
			tokens->instr.sdt.offset.immediate = true;
			add = atoi(token + 1);
			tokens->instr.sdt.offset.offset.imm = add;
		} else
			tokens->instr.sdt.offset.immediate = false;

	} else {
		tokens->instr.sdt.rn = atoi(strtok(token, "[,") + 1);
		tokens->instr.sdt.offset.immediate = true;
		add = atoi(strtok(NULL, "# ]"));
		tokens->instr.sdt.offset.offset.imm = add;
	}
}

static void init_branch(struct instruction *tokens)
{
	tokens->jump = strtok(NULL, " ");
}

static void init_lsl(struct instruction *tokens)
{
	char *token;
	int j = 0;

	tokens->type = 0;
	tokens->code = 13;
	tokens->instr.dpi.rd = atoi(strtok(NULL, " ,") + 1);
	tokens->instr.dpi.op2.immediate = false;
	tokens->instr.dpi.op2.offset.reg.rm = tokens->instr.dpi.rd;
	tokens->instr.dpi.op2.offset.reg.shift_type = 0;
	tokens->instr.dpi.op2.offset.reg.constant = 0;
	token = strtok(NULL, " ,#");
	j = strtol(token, NULL, 0);
	tokens->instr.dpi.op2.offset.reg.amount.integer = j;
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

	tokens->mnemonic = false;

	if (strcmp(token, " ") > 0) {
		tokens->mnemonic = true;
		tokens->type = classify_instr(token);
		tokens->code = instr_code(token, tokens->type);

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
		case 4:
			init_lsl(tokens);
			break;
		}
	}
}
