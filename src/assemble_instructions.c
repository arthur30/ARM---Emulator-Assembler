#include "assemble_instructions.h"
#include "assemble_dictionary.h"
#include "emulate_pi_state.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define COND_ALWAYS (14 << 28)

uint32_t instr_dpi(struct instruction *instr)
{
	uint32_t cond;
	uint32_t opcode;
	uint32_t rd;
	uint32_t rn;
	uint32_t rm;
	uint32_t i;
	uint32_t s;
	uint32_t imm;
	uint32_t shift;
	uint32_t operand2;
	bool constant;
	uint8_t shift_type;
	uint8_t amount;

	cond = COND_ALWAYS;
	opcode = (uint32_t)(instr->code) << 21;
	rd = (uint32_t)instr->instr.dpi.rd << 12;
	rn = (uint32_t)instr->instr.dpi.rn << 16;
	rm = 0;
	i = 0;
	s = 0;
	imm = 0;
	shift = 0;
	operand2 = 0;
	constant = false;
	shift_type = 0;
	amount = 0;

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
	uint32_t cond;
	uint32_t rd;
	uint32_t rs;
	uint32_t rn;
	uint32_t rm;
	uint32_t a;
	uint32_t s;

	cond = COND_ALWAYS;
	rd = instr->instr.mult.rd << 16;
	rs = instr->instr.mult.rs << 8;
	rn = 0;
	rm = instr->instr.mult.rm;
	a = 0;
	s = 0;

	if (instr->instr.mult.accumulate) {
		a = 1 << 21;
		rn = instr->instr.mult.rn << 12;
	}

	return cond + a + s + rd + rn + rs + 144 + rm;
}

uint32_t instr_sdt(struct instruction *instr)
{
	uint32_t cond;
	uint32_t rn;
	uint32_t rd;
	uint32_t rm;
	uint32_t l;
	uint32_t p;
	uint32_t u;
	uint32_t i;
	uint32_t offset;
	bool constant;
	uint8_t shift_type;
	uint8_t amount;

	cond = COND_ALWAYS;
	rn = instr->instr.sdt.rn << 16;
	rd = instr->instr.sdt.rd << 12;
	rm = 0;
	l = 0;
	p = 0;
	u = 0;
	i = 0;
	offset = instr->instr.sdt.offset.offset.imm;
	constant = false;
	shift_type = 0;
	amount = 0;

	if (instr->instr.sdt.up)
		u = (uint32_t)1 << 23;

	if (instr->instr.sdt.load)
		l = (uint32_t)1 << 20;

	if (instr->instr.sdt.preindexing)
		p = (uint32_t)1 << 24;

	if (instr->instr.sdt.offset.immediate) {
		i = (uint32_t)1 << 25;
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
	}

	return cond | (1 << 26) | i | p | u | l | rn | rd | offset | rm;
}

uint32_t instr_branch(struct instruction *instr)
{
	uint32_t cond;
	uint32_t offset;

	cond = instr->code << 28;
	offset = (instr->instr.branch.offset >> 2) & ((1 << 24) - 1);

	return cond + (10 << 24) + offset;
}
