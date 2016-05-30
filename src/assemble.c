#include "assemble_instructions.h"
#include "assemble_tokenizer.h"
#include "assemble_dictionary.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 512
#define SYM_TABLE_CAPACITY 16

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
} sym_table;

struct constants {
	int size;
	int capacity;
	uint32_t *table;
} constant_table;

static void initiate_labels(void)
{
	sym_table.size = 0;
	sym_table.instr_num = 0;
	sym_table.capacity = SYM_TABLE_CAPACITY;
	sym_table.table = calloc(SYM_TABLE_CAPACITY, sizeof(struct sym));

	if (!sym_table.table) {
		fprintf(stderr, "Couldn't allocate memory for labels.");
		exit(EXIT_FAILURE);
	}
}

static void extend_labels(void)
{
	if (sym_table.size == sym_table.capacity) {
		sym_table.capacity = 2 * sym_table.capacity;
		sym_table.table = realloc(sym_table.table, sym_table.capacity);

		if (!sym_table.table)
			fprintf(stderr, "Coudln't extend memory for labels");
	}
}

static void destroy_tables(void)
{
	free(sym_table.table);
	free(constant_table.table);
}

static void initiate_constants(void)
{
	constant_table.size = 0;
	constant_table.capacity = SYM_TABLE_CAPACITY;
	constant_table.table =
		calloc(SYM_TABLE_CAPACITY, sizeof(uint32_t));

	if (!constant_table.table) {
		fprintf(stderr, "Couldn't allocate memory for constants.");
		exit(EXIT_FAILURE);
	}
}

static void extend_constants(void)
{
	if (constant_table.size == constant_table.capacity) {
		constant_table.capacity = 2 * constant_table.capacity;
		constant_table.table =
			realloc(constant_table.table, constant_table.capacity);

		if (!constant_table.table)
			fprintf(stderr, "Coudln't extend memory for labels");
	}
}

static void add_constant(uint32_t constant)
{
	constant_table.table[constant_table.size] = constant;
	constant_table.size++;
}

static void load_io_files(char *inp, char *out)
{
	input = fopen(inp, "r");
	output = fopen(out, "wb");

	if (input == NULL) {
		printf("Error opening the input file");
		exit(EXIT_FAILURE);
	}

	if (output == NULL) {
		printf("Error opening the output file");
		exit(EXIT_FAILURE);
	}
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
	instr->instr.sdt.rn = 15;
	instr->instr.sdt.up = instr->sdt_offset > 0;
	instr->instr.sdt.offset.offset.imm = 4 * (sym_table.instr_num++) -
						(4 * instr_num + 8);
}

static void first_pass(void)
{
	char *line = malloc(MAX_LINE_LENGTH);

	if (!line) {
		fprintf(stderr, "Couldn't allocate memory for the line.\n");
		exit(EXIT_FAILURE);
	}

	while (fgets(line, MAX_LINE_LENGTH, input)) {

		struct instruction *instr = malloc(sizeof(struct instruction));

		extend_labels();
		tokenize(line, instr);


		if (instr->label) {
			sym_table.table[sym_table.size].label =
						strcat(instr->label, "\n");
			sym_table.table[sym_table.size].address =
						4 * sym_table.instr_num;
			sym_table.size++;
		}

		if (instr->mnemonic)
			sym_table.instr_num++;

		free(instr);
	}

	free(line);
}

static void second_pass(void)
{
	char *line = malloc(MAX_LINE_LENGTH);
	uint32_t instr_num = 0;
	int i = 0;

	if (!line) {
		fprintf(stderr, "Couldn't allocate memory for the line.\n");
		exit(EXIT_FAILURE);
	}

	rewind(input);

	struct instruction *instr = calloc(1, sizeof(struct instruction));

	while (fgets(line, MAX_LINE_LENGTH, input)) {
		int jump_to;
		int current;
		int offset;
		uint32_t instr_binary;

		memset(instr, 0, sizeof(struct instruction));
		extend_constants();
		tokenize(line, instr);


		if (instr->mnemonic) {

			switch (instr->type) {
			case 0:
				instr_binary = instr_dpi(instr);
				break;
			case 1:
				instr_binary = instr_multiply(instr);
				break;
			case 2:
				if (instr->sdt_offset) {
					add_constant(instr->sdt_offset);
					sdt_pc_instr(instr, instr_num);
				}

				instr_binary = instr_sdt(instr);
				break;
			case 3:
				jump_to = lookout_symbol(instr->jump);

				if (jump_to == -1) {
					fprintf(stderr, "Invlid label jump.");
					exit(EXIT_FAILURE);
				}

				current = 4*(instr_num);
				offset = jump_to - (current + 8);
				instr->instr.branch.offset = offset;

				instr_binary = instr_branch(instr);
				break;
			default:
				fprintf(stderr, "Error: Invalid Instruction");
				exit(EXIT_FAILURE);
			}

			fwrite(&instr_binary, sizeof(instr_binary), 1, output);
			instr_num++;
		 }

	}

	for (i = 0; i < constant_table.size; i++)
		fwrite(&constant_table.table[i], sizeof(uint32_t), 1, output);

	free(instr);

}



int main(int argc, char **argv)
{
	if (argc != 3)
		return EXIT_FAILURE;

	load_io_files(argv[1], argv[2]);

	initiate_labels();
	initiate_constants();

	first_pass();
	second_pass();

	destroy_tables();
	fclose(input);
	fclose(output);

	return EXIT_SUCCESS;
}

