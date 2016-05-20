#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "instructions.h"

struct sym {
	char *label;
	uint16_t address;
};

int main(int argc, char **argv)
{
	FILE *input;
	FILE *output;

	/* Symbol Table ADT */
	int sym_table_size = 16;
	struct sym *sym_table = calloc(sym_table_size, sizeof(struct sym));

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



	free(sym_table);
	return EXIT_SUCCESS;
}
