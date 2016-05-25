#include "assemble_instructions.h"
#include "assemble_parser.h"
#include "assemble_dictionary.h"
#include "emulate_pi_state.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* Variables to build the instructions. */

uint32_t cond;      /* for COND                  */

uint32_t opcode;    /* for OPCODE                */
uint32_t operand2;  /* for OPERAND2              */
uint32_t offset;    /* for OFFSET                */

uint32_t i;         /* for IMMEDIATE OFFSET      */
uint32_t s;         /* for SET CONDITION BIT     */
uint32_t a;         /* for ACCUMULATE BIT        */
uint32_t p;         /* for PRE/POST INDEXING BIT */
uint32_t u;         /* for UP BIT                */
uint32_t l;         /* for LOAD/STORE BIT        */

uint32_t rn;        /* for REGISTER Rn           */
uint32_t rd;        /* for REGISTER Rd           */
uint32_t rs;        /* for REGISTER Rd           */
uint32_t rm;        /* for REGISTER Rm           */

/* All functions return a 32bit integer. */

uint32_t instr_dpi(struct instruction instr)
{
	/*
	 * Instruction Result:
	 * COND 00 I OpCode S -Rn- -Rd- Operand2
	 * COND => 1110
	 * OpCode => See from Spec.
	 * S => (tst, teq, cmp -> 1) || (rest -> 0)
	 * I => Set if Operand2 is an immediate const.
	 * Operand2 => <#expression> || Rm{,<shift>}
	 */

	struct instr_data_proc parsed = parse_dpi(instr);

	cond = 14 << 28;
	opcode = dpi_to_opcode(instr.mnemonic) << 21;
	rd = parsed.rd << 12;
	rn = parsed.rn << 16;

	if (parsed.op2.immediate) {
		i = 1 << 25;
		operand2 = parsed.op2.offset.imm.imm;
	} else {
		i = 0;
		operand2 = parsed.op2.offset.reg.rm;
	}

	if (parsed.setcond)
		s = 1 << 20;
	else
		s = 0;

	return cond + i + opcode + s + rn + rd + operand2;
}

uint32_t instr_multiply(struct instruction instr)
{
	/*
	 * Instruction Result:
	 * COND 0000 00AS -Rd- -Rn- -Rs- 1001 -Rm-
	 * COND => 1110
	 * A => (mul -> 0) || (mla -> 1)
	 * S => 0
	 * mla r1, r2, r3, r4 => r1 = (r2 x r3) + r4
	 * mul r1, r2, r3 => r1 = r2 x r3
	 */

	struct instr_mult parsed = parse_mult(instr);

	cond = 14 << 28;
	s = 0;
	rs = parsed.rs << 8;
	rm = parsed.rm;
	rd = parsed.rd << 16;

	if (parsed.accumulate) {
		a = 1 << 21;
		rn = parsed.rn << 12;
	} else {
		a = 0;
		rn = 0;
	}

	return cond + a + s + rd + rn + rs + 144 + rm;
}

uint32_t instr_sdt(struct instruction instr)
{
	/*
	 * Instruction Result:
	 * COND 01IP U00L -Rn- -Rd- Offset
	 * L => (ldr -> 1) || (sdr -> 0)
	 * I => Set if Offset is shifted register.
	 * U => Set if Offset is added to base reg.
	 * P => Pre/Post indexing bit. See spec pg9.
	 */

	(void) instr;

	if (l == 0)
		l = 1;
	else
		l = 0;

	return cond + (1 << 26) + i + p + u + l + rn + rd + offset;
}

uint32_t instr_branch(struct instruction instr)
{
	/*
	 * Instruction Result:
	 * COND 1010 Offset
	 * COND => See from the spec.
	 */

	(void) instr;

	return cond + (10 << 24) + offset;
}
