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
	{"add", INSTR_DPI_ADD},
	{"and", INSTR_DPI_AND},
	{"cmp", INSTR_DPI_CMP},
	{"eor", INSTR_DPI_EOR},
	{"lsl", INSTR_DPI_LSL},
	{"mov", INSTR_DPI_MOV},
	{"orr", INSTR_DPI_ORR},
	{"rsb", INSTR_DPI_RSB},
	{"sub", INSTR_DPI_SUB},
	{"teq", INSTR_DPI_TEQ},
	{"tst", INSTR_DPI_TST},
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
	{"lsl",   INSTR_TYPE_DATA_PROC},
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
	{"asr", INSTR_SHIFT_ASR},
	{"lsl", INSTR_SHIFT_LSL},
	{"lsr", INSTR_SHIFT_LSR},
	{"ror", INSTR_SHIFT_ROR},
};

struct map dict_cond[] = {
	{"", INSTR_COND_AL},
	{"al", INSTR_COND_AL},
	{"cc", INSTR_COND_CC},
	{"cs", INSTR_COND_CS},
	{"eq", INSTR_COND_EQ},
	{"ge", INSTR_COND_GE},
	{"gt", INSTR_COND_GT},
	{"hi", INSTR_COND_HI},
	{"le", INSTR_COND_LE},
	{"ls", INSTR_COND_LS},
	{"lt", INSTR_COND_LT},
	{"mi", INSTR_COND_MI},
	{"ne", INSTR_COND_NE},
	{"pl", INSTR_COND_PL},
	{"vc", INSTR_COND_VC},
	{"vs", INSTR_COND_VS},
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
	default:
		return -1;
	}
}

uint32_t shift_code(char *key)
{
	return bsearch_map(key, dict_rot, MAP_SIZE(dict_rot));
}

uint32_t classify_instr(char *key)
{
	return bsearch_map(key, dict_all, MAP_SIZE(dict_all));
}

uint32_t classify_cond(char *key)
{
	return bsearch_map(key, dict_cond, MAP_SIZE(dict_cond));
}

