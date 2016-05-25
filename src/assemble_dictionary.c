#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "assemble_dictionary.h"

uint32_t dpi_to_opcode(char *key)
{
	int i = 0;
	struct map *dict = dict_dpi;

	const char *cand = dict[i].str;

	while (cand) {
		if (strcmp(cand, key) == 0)
			return dict[i].n;
		cand = dict[++i].str;
	}

	return -1;
}

uint32_t branch_to_cond(char *key)
{
	int i = 0;
	struct map *dict = dict_branch;

	const char *cand = dict[i].str;

	while (cand) {
		if (strcmp(cand, key) == 0)
			return dict[i].n;
		cand = dict[++i].str;
	}

	return -1;
}
