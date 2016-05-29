#include "assemble_instructions.h"
#include "assemble_dictionary.h"
#include "emulate_pi_state.h"

#include <stdlib.h>
#include <stdio.h>
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

uint32_t instr_dpi(struct instruction *instr)
{
	cond = 14 << 28;
	opcode = (instr->code) << 21;
	rd = instr->instr.dpi.rd << 12;
	rn = instr->instr.dpi.rn << 16;

	if (instr->instr.dpi.op2.immediate) {
		i = 1 << 25;
		operand2 = instr->instr.dpi.op2.offset.imm.imm;
	} else {
		i = 0;
		operand2 = instr->instr.dpi.op2.offset.reg.rm;
	}

	if (instr->instr.dpi.setcond)
		s = 1 << 20;
	else
		s = 0;

	return cond + i + opcode + s + rn + rd + operand2;
}

uint32_t instr_multiply(struct instruction *instr)
{
	cond = 14 << 28;
	s = 0;
	rs = instr->instr.mult.rs << 8;
	rm = instr->instr.mult.rm;
	rd = instr->instr.mult.rd << 16;

	if (instr->instr.mult.accumulate) {
		a = 1 << 21;
		rn = instr->instr.mult.rn << 12;
	} else {
		a = 0;
		rn = 0;
	}

	return cond + a + s + rd + rn + rs + 144 + rm;
}

uint32_t instr_sdt(struct instruction *instr)
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
	cond = 14 << 28;
	l = 0;
	p = 0;
	u = 1 << 23;
	i = 0;
	rn = instr->instr.sdt.rn << 16;
	rd = instr->instr.sdt.rd << 12;
	offset = instr->instr.sdt.offset.offset.imm;

	if (instr->instr.sdt.load)
		l = 1 << 20;

	if (instr->instr.sdt.preindexing)
		p = 1 << 24;

	if (instr->instr.sdt.offset.immediate)
		i = 1 << 24;

	return cond + (1 << 26) + i + p + u + l + rn + rd + offset;
}

uint32_t instr_branch(struct instruction *instr)
{
	cond = instr->code << 28;
	offset = (instr->instr.branch.offset >> 2) & ((1 << 24) - 1);

	return cond + (10 << 24) + offset;
}
