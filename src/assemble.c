#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "assemble_instructions.h"
#include "assemble_tokenizer.h"

struct sym {
	char *label;
	uint16_t *address;
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
		printf("Error opening the input file");
		return EXIT_FAILURE;
	}

	if (output == NULL) {
		printf("Error opening the output file");
		return EXIT_FAILURE;
	}

	char str[] = "label: mla r0,r2,r3,r4";
	char str2[] = "label mla r0,r2,r3,r4";
	char *ptr = str;
	char *ptr2 = str2;

	struct instruction tokens = tokenize(ptr2);

	printf("%s\n", tokens.label);
	printf("%s\n", tokens.mnemonic);
	printf("%s\n", tokens.op1);
	printf("%s\n", tokens.op2);
	printf("%s\n", tokens.op3);
	printf("%s\n", tokens.op4);
	tokenize(ptr);
	free(sym_table);
	return EXIT_SUCCESS;
}
