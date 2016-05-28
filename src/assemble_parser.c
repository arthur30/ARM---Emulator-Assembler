#include "assemble_parser.h"
#include "assemble_dictionary.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct instr_data_proc parse_dpi(struct instruction *instr)
{
	struct instr_data_proc parsed;

	parsed.opcode = dpi_to_opcode(instr->mnemonic);
	int opc = parsed.opcode;
	char *ptr;

	switch (opc) {
	case 8:
	case 9:
	case 10:
		parsed.setcond = true;
		parsed.rn = (uint8_t) strtol(++instr->op1, &ptr, 10);
		parsed.rd = 0;
		if (instr->op2[0] == '#') {
			parsed.op2.immediate = true;
			parsed.op2.offset.imm.imm
				= strtol(++instr->op2, &ptr, 10);
		} else {
			parsed.op2.immediate = false;
			parsed.op2.offset.reg.rm
				= strtol(++instr->op2, &ptr, 10);
		}
		break;
	case 13:
		parsed.setcond = false;
		parsed.rn = 0;
		parsed.rd = (uint8_t) strtol(++instr->op1, &ptr, 10);
		if (instr->op2[0] == '#') {
			parsed.op2.immediate = true;
			parsed.op2.offset.imm.imm
				= strtol(++instr->op2, &ptr, 10);
		} else {
			parsed.op2.immediate = false;
			parsed.op2.offset.reg.rm
				= strtol(++instr->op2, &ptr, 10);
		}
		break;
	default:
		parsed.setcond = false;
		parsed.rd = (uint8_t) strtol(++instr->op1, &ptr, 10);
		parsed.rn = (uint8_t) strtol(++instr->op2, &ptr, 10);
		if (instr->op3[0] == '#') {
			parsed.op2.immediate = true;
			parsed.op2.offset.imm.imm
				= strtol(++instr->op3, &ptr, 10);
		} else {
			parsed.op2.immediate = false;
			parsed.op2.offset.reg.rm
				= strtol(++instr->op3, &ptr, 10);
		}
		break;
	}

	/* CONSIDER SHITFS */
	/* HAVENT CONSIDERED HEXADECIMALS */

	return parsed;
}

struct instr_mult parse_mult(struct instruction *instr)
{
	struct instr_mult parsed;
	char *ptr;

	parsed.accumulate = mult_select(instr->mnemonic) == 1;
	parsed.setcond = false;
	parsed.rd = (uint8_t) strtol(++instr->op1, &ptr, 10);
	parsed.rm = (uint8_t) strtol(++instr->op2, &ptr, 10);
	parsed.rs = (uint8_t) strtol(++instr->op3, &ptr, 10);

	if (parsed.accumulate)
		parsed.rn = (uint8_t) strtol(++instr->op4, &ptr, 10);

	return parsed;
}

struct instr_transfer parse_sdt(struct instruction *instr)
{
	struct instr_transfer parsed;
	char *rn;
	char *offset;
	char *ptr;

	parsed.load = sdt_select(instr->mnemonic) == 0;
	parsed.rd = (uint8_t) strtol(++instr->op1, &ptr, 10);
	parsed.up = true;
	parsed.preindexing = true;
	parsed.offset.immediate = false;

	if (instr->op2[0] == '=') {
		if (!parsed.load) {
			fprintf(stderr, "sdr cannot have numeric constants.");
			exit(EXIT_FAILURE);
		}
		parsed.rn = 0;

		return parsed;
	}

	if (instr->op3) {

		parsed.preindexing = false;
		rn = strtok(instr->op2, "[]");
		offset = strtok(instr->op3, "#");
		parsed.rn = (uint8_t) strtol(++rn, &ptr, 10);
		parsed.offset.immediate = true;
		parsed.offset.offset.imm = (uint16_t) strtol(offset, &ptr, 10);

		return parsed;
	}

	rn = strtok(instr->op2, "[]");
	parsed.rn = (uint8_t) strtol(++rn, &ptr, 10);

	return parsed;
}

struct instr_branch parse_branch(struct instruction *instr, int off)
{
	(void) instr;
	(void) off;
	struct instr_branch parsed;

	return parsed;
}
