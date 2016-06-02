#include "assemble_instructions.h"
#include "assemble_dictionary.h"
#include "assemble_parser.h"
#include "assemble_tokenizer.h"

#include "pi_msgs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>

#define SYM_TABLE_CAPACITY	16

static FILE *input;
static FILE *output;

struct sym {
	char *label;
	uint16_t address;
};

struct labels {
	int size;
	int capacity;
	int instr_num;
	struct sym *table;
};

static struct labels sym_table;

struct constants {
	int size;
	int capacity;
	uint32_t *table;
};

static struct constants constant_table;

static int initiate_tables(void)
{
	sym_table.size = 0;
	sym_table.instr_num = 0;
	sym_table.capacity = SYM_TABLE_CAPACITY;
	sym_table.table = calloc(SYM_TABLE_CAPACITY, sizeof(struct sym));

	if (!sym_table.table) {
		fprintf(stderr, PI_ERR_MEM, "labels");
		return -1;
	}

	constant_table.size = 0;
	constant_table.capacity = SYM_TABLE_CAPACITY;
	constant_table.table =
		calloc(SYM_TABLE_CAPACITY, sizeof(uint32_t));

	if (!constant_table.table) {
		fprintf(stderr, PI_ERR_MEM, "constants");
		return -1;
	}

	return 0;
}

static int extend_tables(void)
{
	if (sym_table.size == sym_table.capacity) {
		sym_table.capacity = 2 * sym_table.capacity;
		sym_table.table = realloc(sym_table.table, sym_table.capacity);

		if (!sym_table.table) {
			fprintf(stderr, PI_ERR_MEM, "extending labels");
			return -1;
		}
	}

	if (constant_table.size == constant_table.capacity) {
		constant_table.capacity = 2 * constant_table.capacity;
		constant_table.table =
			realloc(constant_table.table, constant_table.capacity);

		if (!constant_table.table) {
			fprintf(stderr, PI_ERR_MEM, "extending constants");
			return -1;
		}
	}

	return 0;
}

static void destroy_tables(void)
{
	free(sym_table.table);
	free(constant_table.table);
}

static void add_constant(uint32_t constant)
{
	constant_table.table[constant_table.size] = constant;
	constant_table.size++;
}

static int load_io_files(char *inp, char *out)
{
	input = fopen(inp, "r");
	output = fopen(out, "wb");

	if (!input) {
		fprintf(stderr, PI_ERR_INPUT, inp, strerror(errno), errno);
		return -1;
	}

	if (!output) {
		fprintf(stderr, PI_ERR_OUTPUT, out, strerror(errno), errno);
		exit(EXIT_FAILURE);
	}

	return 0;
}

static int lookout_symbol(char *key)
{
	int i = 0;
	const char *cand = sym_table.table[i].label;

	while (cand) {
		if (strcmp(cand, key) == 0)
			return sym_table.table[i].address;
		cand = sym_table.table[++i].label;
	}

	return -1;
}

static void sdt_pc_instr(struct instruction *instr, uint32_t instr_num)
{
	instr->instr.sdt.sdt.rn = 15;
	instr->instr.sdt.sdt.up = instr->instr.sdt.offset > 0;
	instr->instr.sdt.sdt.offset.offset.imm = 4 * (sym_table.instr_num++) -
						(4 * instr_num + 8);
}

static int first_pass(struct token_list *toks)
{
	struct instruction *instr = malloc(sizeof(struct instruction));
	int ret;

	for (;;) {
		memset(instr, 0, sizeof(struct instruction));

		extend_tables();

		ret = parse(toks, instr);
		if (ret == 1)
			break;
		if (ret)
			return -1;

		toks = NULL;

		if (instr->label) {
			sym_table.table[sym_table.size].label = instr->label;
			sym_table.table[sym_table.size].address =
						4 * sym_table.instr_num;
			sym_table.size++;
		}

		if (instr->mnemonic)
			sym_table.instr_num++;

	}

	free(instr);

	return 0;
}

static int second_pass(struct token_list *toks)
{
	uint32_t instr_num = 0;
	int i = 0;
	int ret;

	struct instruction *instr = malloc(sizeof(struct instruction));

	for (;;) {
		int jump_to;
		int current;
		int offset;
		uint32_t instr_binary;

		memset(instr, 0, sizeof(struct instruction));
		extend_tables();

		ret = parse(toks, instr);
		if (ret == 1)
			break;
		if (ret)
			return -1;

		toks = NULL;

		if (instr->mnemonic) {

			switch (instr->type) {
			case INSTR_TYPE_DATA_PROC:
				instr_binary = instr_dpi(instr);
				break;

			case INSTR_TYPE_MULT:
				instr_binary = instr_multiply(instr);
				break;

			case INSTR_TYPE_TRANSFER:
				if (instr->instr.sdt.offset) {
					add_constant(instr->instr.sdt.offset);
					sdt_pc_instr(instr, instr_num);
				}

				instr_binary = instr_sdt(instr);
				break;

			case INSTR_TYPE_BRANCH:
				jump_to = lookout_symbol(
						instr->instr.branch.jump);

				if (jump_to == -1) {
					fprintf(stderr,
						ASS_ERR_JUMP_TARGET,
						instr->instr.branch.jump);
					exit(EXIT_FAILURE);
				}

				current = 4*(instr_num);
				offset = jump_to - (current + 8);
				instr->instr.branch.branch.offset = offset;

				instr_binary = instr_branch(instr);
				break;

			default:
				fprintf(stderr, ASS_ERR_INVALID_INSTR);
				exit(EXIT_FAILURE);
			}

			fwrite(&instr_binary, sizeof(instr_binary), 1, output);
			instr_num++;
		}

	}

	for (i = 0; i < constant_table.size; i++)
		fwrite(&constant_table.table[i], sizeof(uint32_t), 1, output);

	free(instr);

	return 0;

}

int main(int argc, char **argv)
{
	struct token_list *tokens;

	if (argc != 3) {
		fprintf(stderr, ASS_ERR_ARGS);
		goto fail;
	}

	if (load_io_files(argv[1], argv[2]))
		goto fail;

	tokens = malloc(sizeof(struct token_list));
	if (!tokens) {
		fprintf(stderr, PI_ERR_MEM, "tokens");
		goto fail;
	}
	if (token_list_alloc(tokens))
		goto fail;
	if (tokenize(input, tokens)) {
		fprintf(stderr, ASS_ERR_TOKENIZE);
		goto fail;
	}
	fclose(input);

	if (initiate_tables())
		goto fail;

	if (first_pass(tokens)) {
		fprintf(stderr, ASS_ERR_PASS1);
		goto fail;
	}
	if (second_pass(tokens)) {
		fprintf(stderr, ASS_ERR_PASS2);
		goto fail;
	}

	destroy_tables();
	fclose(output);

	return EXIT_SUCCESS;

fail:
	return EXIT_FAILURE;
}

