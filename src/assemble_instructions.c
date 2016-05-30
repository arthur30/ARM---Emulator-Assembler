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
	bool constant = false;
	uint8_t shift_type = 0;
	uint8_t amount = 0;

	cond = 14 << 28;
	opcode = (uint32_t)(instr->code) << 21;
	rd = (uint32_t)instr->instr.dpi.rd << 12;
	rn = (uint32_t)instr->instr.dpi.rn << 16;
	rm = 0;
	i = 0;
	s = 0;
	imm = 0;
	shift = 0;
	operand2 = 0;

	if (instr->code == 5)
		return 0;

	if (instr->instr.dpi.op2.immediate) {
		i = 1 << 25;
		imm = instr->instr.dpi.op2.offset.imm.imm;
		shift = instr->instr.dpi.op2.offset.imm.rotate << 8;
	} else {
		rm = instr->instr.dpi.op2.offset.reg.rm;
		constant = instr->instr.dpi.op2.offset.reg.constant;
		shift_type = instr->instr.dpi.op2.offset.reg.shift_type;

		if (!constant) {
			amount = instr->instr.dpi.op2.offset.reg.amount.integer;
			operand2 = ((uint32_t)amount << 7) |
				((uint32_t)shift_type << 5) |
				((uint32_t)constant << 4) |
				rm;
		} else {
			amount = instr->instr.dpi.op2.offset.reg.amount.rs;
			operand2 = ((uint32_t)amount << 8) |
				((uint32_t)shift_type << 5) |
				((uint32_t)constant << 4) |
				rm;
		}
	}


	if (instr->instr.dpi.setcond)
		s = 1 << 20;

	return cond + i + opcode + s + rn + rd + shift + imm + operand2;
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

	bool constant = false;
	uint8_t shift_type = 0;
	uint8_t amount = 0;

	cond = 14 << 28;
	l = 0;
	p = 0;
	u = 0;
	i = 0;
	rn = instr->instr.sdt.rn << 16;
	rd = instr->instr.sdt.rd << 12;
	rm = 0;
	offset = instr->instr.sdt.offset.offset.imm;

	if (instr->instr.sdt.up)
		u = (uint32_t)1 << 23;

	if (instr->instr.sdt.load)
		l = (uint32_t)1 << 20;

	if (instr->instr.sdt.preindexing)
		p = (uint32_t)1 << 24;

	if (instr->instr.sdt.offset.immediate) {
		rm = instr->instr.sdt.offset.offset.reg.rm;
		shift_type = instr->instr.sdt.offset.offset.reg.shift_type;
		constant = instr->instr.sdt.offset.offset.reg.constant;

		if (!constant) {
			amount = instr->instr.sdt.offset.offset.reg.amount.
									integer;
			offset = ((uint32_t)amount << 7) |
				((uint32_t)shift_type << 5) |
				((uint32_t)constant << 4) |
				rm;
		} else {
			amount = instr->instr.sdt.offset.offset.reg.amount.rs;
			offset = ((uint32_t)amount << 8) |
				((uint32_t)shift_type << 5) |
				((uint32_t)constant << 4) |
				rm;
		}

		i = (uint32_t)1 << 25;
	}

	return cond | (1 << 26) | i | p | u | l | rn | rd | offset | rm;
}

uint32_t instr_branch(struct instruction *instr)
{
	cond = instr->code << 28;
	offset = (instr->instr.branch.offset >> 2) & ((1 << 24) - 1);

	return cond + (10 << 24) + offset;
}
