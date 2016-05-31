#ifndef ASSEMBLE_PARSER_H
#define ASSEMBLE_PARSER_H

#include "assemble_tokenizer.h"

#include "pi_state.h"

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

int parse(struct token_list *toklist, struct instruction *tokens);

#endif
