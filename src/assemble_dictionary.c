#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "assemble_dictionary.h"

struct map {
	const char *str;
	int n;
};

struct map dict_dpi[] = {
	{"and", 0},
	{"eor", 1},
	{"sub", 2},
	{"rsb", 3},
	{"add", 4},
	{"orr", 12},
	{"mov", 13},
	{"tst", 8},
	{"teq", 9},
	{"cmp", 10},
	{"mul", 20},
	{"mla", 21},
	{0, 0}
};

struct map dict_branch[] = {
	{"beq", 0},
	{"bne", 1},
	{"bge", 10},
	{"blt", 11},
	{"bgt", 12},
	{"ble", 13},
	{"b", 14},
	{"bal", 14},
	{0, 0}
};



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
