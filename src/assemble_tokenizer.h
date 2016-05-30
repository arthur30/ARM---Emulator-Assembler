#ifndef ASSEMBLE_TOKENIZER_H
#define ASSEMBLE_TOKENIZER_H

#include "emulate_pi_state.h"

struct instruction {
	char *label;
	char *jump;
	bool mnemonic;
	uint8_t type;
	uint8_t code;
	uint32_t sdt_offset;
	union {
		struct instr_data_proc dpi;
		struct instr_mult mult;
		struct instr_transfer sdt;
		struct instr_branch branch;
	} instr;
};

void tokenize(char *instr, struct instruction *tokens);

#endif
