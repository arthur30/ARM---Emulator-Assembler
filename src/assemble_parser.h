#ifndef ASSEMBLE_PARSER_H
#define ASSEMBLE_PARSER_H

#include "assemble_tokenizer.h"

#include "pi_state.h"

struct instruction {
	char *label;
	bool mnemonic;
	uint8_t type;
	uint8_t cond;
	uint8_t opcode;
	union {
		struct instr_data_proc dpi;
		struct instr_mult mult;
		struct {
			struct instr_transfer sdt;
			uint32_t offset;
		} sdt;
		struct {
			struct instr_branch branch;
			char *jump;
		} branch;
	} instr;
};

int parse(struct token_list *toklist, struct instruction *tokens);

#endif
