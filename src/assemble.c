#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "instructions.h"

struct symbol {
	char *label;
	uint16_t address;
};

int numLines(FILE *file)
{
	char ch;
	int num_of_lines = 0;

	do {
		ch = fgetc(file);
		if (ch == '\n')
			num_of_lines++;
	} while (ch != EOF);

	if (ch != '\n' && num_of_lines != 0)
		num_of_lines++;

	return num_of_lines;
}

int main(int argc, char **argv)
{
	FILE *input;
	FILE *output;

	/* Symbol Table ADT */
	int symbol_table_size = 16;
	struct symbol *symbol_table = malloc(symbol_table_size * sizeof(struct symbol));

	/* Check if source and dest files are provided.	 */
	if (argc < 3)
		return EXIT_FAILURE;

	/* Open the input and output files */
	input = fopen(argv[0], "r");
	output = fopen(argv[1], "wb");

	/* Handle errors if files don't exist */
	if (input == NULL) {
		perror("Error opening the input file");
		return EXIT_FAILURE;
	}

	if (output == NULL) {
		perror("Error opening the output file");
		return EXIT_FAILURE;
	}

	

	free(symbol_table);
	return EXIT_SUCCESS;
}
