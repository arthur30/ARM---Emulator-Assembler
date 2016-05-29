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
	struct sym *table;
} sym_table;

static void initiate_labels(void)
{
	sym_table.size = 0;
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

static void destroy_labels(void)
{
	free(sym_table.table);
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

static void first_pass(void)
{
	char *line = malloc(MAX_LINE_LENGTH);
	int instr_num = 0;

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
			sym_table.table[sym_table.size].address = 4 * instr_num;
			sym_table.size++;
		}

		if (instr->mnemonic)
			instr_num++;

		free(instr);
	}

	free(line);
}

static void second_pass(void)
{
	char *line = malloc(MAX_LINE_LENGTH);
	int instr_num = 0;

	if (!line) {
		fprintf(stderr, "Couldn't allocate memory for the line.\n");
		exit(EXIT_FAILURE);
	}

	rewind(input);

	while (fgets(line, MAX_LINE_LENGTH, input)) {
		struct instruction *instr =
			calloc(1, sizeof(struct instruction));
		int jump_to;
		int current;
		int offset;
		uint32_t instr_binary;

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
				/* Handle the variable placement. */
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

		free(instr);
	}

}



int main(int argc, char **argv)
{
	if (argc != 3)
		return EXIT_FAILURE;

	load_io_files(argv[1], argv[2]);

	initiate_labels();

	first_pass();
	second_pass();

	destroy_labels();
	fclose(input);
	fclose(output);

	return EXIT_SUCCESS;
}

