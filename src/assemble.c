#include "assemble_instructions.h"
#include "assemble_tokenizer.h"
#include "assemble_dictionary.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 512
#define SYM_TABLE_CAPACITY 16

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

int main(int argc, char **argv)
{
	FILE *input;
	FILE *output;

	/* Check if source and dest files are provided.	 */
	if (argc != 3)
		return EXIT_FAILURE;

	/* Open the input and output files */
	input = fopen(argv[1], "r");
	output = fopen(argv[2], "wb");

	/* Handle errors if files don't exist */
	if (input == NULL) {
		printf("Error opening the input file");
		return EXIT_FAILURE;
	}

	if (output == NULL) {
		printf("Error opening the output file");
		return EXIT_FAILURE;
	}

	initiate_labels();

	char *line = malloc(MAX_LINE_LENGTH);
	int instr_num = 0;

	if (!line) {
		fprintf(stderr, "Couldn't allocate memory for the line.\n");
		exit(EXIT_FAILURE);
	}

	/* FIRST PASS */
	while (fgets(line, MAX_LINE_LENGTH, input)) {

		extend_labels();

		struct instruction *instr = malloc(sizeof(struct instruction));

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

	int i = 0;

	for (i = 0; i < sym_table.size; i++) {
		printf("%s\t%i\n", sym_table.table[i].label,
				sym_table.table[i].address);
	}

	free(line);
	fclose(input);
	fclose(output);
	destroy_labels();
	return EXIT_SUCCESS;
}

