#ifndef ASSEMBLE_DICTIONARY_H
#define ASSEMBLE_DICTIONARY_H

uint32_t dpi_to_opcode(char *key);

uint32_t branch_to_cond(char *key);

uint32_t mult_select(char *key);

uint32_t sdt_select(char *key);

uint32_t classify_instr(char *key);

#endif
