#ifndef ASSEMBLE_PARSER_H
#define ASSEMBLE_PARSER_H

#include "emulate_pi_state.h"
#include "assemble_tokenizer.h"

struct instr_data_proc parse_dpi(struct instruction instr);

struct instr_mult parse_mult(struct instruction instr);

struct instr_transfer parse_sdt(struct instruction instr);

struct instr_branch parse_branch(struct instruction instr);

#endif
