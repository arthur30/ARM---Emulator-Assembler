#ifndef ASSEMBLE_INSTRUCTIONS_H
#define ASSEMBLE_INSTRUCTIONS_H

#include <stdint.h>

uint32_t key_to_int(char *key, int dpi);

uint32_t instr_dpi(void);

uint32_t instr_multiply(void);

uint32_t instr_sdt(void);

uint32_t instr_branch(void);

#endif
