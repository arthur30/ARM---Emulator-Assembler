#include "assemble_parser.h"
#include "assemble_dictionary.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct instr_data_proc parse_dpi(struct instruction instr)
{
	struct instr_data_proc parsed;

	parsed.opcode = dpi_to_opcode(instr.mnemonic);

	return parsed;
}

struct instr_mult parse_mult(struct instruction instr)
{
	(void) instr;
	struct instr_mult parsed;

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
