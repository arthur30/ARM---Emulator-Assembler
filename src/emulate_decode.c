#include "emulate_decode.h"

#include "pi_state.h"

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#define GETFLAG(ic, flag) (!!(ic & flag))
#define GETBITS(ic, p, n) (((ic) >> (p)) & ((1 << (n)) - 1))
#define GETREG(ic, p) GETBITS(ic, p, 4)
#define SEXT26(value) ((value) | (((value) & (1 << 25)) ? 0xFC000000 : 0))

static bool instr_is_data_proc(uint32_t ic)
{
	return !(ic & INSTR_DATA_PROC_MASK) &&
			(GETFLAG(ic, INSTR_BIT_IMM) ||
			(ic & INSTR_MULT_BITP) < INSTR_MULT_BITP);
}

static bool instr_is_mult(uint32_t ic)
{
	return (ic & INSTR_MULT_MASK) == INSTR_MULT_BITP;
}

static bool instr_is_transfer(uint32_t ic)
{
	return (ic & INSTR_TRANSFER_MASK) == INSTR_TRANSFER_BITP;
}

static bool instr_is_branch(uint32_t ic)
{
	return (ic & INSTR_BRANCH_MASK) == INSTR_BRANCH_BITP;
}

static void set_instruction_type(struct pi_state *pstate, enum instr_type type)
{
	pstate->pipeline.instruction.type = type;
}

static int decode_halt(struct pi_state *pstate)
{
	set_instruction_type(pstate, HALT);
	return 0;
}

static int decode_shift_register(uint32_t ic, struct shift_reg *reg)
{
	reg->rm = GETREG(ic, 0);
	reg->constant = !GETFLAG(ic, INSTR_BIT_CONST);
	reg->shift_type = GETBITS(ic, 5, 2);
	if (reg->constant)
		reg->amount.integer = GETBITS(ic, 7, 5);
	else
		reg->amount.rs = GETREG(ic, 8);
	return 0;
}

static int decode_op2(uint32_t ic, struct instr_op2 *op2)
{
	op2->immediate = GETFLAG(ic, INSTR_BIT_IMM);
	if (op2->immediate) {
		op2->offset.imm.imm = GETBITS(ic, 0, 8);
		op2->offset.imm.rotate = GETBITS(ic, 8, 4);
	} else {
		decode_shift_register(ic, &op2->offset.reg);
	}

	return 0;
}

static int decode_offset(uint32_t ic, struct instr_offset *offset)
{
	offset->immediate = GETFLAG(ic, INSTR_BIT_IMM);
	if (offset->immediate)
		decode_shift_register(ic, &offset->offset.reg);
	else
		offset->offset.imm = GETBITS(ic, 0, 12);

	return 0;
}

static int decode_data_proc(uint32_t ic, struct pi_state *pstate)
{
	struct instr_data_proc *data_proc;

	set_instruction_type(pstate, DATA_PROC);
	ic = pstate->pipeline.instr_code;
	data_proc = &pstate->pipeline.instruction.instr_bits.data_proc;

	data_proc->opcode = GETBITS(ic, 21, 4);
	data_proc->setcond = GETFLAG(ic, INSTR_BIT_SETCOND);
	data_proc->rn = GETREG(ic, 16);
	data_proc->rd = GETREG(ic, 12);
	return decode_op2(ic, &data_proc->op2);
}

static int decode_mult(uint32_t ic, struct pi_state *pstate)
{
	struct instr_mult *mult;

	set_instruction_type(pstate, MULT);
	ic = pstate->pipeline.instr_code;
	mult = &pstate->pipeline.instruction.instr_bits.mult;

	mult->accumulate = GETFLAG(ic, INSTR_BIT_ACC);
	mult->setcond = GETFLAG(ic, INSTR_BIT_SETCOND);
	mult->rd = GETREG(ic, 16);
	mult->rn = GETREG(ic, 12);
	mult->rs = GETREG(ic,  8);
	mult->rm = GETREG(ic,  0);

	return 0;
}

static int decode_transfer(uint32_t ic, struct pi_state *pstate)
{
	struct instr_transfer *transfer;

	set_instruction_type(pstate, TRANSFER);
	ic = pstate->pipeline.instr_code;
	transfer = &pstate->pipeline.instruction.instr_bits.transfer;

	transfer->preindexing = GETFLAG(ic, INSTR_BIT_INDEX);
	transfer->up = GETFLAG(ic, INSTR_BIT_UP);
	transfer->load = GETFLAG(ic, INSTR_BIT_LOAD);
	transfer->rn = GETREG(ic, 16);
	transfer->rd = GETREG(ic, 12);
	return decode_offset(ic, &transfer->offset);
}

static int decode_branch(uint32_t ic, struct pi_state *pstate)
{
	struct instr_branch *branch;
	int32_t offset;

	set_instruction_type(pstate, BRANCH);
	ic = pstate->pipeline.instr_code;
	branch = &pstate->pipeline.instruction.instr_bits.branch;

	offset = GETBITS(ic, 0, 24) << 2;
	offset = SEXT26(offset);
	branch->offset = offset;

	return 0;
}

int decode(struct pi_state *pstate)
{
	uint32_t ic;
	struct instr *instruction;

	if (!pstate->pipeline.fetched)
		return 0;

	pstate->pipeline.fetched = false;
	pstate->pipeline.decoded = true;

	ic = pstate->pipeline.instr_code;

	instruction = &pstate->pipeline.instruction;
	instruction->cond = GETBITS(ic, 28, 4);

	if (!ic)
		return decode_halt(pstate);
	if (instr_is_data_proc(ic))
		return decode_data_proc(ic, pstate);
	if (instr_is_mult(ic))
		return decode_mult(ic, pstate);
	if (instr_is_transfer(ic))
		return decode_transfer(ic, pstate);
	if (instr_is_branch(ic))
		return decode_branch(ic, pstate);

	errno = EINVAL;
	return -1;
}
