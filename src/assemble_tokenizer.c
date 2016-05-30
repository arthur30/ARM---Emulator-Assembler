#include "assemble_tokenizer.h"
#include "assemble_dictionary.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#define SHIFT_BIT_SIZE (1 << 4)
#define IMM_BIT_SIZE (1 << 8)

static int generate_op2(uint32_t op2, uint8_t *imm, uint8_t *shift)
{
	int i;

	for (i = 0; i < SHIFT_BIT_SIZE; i++) {
		if (op2 < IMM_BIT_SIZE) {
			*imm = (uint8_t)op2;
			*shift = i;
			return 0;
		}
		op2 = (op2 << 2) | (op2 >> 30);
	}

	return -1;
}

static void init_dpi(struct instruction *tokens)
{
	uint32_t j = 0;
	char *token;
	uint8_t imm;
	uint8_t shift;

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
	j = (uint32_t)strtol(token + 1, NULL, 0);
	if (generate_op2(j, &imm, &shift)) {
		fprintf(stderr, "Operand2 value doesn't fit.");
		exit(EXIT_FAILURE);

	}

	if (tokens->instr.dpi.op2.immediate) {
		tokens->instr.dpi.op2.offset.imm.imm = imm;
		tokens->instr.dpi.op2.offset.imm.rotate = shift;
	} else {
		tokens->instr.dpi.op2.offset.reg.rm = j;
		token = strtok(NULL, " ,\n");

		if (token) {
			tokens->instr.dpi.op2.offset.reg.shift_type =
							instr_code(token, 6);
			tokens->instr.dpi.op2.offset.reg.constant = true;

			token = strtok(NULL, " ,\n");
			if (token[0] == '#') {
				tokens->instr.dpi.op2.offset.reg.constant =
									false;
				tokens->instr.dpi.op2.offset.reg.amount.integer
						= strtol(token	+ 1, NULL, 0);
			} else {
				tokens->instr.dpi.op2.offset.reg.amount.rs =
						strtol(token + 1, NULL, 0);
			}
		}
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
	char *token;
	int32_t add;
	int d;
	uint8_t imm;
	uint8_t shift;

	tokens->instr.sdt.load = !tokens->code;
	tokens->instr.sdt.up = true;
	d = atoi(strtok(NULL, " ,") + 1);
	tokens->instr.sdt.rd = d;
	tokens->instr.sdt.preindexing = true;
	tokens->instr.sdt.offset.immediate = false;
	tokens->sdt_offset = 0;

	token = strtok(NULL, "");

	if (token[0] == ' ')
		token++;


	if (token[0] == '=') {

		if (tokens->code) {
			fprintf(stderr, "Cannot store in immediate values.");
			exit(EXIT_FAILURE);
		}

		add = strtol(token + 1, NULL, 0);

		if (generate_op2(add, &imm, &shift)) {
			tokens->sdt_offset = add;
			tokens->instr.sdt.offset.immediate = false;
		} else {
			tokens->type = 0;
			tokens->code = 13;
			tokens->instr.dpi.rn = 0;
			tokens->instr.dpi.rd = d;
			tokens->instr.dpi.setcond = false;
			tokens->instr.dpi.op2.immediate = true;
			tokens->instr.dpi.op2.offset.imm.imm = imm;
			tokens->instr.dpi.op2.offset.imm.rotate = shift;
		}

	} else if (token[3] == ']') {
		tokens->instr.sdt.rn = atoi(strtok(token, "[]") + 1);
		token = strtok(NULL, " ,\n");

		if (token) {
			tokens->instr.sdt.preindexing = false;
			add = strtol(token + 1, NULL, 0);

			if (token[0] == '#') {
				tokens->instr.sdt.up = add > 0;
				tokens->instr.sdt.offset.offset.imm =
								(uint32_t)add;
			} else {
				tokens->instr.sdt.offset.immediate = true;
				tokens->instr.sdt.offset.offset.reg.rm = add;
			}
		}

	} else {
		tokens->instr.sdt.rn = atoi(strtok(token, "[,") + 1);
		token = strtok(NULL, " ]\n");
		add = strtol(token + 1, NULL, 0);

		if (token[0] == '#') {
			tokens->instr.sdt.up = add > 0;
			tokens->instr.sdt.offset.offset.imm =
							(uint32_t)abs(add);
		} else {
			tokens->instr.sdt.offset.immediate = true;
			tokens->instr.sdt.offset.offset.reg.rm = add;
		}
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
		case 5:
			tokens->type = 0;
			break;
		}
	}
}
