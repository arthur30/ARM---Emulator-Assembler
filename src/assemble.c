#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "assemble_instructions.h"
#include "assemble_tokenizer.h"


struct sym {
	char *label;
	uint16_t *address;
};


static int print_bits(uint32_t x)
{
	int i;
	uint32_t mask = 1 << 31;

	for (i = 0; i < 32; i++) {
		printf("%i", (x & mask) != 0);
		x <<= 1;
	}
	printf("\n");

	return 1;
}

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
		printf("Error opening the input file");
		return EXIT_FAILURE;
	}

	if (output == NULL) {
		printf("Error opening the output file");
		return EXIT_FAILURE;
	}

	char str[] = "mla r0,r1,r2,r3";
	char *ptr = str;

	print_bits(instr_multiply(tokenize(ptr)));
	free(sym_table);
	return EXIT_SUCCESS;
}

