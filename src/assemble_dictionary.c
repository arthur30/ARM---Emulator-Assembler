#include "assemble_dictionary.h"

#include "pi_state.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAP_SIZE(dict) (sizeof(dict) / sizeof(struct map))

struct map {
	const char *str;
	int n;
};

struct map dict_dpi[] = {
	{"add", 4},
	{"and", 0},
	{"andeq", 5},
	{"cmp", 10},
	{"eor", 1},
	{"mov", 13},
	{"orr", 12},
	{"rsb", 3},
	{"sub", 2},
	{"teq", 9},
	{"tst", 8},
};

struct map dict_mult[] = {
	{"mla", 1},
	{"mul", 0},
};

struct map dict_sdt[] = {
	{"ldr", 0},
	{"str", 1},
};

struct map dict_all[] = {
	{"add",   INSTR_TYPE_DATA_PROC},
	{"and",   INSTR_TYPE_DATA_PROC},
	{"b",     INSTR_TYPE_BRANCH},
	{"bal",   INSTR_TYPE_BRANCH},
	{"beq",   INSTR_TYPE_BRANCH},
	{"bge",   INSTR_TYPE_BRANCH},
	{"bgt",   INSTR_TYPE_BRANCH},
	{"ble",   INSTR_TYPE_BRANCH},
	{"blt",   INSTR_TYPE_BRANCH},
	{"bne",   INSTR_TYPE_BRANCH},
	{"cmp",   INSTR_TYPE_DATA_PROC},
	{"eor",   INSTR_TYPE_DATA_PROC},
	{"ldr",   INSTR_TYPE_TRANSFER},
	{"lsl",   5},
	{"mla",   INSTR_TYPE_MULT},
	{"mov",   INSTR_TYPE_DATA_PROC},
	{"mul",   INSTR_TYPE_MULT},
	{"orr",   INSTR_TYPE_DATA_PROC},
	{"rsb",   INSTR_TYPE_DATA_PROC},
	{"str",   INSTR_TYPE_TRANSFER},
	{"sub",   INSTR_TYPE_DATA_PROC},
	{"teq",   INSTR_TYPE_DATA_PROC},
	{"tst",   INSTR_TYPE_DATA_PROC},
};

struct map dict_rot[] = {
	{"asr", 2},
	{"lsl", 0},
	{"lsr", 1},
	{"ror", 3},
};

struct map dict_cond[] = {
	{"", 14},
	{"al", 14},
	{"cc", 3},
	{"cs", 2},
	{"eq", 0},
	{"ge", 10},
	{"gt", 12},
	{"hi", 8},
	{"le", 13},
	{"ls", 9},
	{"lt", 11},
	{"mi", 4},
	{"ne", 1},
	{"pl", 5},
	{"vc", 7},
	{"vs", 6},
};

static int map_compar(const void *key, const void *map_elem)
{
	return strcmp((char *)key, ((struct map *)map_elem)->str);
}

static uint32_t bsearch_map(const void *key, const void *base, size_t nmemb)
{
	struct map *elem;

	elem = bsearch(key, base, nmemb, sizeof(struct map), map_compar);
	if (elem)
		return elem->n;
	return -1;
}

uint32_t instr_code(char *key, int type)
{
	switch (type) {
	case INSTR_TYPE_DATA_PROC:
		return bsearch_map(key, dict_dpi, MAP_SIZE(dict_dpi));
	case INSTR_TYPE_MULT:
		return bsearch_map(key, dict_mult, MAP_SIZE(dict_mult));
	case INSTR_TYPE_TRANSFER:
		return bsearch_map(key, dict_sdt, MAP_SIZE(dict_sdt));
	case INSTR_TYPE_BRANCH:
		return -1;
	case 6:
		return bsearch_map(key, dict_rot, MAP_SIZE(dict_rot));
	default:
		return -1;
	}
}

uint32_t classify_instr(char *key)
{
	return bsearch_map(key, dict_all, MAP_SIZE(dict_all));
}

uint32_t classify_cond(char *key)
{
	return bsearch_map(key, dict_cond, MAP_SIZE(dict_cond));
}

