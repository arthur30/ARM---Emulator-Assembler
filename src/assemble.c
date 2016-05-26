#include "assemble_instructions.h"
#include "assemble_tokenizer.h"
#include "assemble_dictionary.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_LINE_LENGTH 512

struct sym {
	char *label;
	uint16_t address;
};


/*static int print_bits(uint32_t x)
*{
*	int i;
*	uint32_t mask = 1 << 31;
*
*	for (i = 0; i < 32; i++) {
*		printf("%i", (x & mask) != 0);
*		x <<= 1;
*	}
*	printf("\n");
*
*	return 1;
*}
*/

int main(int argc, char **argv)
{
	FILE *input;
	FILE *output;

	/* Symbol Table ADT */
	int sym_table_size = 16;
	struct sym *sym_table = calloc(sym_table_size, sizeof(struct sym));

	if (!sym_table) {
		fprintf(stderr, "Couldn't allocate space to sym table.");
		exit(EXIT_FAILURE);
	}

	/* Check if source and dest files are provided.	 */
	if (argc < 3)
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

	/* char str[] = "bal a";
	 * char *ptr = str;
	 *
	 * struct instruction instr = tokenize(ptr);
	 *
	 * if (instr.label == NULL)
	 *	print_bits(instr_branch(instr, -16));
	 */

	char *line = malloc(MAX_LINE_LENGTH);
	int instr_num = 0;

	if (!line) {
		fprintf(stderr, "Couldn't allocate memory for the line.\n");
		exit(EXIT_FAILURE);
	}

	/* FIRST PASS */
	while (fgets(line, MAX_LINE_LENGTH, input)) {

		printf("%s", line);

		/*		if (instr_num % 16 == 0)
		 *	sym_table = realloc(sym_table,
		 *			2*instr_num*sizeof(struct sym));
		 */
		struct instruction instr = tokenize(line);

		if (instr.label && instr.mnemonic) {
			sym_table[instr_num].label = instr.label;
			sym_table[instr_num].address =  4*instr_num;
			instr_num++;
		} else if (instr.label) {
			sym_table[instr_num].label = instr.label;
			sym_table[instr_num].address =  4*(instr_num + 1);
		} else if (instr.mnemonic) {
			instr_num++;
		}

	}


	free(line);
	fclose(input);
	fclose(output);
	free(sym_table);
	return EXIT_SUCCESS;
}

