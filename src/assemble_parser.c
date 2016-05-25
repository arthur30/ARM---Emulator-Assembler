#include "assemble_parser.h"
#include "assemble_dictionary.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct instr_data_proc parse_dpi(struct instruction instr)
{
	struct instr_data_proc parsed;

	parsed.opcode = dpi_to_opcode(instr.mnemonic);
	int opc = parsed.opcode;
	char *ptr;

	switch (opc) {
	case 8:
	case 9:
	case 10:
		parsed.setcond = true;
		parsed.rn = (uint8_t) strtol(++instr.op1, &ptr, 10);
		parsed.rd = 0;
		/* OP2 can be a register as well. Consider that */
		parsed.op2.offset.imm.imm = strtol(++instr.op2, &ptr, 10);
		break;
	case 13:
		parsed.setcond = false;
		parsed.rn = 0;
		parsed.rd = (uint8_t) strtol(++instr.op1, &ptr, 10);
		/* OP2 can be a register as well. Consider that */
		parsed.op2.offset.imm.imm = strtol(++instr.op2, &ptr, 10);
		break;
	default:
		parsed.setcond = false;
		parsed.rd = (uint8_t) strtol(++instr.op1, &ptr, 10);
		parsed.rn = (uint8_t) strtol(++instr.op2, &ptr, 10);
		/* OP2 can be a register as well. Consider that */
		parsed.op2.offset.imm.imm = strtol(++instr.op3, &ptr, 10);
		break;
	}

	/* HAVENT CONSIDERED HEXADECIMALS */

	return parsed;
}

struct instr_mult parse_mult(struct instruction instr)
{
	struct instr_mult parsed;
	char *ptr;

	parsed.accumulate = dpi_to_opcode(instr.mnemonic) == 21;
	parsed.setcond = false;
	parsed.rd = (uint8_t) strtol(++instr.op1, &ptr, 10);
	parsed.rm = (uint8_t) strtol(++instr.op2, &ptr, 10);
	parsed.rs = (uint8_t) strtol(++instr.op3, &ptr, 10);

	if (parsed.accumulate)
		parsed.rn = (uint8_t) strtol(++instr.op4, &ptr, 10);

	return parsed;
}

struct instr_transfer parse_sdt(struct instruction instr)
{
	(void) instr;
	struct instr_transfer parsed;

	return parsed;
}

struct instr_branch parse_branch(struct instruction instr)
{
	(void) instr;
	struct instr_branch parsed;

	return parsed;
}
