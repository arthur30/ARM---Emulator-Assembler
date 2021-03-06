#include "assemble_parser.h"
#include "assemble_dictionary.h"
#include "assemble_tokenizer.h"

#include "pi_msgs.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SHIFT_BIT_SIZE		(1 << 4)
#define IMM_BIT_SIZE		(1 << 8)

/* These globals are justified,
 * since parse() used in a similar fashion as strtok
 * First a non-null token list is passed in then
 * on next invocations a NULL value should be passed
 * to continue parsing the same token stream
 */

static struct token_list *toks;
static size_t tokens_position;
static struct token *tok;

static struct token *nexttok(void)
{
	return tok = token_list_getnext(toks, &tokens_position);
}

static bool tok_is_string(void)
{
	return tok->type == TOKEN_TYPE_STRING;
}

static bool tok_is_number(void)
{
	return tok->type == TOKEN_TYPE_NUMBER;
}

static bool tok_is_bracket_open(void)
{
	return tok->type == TOKEN_TYPE_BRACKET_OPEN;
}

static bool tok_is_bracket_close(void)
{
	return tok->type == TOKEN_TYPE_BRACKET_CLOSE;
}

static bool tok_is_newline(void)
{
	return tok->type == TOKEN_TYPE_NEWLINE;
}

static bool tok_is_reg(void)
{
	return tok->type == TOKEN_TYPE_STRING && tok->str[0] == 'r';
}

static bool tok_is_reg_or_number(void)
{
	return tok_is_reg() || tok_is_number();
}

static uint8_t tok_get_reg(void)
{
	return atoi(tok->str + 1);
}

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

static int init_lsl(struct instruction *tokens)
{
	if (!nexttok() || !tok_is_reg())
		return -1;

	tokens->type = INSTR_TYPE_DATA_PROC;
	tokens->opcode = INSTR_DPI_MOV;
	tokens->instr.dpi.rd = tok_get_reg();
	tokens->instr.dpi.op2.immediate = false;
	tokens->instr.dpi.op2.offset.reg.rm = tokens->instr.dpi.rd;
	tokens->instr.dpi.op2.offset.reg.shift_type = 0;
	tokens->instr.dpi.op2.offset.reg.constant = 0;

	if (!nexttok() || !tok_is_number())
		return -1;

	tokens->instr.dpi.op2.offset.reg.amount.integer = tok->num;

	return 0;
}

static int init_dpi(struct instruction *tokens)
{
	uint8_t imm;
	uint8_t shift;

	tokens->instr.dpi.opcode = tokens->opcode;
	tokens->instr.dpi.setcond = false;

	switch (tokens->opcode) {
	case INSTR_DPI_TST:
	case INSTR_DPI_TEQ:
	case INSTR_DPI_CMP:
		tokens->instr.dpi.setcond = true;
		tokens->instr.dpi.rd = 0;
		if (!nexttok() || !tok_is_reg())
			return -1;
		tokens->instr.dpi.rn = tok_get_reg();
		break;

	case INSTR_DPI_MOV:
		tokens->instr.dpi.rn = 0;
		if (!nexttok() || !tok_is_reg())
			return -1;
		tokens->instr.dpi.rd = tok_get_reg();
		break;

	case INSTR_DPI_LSL:
		return init_lsl(tokens);

	default:
		if (!nexttok() || !tok_is_reg())
			return -1;
		tokens->instr.dpi.rd = tok_get_reg();
		if (!nexttok() || !tok_is_reg())
			return -1;
		tokens->instr.dpi.rn = tok_get_reg();
		break;
	}

	if (!nexttok())
		return -1;

	tokens->instr.dpi.op2.immediate = tok_is_number();

	if (tokens->instr.dpi.op2.immediate) {
		if (generate_op2(tok->num, &imm, &shift)) {
			fprintf(stderr, ASS_ERR_OP2_FIT, tok->num);
			return -1;
		}
		tokens->instr.dpi.op2.offset.imm.imm = imm;
		tokens->instr.dpi.op2.offset.imm.rotate = shift;
	} else {
		if (!tok_is_reg())
			return -1;
		tokens->instr.dpi.op2.offset.reg.rm = tok_get_reg();

		if (!nexttok() || !tok_is_newline()) {
			if (!tok_is_string())
				return -1;
			tokens->instr.dpi.op2.offset.reg.shift_type =
						classify_shift(tok->str);
			tokens->instr.dpi.op2.offset.reg.constant = true;

			if (!nexttok())
				return -1;
			if (!tok_is_reg_or_number())
				return -1;

			if (tok_is_number()) {
				tokens->instr.dpi.op2.offset.reg.constant =
									false;
				tokens->instr.dpi.op2.offset.reg.amount.integer
						= tok->num;
			} else {
				tokens->instr.dpi.op2.offset.reg.amount.rs =
						tok_get_reg();
			}
		} else {
			return 0;
		}
	}

	if (!nexttok() || !tok_is_newline())
		return -1;

	return 0;
}

