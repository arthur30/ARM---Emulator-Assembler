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
	uint32_t cond = COND_ALWAYS;
	uint32_t opcode = (uint32_t)(instr->code) << 21;
	uint32_t rd = (uint32_t)instr->instr.dpi.rd << 12;
	uint32_t rn = (uint32_t)instr->instr.dpi.rn << 16;
	uint32_t rm = 0;
	uint32_t i = 0;
	uint32_t s = 0;
	uint32_t imm = 0;
	uint32_t shift = 0;
	uint32_t operand2 = 0;
	bool constant = false;
	uint8_t shift_type = 0;
	uint8_t amount = 0;

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
	uint32_t cond = COND_ALWAYS;
	uint32_t rd = instr->instr.mult.rd << 16;
	uint32_t rs = instr->instr.mult.rs << 8;
	uint32_t rn = 0;
	uint32_t rm = instr->instr.mult.rm;
	uint32_t a = 0;
	uint32_t s = 0;

	if (instr->instr.mult.accumulate) {
		a = 1 << 21;
		rn = instr->instr.mult.rn << 12;
	}

	return cond + a + s + rd + rn + rs + 144 + rm;
}

uint32_t instr_sdt(struct instruction *instr)
{
	uint32_t cond = COND_ALWAYS;
	uint32_t rn = instr->instr.sdt.rn << 16;
	uint32_t rd = instr->instr.sdt.rd << 12;
	uint32_t rm = 0;
	uint32_t l = 0;
	uint32_t p = 0;
	uint32_t u = 0;
	uint32_t i = 0;
	uint32_t offset = instr->instr.sdt.offset.offset.imm;
	bool constant = false;
	uint8_t shift_type = 0;
	uint8_t amount = 0;

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
	uint32_t cond = instr->code << 28;
	uint32_t offset = (instr->instr.branch.offset >> 2) & ((1 << 24) - 1);

	return cond + (10 << 24) + offset;
}
