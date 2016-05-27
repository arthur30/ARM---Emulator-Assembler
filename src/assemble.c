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
			sym_table.table[sym_table.size].label = instr->label;
			sym_table.table[sym_table.size].address = 4 * instr_num;
			sym_table.size++;
		}

		if (strlen(instr->mnemonic) > 1)
			instr_num++;

		free(instr);
	}

	free(line);
}

static void second_pass(void)
{

}

int main(int argc, char **argv)
{
	if (argc != 3)
		return EXIT_FAILURE;

	load_io_files(argv[1], argv[2]);
	initiate_labels();
	first_pass();
	second_pass();

	fclose(input);
	fclose(output);
	destroy_labels();
	return EXIT_SUCCESS;
}