static int init_mult(struct instruction *tokens)
{
	tokens->instr.mult.accumulate = tokens->opcode;
	tokens->instr.mult.setcond = false;

	if (!nexttok() || !tok_is_reg())
		return -1;
	tokens->instr.mult.rd = atoi(tok->str + 1);

	if (!nexttok() || !tok_is_reg())
		return -1;
	tokens->instr.mult.rm = atoi(tok->str + 1);

	if (!nexttok() || !tok_is_reg())
		return -1;
	tokens->instr.mult.rs = atoi(tok->str + 1);

	if (tokens->opcode) {
		if (!nexttok() || !tok_is_reg())
			return -1;
		tokens->instr.mult.rn = atoi(tok->str + 1);
	}

	if (!nexttok() || !tok_is_newline())
		return -1;

	return 0;
}

static int init_sdt(struct instruction *tokens)
{
	int8_t rd;
	uint8_t imm;
	uint8_t shift;

	tokens->instr.sdt.sdt.load = !tokens->opcode;
	tokens->instr.sdt.sdt.up = true;

	if (!nexttok() || !tok_is_reg())
		return -1;

	rd = tok_get_reg();

	tokens->instr.sdt.sdt.rd = rd;
	tokens->instr.sdt.sdt.preindexing = true;
	tokens->instr.sdt.sdt.offset.immediate = false;
	tokens->instr.sdt.offset = 0;

	if (!nexttok())
		return -1;

	if (tok_is_number()) {
		if (!tokens->instr.sdt.sdt.load) {
			fprintf(stderr, ASS_ERR_STR_IMM);
			return -1;
		}

		if (generate_op2(tok->num, &imm, &shift)) {
			tokens->instr.sdt.offset = tok->num;
			tokens->instr.sdt.sdt.offset.immediate = false;
		} else {
			tokens->type = INSTR_TYPE_DATA_PROC;
			tokens->opcode = INSTR_DPI_MOV;
			tokens->instr.dpi.rn = 0;
			tokens->instr.dpi.rd = rd;
			tokens->instr.dpi.setcond = false;
			tokens->instr.dpi.op2.immediate = true;
			tokens->instr.dpi.op2.offset.imm.imm = imm;
			tokens->instr.dpi.op2.offset.imm.rotate = shift;
		}

		if (!nexttok() || !tok_is_newline())
			return -1;

		return 0;
	}

	if (!tok_is_bracket_open())
		return -1;

	if (!nexttok() || !tok_is_reg())
		return -1;

	tokens->instr.sdt.sdt.rn = tok_get_reg();

	if (!nexttok())
		return -1;

	if (tok_is_number()) {
		tokens->instr.sdt.sdt.up = tok->num > 0;
		tokens->instr.sdt.sdt.offset.offset.imm
						= (uint32_t)labs(tok->num);

		if (!nexttok() || !tok_is_bracket_close())
			return -1;
	} else if (tok_is_reg()) {
		tokens->instr.sdt.sdt.offset.immediate = true;
		tokens->instr.sdt.sdt.offset.offset.reg.rm = tok_get_reg();

		if (!nexttok())
			return -1;

		if (tok_is_string()) {
			tokens->instr.sdt.sdt.offset.offset.reg.shift_type =
						instr_code(tok->str, 6);
			tokens->instr.sdt.sdt.offset.offset.reg.shift_type =
						classify_shift(tok->str);

			if (!nexttok())
				return -1;

			if (tok_is_number()) {
				tokens->instr.sdt.sdt.offset.offset.
							reg.constant = false;
				tokens->instr.sdt.sdt.offset.offset.reg.
					amount.integer = tok->num;
			} else if (tok_is_reg()) {
				tokens->instr.sdt.sdt.offset.offset.
							reg.constant = true;
				tokens->instr.sdt.sdt.offset.offset.reg.
					amount.rs = tok_get_reg();
			} else {
				return -1;
			}

			if (!nexttok() || !tok_is_bracket_close())
				return -1;
		} else if (tok_is_bracket_close()) {
			;
		} else {
			return -1;
		}
	} else if (tok_is_bracket_close()) {
		;
	} else {
		return -1;
	}

	if (!nexttok())
		return -1;

	if (tok_is_newline())
		return 0;

	tokens->instr.sdt.sdt.preindexing = false;

	if (tok_is_number()) {
		tokens->instr.sdt.sdt.up = tok->num > 0;
		tokens->instr.sdt.sdt.offset.offset.imm = (uint32_t)tok->num;
	} else if (tok_is_reg()) {
		tokens->instr.sdt.sdt.offset.immediate = true;
		tokens->instr.sdt.sdt.offset.offset.reg.rm = tok_get_reg();
	}

	if (!nexttok() || !tok_is_newline())
		return -1;

	return 0;
}

