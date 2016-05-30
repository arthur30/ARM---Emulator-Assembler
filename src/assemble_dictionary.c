#include "assemble_dictionary.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct map {
	const char *str;
	int n;
};

struct map dict_dpi[] = {
	{"and", 0},
	{"andeq", 5},
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

struct map dict_mult[] = {
	{"mul", 0},
	{"mla", 1},
	{0, 0}
};

struct map dict_sdt[] = {
	{"ldr", 0},
	{"str", 1},
	{0, 0}
};

struct map dict_all[] = {
	{"and", 0},
	{"eor", 0},
	{"sub", 0},
	{"rsb", 0},
	{"add", 0},
	{"orr", 0},
	{"mov", 0},
	{"tst", 0},
	{"teq", 0},
	{"cmp", 0},
	{"mul", 1},
	{"mla", 1},
	{"ldr", 2},
	{"str", 2},
	{"beq", 3},
	{"bne", 3},
	{"bge", 3},
	{"blt", 3},
	{"bgt", 3},
	{"ble", 3},
	{"b", 3},
	{"bal", 3},
	{"lsl", 4},
	{"andeq", 5},
	{0, 0}
};

struct map dict_rot[] = {
	{"lsl", 0},
	{"lsr", 1},
	{"asr", 2},
	{"ror", 3},
	{0, 0}
};

static uint32_t key_to_int(struct map *dict, char *key)
{
	int i = 0;
	const char *cand = dict[i].str;

	while (cand) {
		if (strcmp(cand, key) == 0)
			return dict[i].n;
		cand = dict[++i].str;
	}

	return -1;
}

uint32_t instr_code(char *key, int type)
{
	switch (type) {
	case 0:
		return key_to_int(dict_dpi, key);
	case 1:
		return key_to_int(dict_mult, key);
	case 2:
		return key_to_int(dict_sdt, key);
	case 3:
		return key_to_int(dict_branch, key);
	case 5:
		return key_to_int(dict_dpi, key);
	case 6:
		return key_to_int(dict_rot, key);
	default:
		return -1;
	}
}

uint32_t classify_instr(char *key)
{
	return key_to_int(dict_all, key);
}
