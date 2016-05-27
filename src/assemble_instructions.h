#ifndef ASSEMBLE_INSTRUCTIONS_H
#define ASSEMBLE_INSTRUCTIONS_H

#include <stdint.h>
#include "assemble_tokenizer.h"

uint32_t instr_dpi(struct instruction *instr);

uint32_t instr_multiply(struct instruction *instr);

uint32_t instr_sdt(struct instruction *instr);

uint32_t instr_branch(struct instruction *instr, int off);

#endif
