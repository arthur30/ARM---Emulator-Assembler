#include "assemble_instructions.h"
#include "assemble_dictionary.h"
#include "emulate_pi_state.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* Variables to build the instructions. */

static uint32_t cond;      /* for COND                  */

static uint32_t opcode;    /* for OPCODE                */
static uint32_t operand2;  /* for OPERAND2              */
static uint32_t imm;       /* for OPERAND2              */
static uint32_t shift;     /* for OPERAND2              */
static uint32_t offset;    /* for OFFSET                */

static uint32_t i;         /* for IMMEDIATE OFFSET      */
static uint32_t s;         /* for SET CONDITION BIT     */
static uint32_t a;         /* for ACCUMULATE BIT        */
static uint32_t p;         /* for PRE/POST INDEXING BIT */
static uint32_t u;         /* for UP BIT                */
static uint32_t l;         /* for LOAD/STORE BIT        */

static uint32_t rn;        /* for REGISTER Rn           */
static uint32_t rd;        /* for REGISTER Rd           */
static uint32_t rs;        /* for REGISTER Rd           */
static uint32_t rm;        /* for REGISTER Rm           */

/* All functions return a 32bit integer. */

uint32_t instr_dpi(struct instruction *instr)
{
	cond = 14 << 28;
	opcode = (instr->code) << 21;
	rd = instr->instr.dpi.rd << 12;
	rn = instr->instr.dpi.rn << 16;
	i = 0;
	s = 0;
	imm = 0;
	shift = 0;
	operand2 = 0;

	if (instr->instr.dpi.op2.immediate) {
		i = 1 << 25;
		imm = instr->instr.dpi.op2.offset.imm.imm;
		shift = instr->instr.dpi.op2.offset.imm.rotate;
	} else {
		operand2 = instr->instr.dpi.op2.offset.reg.rm;
	}

	if (instr->instr.dpi.setcond)
		s = 1 << 20;

	return cond + i + opcode + s + rn + rd + (shift << 8) + imm + operand2;
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
