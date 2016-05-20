#include "emulate_pi_state.h"
#include "emulate_decode.h"

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#define BIT_IMMEDIATE (1 << 25)

#define DATA_PROC_MASK 0x0C000000
#define DATA_PROC_BITP 0x00000000

#define MULT_MASK      0x0FC000F0
#define MULT_BITP      0x00000090

#define TRANSFER_MASK  0x0C600000
#define TRANSFER_BITP  0x04000000

#define BRANCH_MASK    0x0F000000
#define BRANCH_BITP    0x0A000000

static bool instr_is_data_proc(uint32_t instr)
{
	return !(instr & DATA_PROC_MASK) &&
		(instr & BIT_IMMEDIATE || (instr & MULT_BITP) < MULT_BITP);
}

static bool instr_is_mult(uint32_t instr)
{
	return (instr & MULT_MASK) == MULT_BITP;
}

static bool instr_is_transfer(uint32_t instr)
{
	return (instr & TRANSFER_MASK) == TRANSFER_BITP;
}

static bool instr_is_branch(uint32_t instr)
{
	return (instr & BRANCH_MASK) == BRANCH_BITP;
}

static void set_instruction_type(struct pi_state *pstate, enum instr_type type)
{
	pstate->pipeline.instruction.type = type;
}

static int decode_data_halt(struct pi_state *pstate)
{
	set_instruction_type(pstate, HALT);
	return 0;
}

static int decode_shift_register(uint32_t ic, struct shift_reg *reg)
{
	reg->rm = ic & 0xF;
	reg->constant = !(ic & (1 << 4));
	reg->shift_type = ((ic >> 5) & 0x3);
	if (reg->constant)
		reg->amount.integer = (ic >> 7) & 0x1F;
	else
		reg->amount.rs = (ic >> 8) & 0xF;
	return 0;
}

static int decode_op2(uint32_t ic, struct instr_op2 *op2)
{
	op2->immediate = !!(ic & (1 << 25));
	if (op2->immediate) {
		op2->offset.imm.imm = ic & 0x1FF;
		op2->offset.imm.rotate = (ic >> 8) & 0xF;
	} else {
		decode_shift_register(ic, &op2->offset.reg);
	}

	return 0;
}

static int decode_offset(uint32_t ic, struct instr_offset *offset)
{
	offset->immediate = !!(ic & (1 << 25));
	if (offset->immediate)
		decode_shift_register(ic, &offset->offset.reg);
	else
		offset->offset.imm = ic & 0xFFF;

	return 0;
}

static int decode_data_proc(struct pi_state *pstate)
{
	uint32_t ic;
	struct instr_data_proc *data_proc;

	set_instruction_type(pstate, DATA_PROC);
	ic = pstate->pipeline.instr_code;
	data_proc = &pstate->pipeline.instruction.instr_bits.data_proc;

	data_proc->opcode = (ic >> 21) & 0xF;
	data_proc->setcond = !!(ic & (1 << 20));
	data_proc->rn = (ic >> 16) & 0xF;
	data_proc->rd = (ic >> 12) & 0xF;
	return decode_op2(ic, &data_proc->op2);
}

static int decode_mult(struct pi_state *pstate)
{
	uint32_t ic;
	struct instr_mult *mult;

	set_instruction_type(pstate, MULT);
	ic = pstate->pipeline.instr_code;
	mult = &pstate->pipeline.instruction.instr_bits.mult;

	mult->accumulate = !!(ic & (1 << 21));
	mult->setcond = !!(ic & (1 << 20));
	mult->rd = (ic >> 16) & 0xF;
	mult->rn = (ic >> 12) & 0xF;
	mult->rs = (ic >>  8) & 0xF;
	mult->rm =  ic        & 0xF;

	return 0;
}

static int decode_transfer(struct pi_state *pstate)
{
	uint32_t ic;
	struct instr_transfer *transfer;

	set_instruction_type(pstate, TRANSFER);
	ic = pstate->pipeline.instr_code;
	transfer = &pstate->pipeline.instruction.instr_bits.transfer;

	transfer->preindexing = !!(ic & (1 << 24));
	transfer->up = !!(ic & (1 << 23));
	transfer->load = !!(ic & (1 << 20));
	transfer->rn = (ic >> 16) & 0xF;
	transfer->rd = (ic >> 12) & 0xF;
	return decode_offset(ic, &transfer->offset);
}

static int decode_branch(struct pi_state *pstate)
{
	uint32_t ic;
	struct instr_branch *branch;
	int32_t offset;

	set_instruction_type(pstate, BRANCH);
	ic = pstate->pipeline.instr_code;
	branch = &pstate->pipeline.instruction.instr_bits.branch;

	offset = (ic & 0xFFFFFF) << 2;
	offset |= (offset & (1 << 25)) ? 0xFC000000 : 0;
	branch->offset = offset;

	return 0;
}

int decode(struct pi_state *pstate)
{
	uint32_t instr_code;
	struct instr *instruction;

	if (!pstate->pipeline.fetched)
		return 0;

	pstate->pipeline.fetched = false;
	pstate->pipeline.decoded = true;

	instr_code = pstate->pipeline.instr_code;

	instruction = &pstate->pipeline.instruction;
	instruction->cond = instr_code >> 28;

	if (!instr_code)
		return decode_data_halt(pstate);
	if (instr_is_data_proc(instr_code))
		return decode_data_proc(pstate);
	if (instr_is_mult(instr_code))
		return decode_mult(pstate);
	if (instr_is_transfer(instr_code))
		return decode_transfer(pstate);
	if (instr_is_branch(instr_code))
		return decode_branch(pstate);

	errno = EINVAL;
	return -1;
}
