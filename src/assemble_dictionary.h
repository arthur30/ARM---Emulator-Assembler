#ifndef ASSEMBLE_DICTIONARY_H
#define ASSEMBLE_DICTIONARY_H

#include <stdint.h>

uint32_t instr_code(char *key, int type);

uint32_t classify_instr(char *key);

uint32_t classify_cond(char *key);

#endif
