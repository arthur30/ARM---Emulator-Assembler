#include "assemble_dictionary.h"

#include "pi_state.h"

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
	{"and",   INSTR_TYPE_DATA_PROC},
	{"eor",   INSTR_TYPE_DATA_PROC},
	{"sub",   INSTR_TYPE_DATA_PROC},
	{"rsb",   INSTR_TYPE_DATA_PROC},
	{"add",   INSTR_TYPE_DATA_PROC},
	{"orr",   INSTR_TYPE_DATA_PROC},
	{"mov",   INSTR_TYPE_DATA_PROC},
	{"tst",   INSTR_TYPE_DATA_PROC},
	{"teq",   INSTR_TYPE_DATA_PROC},
	{"cmp",   INSTR_TYPE_DATA_PROC},
	{"mul",   INSTR_TYPE_MULT},
	{"mla",   INSTR_TYPE_MULT},
	{"ldr",   INSTR_TYPE_TRANSFER},
	{"str",   INSTR_TYPE_TRANSFER},
	{"beq",   INSTR_TYPE_BRANCH},
	{"bne",   INSTR_TYPE_BRANCH},
	{"bge",   INSTR_TYPE_BRANCH},
	{"blt",   INSTR_TYPE_BRANCH},
	{"bgt",   INSTR_TYPE_BRANCH},
	{"ble",   INSTR_TYPE_BRANCH},
	{"b",     INSTR_TYPE_BRANCH},
	{"bal",   INSTR_TYPE_BRANCH},
	{"lsl",   5},
	{"andeq", INSTR_TYPE_HALT},
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
	case INSTR_TYPE_HALT:
		return key_to_int(dict_dpi, key);
	case INSTR_TYPE_DATA_PROC:
		return key_to_int(dict_dpi, key);
	case INSTR_TYPE_MULT:
		return key_to_int(dict_mult, key);
	case INSTR_TYPE_TRANSFER:
		return key_to_int(dict_sdt, key);
	case INSTR_TYPE_BRANCH:
		return key_to_int(dict_branch, key);
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

