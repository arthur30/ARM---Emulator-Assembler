#include "emulate_pi_state.h"
#include "emulate_decode.h"

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#define BIT_IMM     (1 << 25)
#define BIT_SETCOND (1 << 20)
#define BIT_ACC     (1 << 21)
#define BIT_INDEX   (1 << 24)
#define BIT_UP      (1 << 23)
#define BIT_LOAD    (1 << 20)
#define BIT_CONST   (1 <<  4)

#define DATA_PROC_MASK 0x0C000000
#define DATA_PROC_BITP 0x00000000

#define MULT_MASK      0x0FC000F0
#define MULT_BITP      0x00000090

#define TRANSFER_MASK  0x0C600000
#define TRANSFER_BITP  0x04000000

#define BRANCH_MASK    0x0F000000
#define BRANCH_BITP    0x0A000000

#define GETBITS(ic, p, n) (((ic) >> (p)) & ((1 << (n)) - 1))
#define GETREG(ic, p) GETBITS(ic, p, 4)

static bool instr_is_data_proc(uint32_t ic)
{
	return !(ic & DATA_PROC_MASK) &&
		(ic & BIT_IMM || (ic & MULT_BITP) < MULT_BITP);
}

static bool instr_is_mult(uint32_t ic)
{
	return (ic & MULT_MASK) == MULT_BITP;
}

static bool instr_is_transfer(uint32_t ic)
{
	return (ic & TRANSFER_MASK) == TRANSFER_BITP;
}

static bool instr_is_branch(uint32_t ic)
{
	return (ic & BRANCH_MASK) == BRANCH_BITP;
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
	reg->constant = !(ic & BIT_CONST);
	reg->shift_type = GETBITS(ic, 5, 2);
	if (reg->constant)
		reg->amount.integer = GETBITS(ic, 7, 5);
	else
		reg->amount.rs = GETREG(ic, 8);
	return 0;
}

static int decode_op2(uint32_t ic, struct instr_op2 *op2)
{
	op2->immediate = !!(ic & BIT_IMM);
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
	offset->immediate = !!(ic & BIT_IMM);
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
	data_proc->setcond = !!(ic & BIT_SETCOND);
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

	mult->accumulate = !!(ic & BIT_ACC);
	mult->setcond = !!(ic & BIT_SETCOND);
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

	transfer->preindexing = !!(ic & BIT_INDEX);
	transfer->up = !!(ic & BIT_UP);
	transfer->load = !!(ic & BIT_LOAD);
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
	offset |= (offset & (1 << 25)) ? 0xFC000000 : 0;
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