static int init_branch(struct instruction *tokens)
{
	if (!nexttok() || tok->type != TOKEN_TYPE_STRING)
		return -1;

	tokens->instr.branch.jump = tok->str;

	if (!nexttok() || tok->type != TOKEN_TYPE_NEWLINE)
		return -1;

	return 0;
}

/* return values:
 * -1 on error
 *  0 on ok
 *  1 on all tokens consumed
 */
int parse(struct token_list *toklist, struct instruction *tokens)
{
	int ret;
	char *instr;

	instr = NULL;

	if (toklist) {
		ret = 0;
		toks = toklist;
		tokens_position = 0;
	}

	if (!toks)
		return -1;

	if (!nexttok())
		return 1;

	if (tok->type == TOKEN_TYPE_NEWLINE)
		return 0;

	if (tok->type == TOKEN_TYPE_STRING &&
			tok->str[tok->strlen - 1] == ':') {
		tokens->label = strndup(tok->str, tok->strlen - 1);
		if (!nexttok())
			goto fail;
	}


	tokens->mnemonic = false;

	if (tok) {
		if (tok->type == TOKEN_TYPE_NEWLINE)
			return 0;

		tokens->mnemonic = true;
		instr = strndup(tok->str, 3);
		tokens->type = classify_instr(instr);
		tokens->opcode = instr_code(instr, tokens->type);
		tokens->cond = classify_cond(tok->str + 3);

		switch (tokens->type) {
		case INSTR_TYPE_DATA_PROC:
			ret = init_dpi(tokens);
			break;
		case INSTR_TYPE_MULT:
			ret = init_mult(tokens);
			break;
		case INSTR_TYPE_TRANSFER:
			ret = init_sdt(tokens);
			break;
		case INSTR_TYPE_BRANCH:
			tokens->cond = classify_cond(tok->str + 1);
			ret = init_branch(tokens);
			break;
		}

		if (ret)
			goto fail;
	}

	free(instr);

	return 0;

fail:

	free(instr);

	if (tok)
		fprintf(stderr, ASS_ERR_PARSE_NEAR, tok->lineno, tok->colno);
	else
		fprintf(stderr, ASS_ERR_PARSE_EOF);
	return -1;
}
