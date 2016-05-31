#include "emulate_decode.h"

#include "pi_state.h"

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

/* gets the flag */
#define GETFLAG(ic, flag) (!!(ic & flag))
/* gets n bits starting at bit p from the number ic*/
#define GETBITS(ic, p, n) (((ic) >> (p)) & ((1 << (n)) - 1))
/* get the register starting at bit p */
#define GETREG(ic, p) GETBITS(ic, p, NUMBER_OF_BITS_TO_EXTRACT_FOR_REGISTERS)
/* extends a 26 bit (signed) value to 32 bits - Sign = bit 25  */
#define SEXT26(value) ((value) | (((value) & (1 << 25)) ? 0xFC000000 : 0))
/* sets the i-th bit of num */
#define SETBIT(num, i) (num | (1 << i))
/* clears the i-th bit of num */
#define CLEARBIT(num, i) (num | (~(1 << i)))

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
	set_instruction_type(pstate, INSTR_TYPE_HALT);
	return 0;
}

static int decode_shift_register(uint32_t ic, struct shift_reg *reg)
{
	reg->rm = GETREG(ic, OP2_REGISTER_RM_FIRST_BIT);
	reg->constant = !GETFLAG(ic, INSTR_BIT_CONST);
	reg->shift_type = GETBITS(ic, OP2_REGISTER_SHIFT_TYPE_FIRST_BIT, OP2_REGISTER_SHIFT_TYPE_NUMBER_OF_BITS);
	if (reg->constant)
		reg->amount.integer = GETBITS(ic, OP2_REGISTER_SHIFT_CONSTANT_FIRST_BIT, OP2_REGISTER_SHIFT_CONSTANT_NUMBER_OF_BITS);
	else
		reg->amount.rs = GETREG(ic, OP2_REGISTER_SHIFT_REGISTER_FIRST_BIT);
	return 0;
}

static int decode_op2(uint32_t ic, struct instr_op2 *op2)
{
	op2->immediate = GETFLAG(ic, INSTR_BIT_IMM);
	if (op2->immediate) {
		op2->offset.imm.imm = GETBITS(ic, OP2_IMMEDIATE_VALUE_FIRST_BIT, OP2_IMMEDIATE_VALUE_LAST_BIT);
		op2->offset.imm.rotate = GETBITS(ic, OP2_IMMEDIATE_VALUE_ROTATION_FIRST_BIT, OP2_IMMEDIATE_VALUE_NUMBER_OF_ROTATIONS);
	} else {
		decode_shift_register(ic, &op2->offset.reg);
	}

	return 0;
}

/* Decodes the offset from single data transfer */
static int decode_offset(uint32_t ic, struct instr_offset *offset)
{
	offset->immediate = GETFLAG(ic, INSTR_BIT_IMM);
	if (offset->immediate)
		decode_shift_register(ic, &offset->offset.reg);
	else
		offset->offset.imm = GETBITS(ic, TRANSFER_OFFSET_FIRST_BIT, TRANSFER_OFFSET_LAST_BIT);

	return 0;
}
/* Cond(bits 31-28) 00(bits 27-26) I(Immediate operand-bit 25)
 * OpCode(bits 24-21) S(Set condition code-bit 20)
 * Rn(First operand register-bits 19-16)
 * Rd(Destination register-bits 15-12)
 * Operand2(bits 11-0)*/
static int decode_data_proc(uint32_t ic, struct pi_state *pstate)
{
	struct instr_data_proc *data_proc;

	set_instruction_type(pstate, INSTR_TYPE_DATA_PROC);
	ic = pstate->pipeline.instr_code;
	data_proc = &pstate->pipeline.instruction.instr_bits.data_proc;

	data_proc->opcode = GETBITS(ic, DATA_PROC_OPCODE_FIRST_BIT, NUMBER_OF_BITS_TO_EXTRACT_FOR_REGISTERS);
	data_proc->setcond = GETFLAG(ic, INSTR_BIT_SETCOND);
	data_proc->rn = GETREG(ic, DATA_PROC_RN_FIRST_BIT);
	data_proc->rd = GETREG(ic, DATA_PROC_RD_FIRST_BIT);
	return decode_op2(ic, &data_proc->op2);
}

/* Structure: Cond(bits 31-28) 000000(bits 27-22) A(Accumulate-bit 21)
 * S(Set cspr flags-bit 20) Rd(Destination register-bits 19-16)
 * Rn(Operand register-bits 15-12) Rs(Operand register-bits 11-8)
 * 1001(bits 7-4) Rm(Operand register-bits 4-0) */
static int decode_mult(uint32_t ic, struct pi_state *pstate)
{
	struct instr_mult *mult;

	set_instruction_type(pstate, INSTR_TYPE_MULT);
	ic = pstate->pipeline.instr_code;
	mult = &pstate->pipeline.instruction.instr_bits.mult;

	mult->accumulate = GETFLAG(ic, INSTR_BIT_ACC);
	mult->setcond = GETFLAG(ic, INSTR_BIT_SETCOND);
	mult->rd = GETREG(ic, MULT_RD_REGISTER_FIRST_BIT);
	mult->rn = GETREG(ic, MULT_RN_REGISTER_FIRST_BIT);
	mult->rs = GETREG(ic, MULT_RS_REGISTER_FIRST_BIT);
	mult->rm = GETREG(ic, MULT_RM_REGISTER_FIRST_BIT);

	return 0;
}

/* Structure: Cond(bits 31-28) 01(bits 27-26) I(Immediate offset-bit 25)
 * P(Pre/Post Indexing Bit-bit 24) U(Up bit-bit 23) 00(bits 22-21)
 * L(Load/Store bit-bit 20) Rn(Base register-bits 19-16)
 * Rd(Source/Destination register-bits 15-12) Offset(bits 11-0) */
static int decode_transfer(uint32_t ic, struct pi_state *pstate)
{
	struct instr_transfer *transfer;

	set_instruction_type(pstate, INSTR_TYPE_TRANSFER);
	ic = pstate->pipeline.instr_code;
	transfer = &pstate->pipeline.instruction.instr_bits.transfer;

	transfer->preindexing = GETFLAG(ic, INSTR_BIT_INDEX);
	transfer->up = GETFLAG(ic, INSTR_BIT_UP);
	transfer->load = GETFLAG(ic, INSTR_BIT_LOAD);
	transfer->rn = GETREG(ic, TRANSFER_RN_FIRST_BIT);
	transfer->rd = GETREG(ic, TRANSFER_RD_FIRST_BIT);
	return decode_offset(ic, &transfer->offset);
}

/* Cond(bits 31-28) 101(bits 27-25) 0(bit24) Offset(bits 23-0) */
static int decode_branch(uint32_t ic, struct pi_state *pstate)
{
	struct instr_branch *branch;
	int32_t offset;

	set_instruction_type(pstate, INSTR_TYPE_BRANCH);
	ic = pstate->pipeline.instr_code;
	branch = &pstate->pipeline.instruction.instr_bits.branch;

	offset = GETBITS(ic, BRANCH_OFFSET_FIRST_BIT, BRANCH_OFFSET_LAST_BIT) << 2;
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
	instruction->cond = GETBITS(ic, COND_FIRST_BIT, COND_NUMBER_OF_BITS);

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
	return -1; // error
}
