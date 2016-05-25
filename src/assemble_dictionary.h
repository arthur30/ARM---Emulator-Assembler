#ifndef ASSEMBLE_DICTIONARY_H
#define ASSEMBLE_DICTIONARY_H

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

uint32_t dpi_to_opcode(char *key);

uint32_t branch_to_cond(char *key);

#endif
