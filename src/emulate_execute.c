#include "emulate_execute.h"
#include "emulate_memory.h"

#include "pi_state.h"
#include "pi_msgs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static void set_cpsr_c(uint32_t *cpsr, bool val)
{
	if (cpsr) {
		*cpsr &= ~CPSR_BIT_C;
		*cpsr |= val ? CPSR_BIT_C : 0;
	}
}

static void set_cpsr_zn(uint32_t res, uint32_t *cpsr)
{
	if (cpsr) {
		*cpsr &= ~CPSR_BIT_Z & ~CPSR_BIT_N;
		*cpsr |= !res ? CPSR_BIT_Z : 0;
		*cpsr |= !!(res >> 31) ? CPSR_BIT_N : 0;
	}
}

static int
op_undef(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr)
{
	(void)rd;
	(void)rn;
	(void)op2;
	(void)cpsr;

	fprintf(stderr, EMU_ERR_UNDEF_DPI_OP);
	errno = EINVAL;
	return -1;
}

static int
op_and(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr)
{
	uint32_t res = rn & op2;

	set_cpsr_zn(res, cpsr);
	*rd = res;
	return 0;
}

static int
op_eor(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr)
{
	uint32_t res = rn ^ op2;

	set_cpsr_zn(res, cpsr);
	*rd = res;
	return 0;
}

static int
op_sub(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr)
{
	uint64_t res;
	bool carry;

	res = (uint64_t)rn - (uint64_t)op2;
	carry = !(res >> 32);
	set_cpsr_c(cpsr, carry);
	set_cpsr_zn(res, cpsr);
	*rd = res;
	return 0;
}

static int
op_rsb(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr)
{
	uint64_t res;
	bool carry = true;

	res = (uint64_t)op2 - (uint64_t)rn;
	carry = !(res >> 32);
	set_cpsr_c(cpsr, carry);
	set_cpsr_zn(res, cpsr);
	*rd = res;
	return 0;
}

static int
op_add(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr)
{
	uint64_t res;
	bool carry;

	res = (uint64_t)rn + (uint64_t)op2;
	carry = res & (1L << 33);
	set_cpsr_c(cpsr, carry);
	set_cpsr_zn(res, cpsr);
	*rd = (uint32_t)res;
	return 0;
}

static int
op_tst(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr)
{
	(void)rd;
	uint32_t res = rn & op2;

	set_cpsr_zn(res, cpsr);
	return 0;
}

static int
op_teq(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr)
{
	(void)rd;
	uint32_t res = rn ^ op2;

	set_cpsr_zn(res, cpsr);
	return 0;
}

static int
op_cmp(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr)
{
	(void)rd;
	uint64_t res;
	bool carry;

	res = (uint64_t)rn - (uint64_t)op2;
	carry = !(res >> 32);
	set_cpsr_c(cpsr, carry);
	set_cpsr_zn(res, cpsr);
	return 0;
}

static int
op_orr(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr)
{
	uint32_t res = rn | op2;

	set_cpsr_zn(res, cpsr);
	*rd = res;
	return 0;
}

static int
op_mov(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr)
{
	(void)rn;
	uint32_t res = op2;

	set_cpsr_zn(res, cpsr);
	*rd = res;
	return 0;
}

static int (*data_proc_table[INSTR_DPI_OPCODE_COUNT])
(uint32_t *rd, uint32_t rn, uint32_t op2, uint32_t *cpsr) = {
	/* 0000 */[INSTR_DPI_AND] = op_and,
	/* 0001 */[INSTR_DPI_EOR] = op_eor,
	/* 0010 */[INSTR_DPI_SUB] = op_sub,
	/* 0011 */[INSTR_DPI_RSB] = op_rsb,
	/* 0100 */[INSTR_DPI_ADD] = op_add,
	/* 0101 */ /* not implemented */
	/* 0110 */ /* not implemented */
	/* 0111 */ /* not implemented */
	/* 1000 */[INSTR_DPI_TST] = op_tst,
	/* 1001 */[INSTR_DPI_TEQ] = op_teq,
	/* 1010 */[INSTR_DPI_CMP] = op_cmp,
	/* 1011 */ /* not implemented */
	/* 1100 */[INSTR_DPI_ORR] = op_orr,
	/* 1101 */[INSTR_DPI_MOV] = op_mov,
	/* 1110 */ /* not implemented */
	/* 1111 */ /* not implemented */
};

static int shift_lsl(uint32_t *r, uint32_t val, uint32_t amount, bool *carry)
{
	if (amount > 31) {
		errno = EINVAL;
		return -1;
	}

	if (amount > 0)
		*carry = !!((val << (amount - 1)) & (1 << 31));
	*r = val << amount;

	return 0;
}

static int shift_lsr(uint32_t *r, uint32_t val, uint32_t amount, bool *carry)
{
	if (amount > 31) {
		errno = EINVAL;
		return -1;
	}

	if (amount > 0)
		*carry = !!((val >> (amount - 1)) & 0x1);
	*r = val >> amount;

	return 0;
}

static int shift_asr(uint32_t *r, uint32_t val, uint32_t amount, bool *carry)
{
	if (amount > 31) {
		errno = EINVAL;
		return -1;
	}

	if (amount > 0)
		*carry = !!((val >> (amount - 1)) & 0x1);
	*r = ((signed)val) >> amount;

	return 0;
}

static int shift_ror(uint32_t *r, uint32_t val, uint32_t amount, bool *carry)
{
	if (amount > 31) {
		errno = EINVAL;
		return -1;
	}

	if (amount > 0)
		*carry = !!((val >> (amount - 1)) & 0x1);
	*r = (val >> amount) | (val << (32 - amount));

	return 0;
}

static int (*shift_table[INSTR_SHIFT_COUNT])
(uint32_t *r, uint32_t val, uint32_t amount, bool *carry) = {
	/* 00 */[INSTR_SHIFT_LSL] = shift_lsl,
	/* 01 */[INSTR_SHIFT_LSR] = shift_lsr,
	/* 10 */[INSTR_SHIFT_ASR] = shift_asr,
	/* 11 */[INSTR_SHIFT_ROR] = shift_ror
};

static int get_shift_reg
(struct pi_state *pstate, struct shift_reg *reg,
uint8_t *type, uint32_t *val, uint32_t *amount)
{
	int8_t rs;

	*type = reg->shift_type;
	*val = pstate->registers[reg->rm];
	rs = reg->amount.rs;

	if (reg->constant)
		*amount = reg->amount.integer;
	else
		*amount = pstate->registers[rs];

	return 0;
}

static int get_op2
(uint32_t *r, struct instr_op2 *op2, struct pi_state *pstate, bool *carry)
{
	uint8_t type;
	uint32_t val;
	uint32_t amount;
	struct shift_reg *reg;

	if (op2->immediate) {
		type = INSTR_SHIFT_ROR;
		val = op2->offset.imm.imm;
		amount = op2->offset.imm.rotate;
		amount *= 2;
	} else {
		reg = &op2->offset.reg;
		if (get_shift_reg(pstate, reg, &type, &val, &amount))
			return -1;

	}

	return shift_table[type](r, val, amount, carry);
}

static int get_offset
(uint32_t *r, struct instr_offset *offset, struct pi_state *pstate, bool *carry)
{
	uint8_t type;
	uint32_t val;
	uint32_t amount;
	struct shift_reg *reg;

	if (offset->immediate) {
		reg = &offset->offset.reg;
		if (get_shift_reg(pstate, reg, &type, &val, &amount))
			return -1;
	} else {
		*r = offset->offset.imm;
		return 0;
	}

	return shift_table[type](r, val, amount, carry);
}

static int execute_data_proc(struct pi_state *pstate)
{
	struct instr_data_proc *data_proc;
	struct instr_op2 *op2;
	uint32_t *cpsr;
	uint32_t *rd;
	uint32_t rn;
	uint32_t op2val;
	uint8_t opcode;
	bool carry;

	data_proc = &pstate->pipeline.instruction.instr_bits.data_proc;
	op2 = &data_proc->op2;
	carry = !!(pstate->registers[R_CPSR] & CPSR_BIT_C);

	rd = &pstate->registers[data_proc->rd];
	rn = pstate->registers[data_proc->rn];
	get_op2(&op2val, op2, pstate, &carry);

	cpsr = data_proc->setcond ? &pstate->registers[R_CPSR] : NULL;
	set_cpsr_c(cpsr, carry);

	opcode = data_proc->opcode;
	if (!data_proc_table[opcode])
		return op_undef(rd, rn, op2val, cpsr);
	return data_proc_table[opcode](rd, rn, op2val, cpsr);
}

static int execute_mult(struct pi_state *pstate)
{
	uint32_t *rd;
	uint32_t rn;
	uint32_t rm;
	uint32_t rs;
	struct instr_mult *mult;

	mult = &pstate->pipeline.instruction.instr_bits.mult;

	rd = &pstate->registers[mult->rd];
	rn = pstate->registers[mult->rn];
	rs = pstate->registers[mult->rs];
	rm = pstate->registers[mult->rm];

	*rd = rm * rs + (mult->accumulate ? rn : 0);
	set_cpsr_zn(*rd, &pstate->registers[R_CPSR]);

	return 0;
}

static int execute_transfer(struct pi_state *pstate)
{
	struct instr_transfer *transfer;
	struct instr_offset *offset;
	uint32_t *rd;
	uint32_t rn;
	uint32_t offsetval;
	bool carry;
	uint8_t *mem;

	transfer = &pstate->pipeline.instruction.instr_bits.transfer;
	offset = &transfer->offset;
	carry = !!(pstate->registers[R_CPSR] & CPSR_BIT_C);

	rd = &pstate->registers[transfer->rd];
	rn = pstate->registers[transfer->rn];
	get_offset(&offsetval, offset, pstate, &carry);

	if (transfer->preindexing) {
		if (transfer->up)
			rn += offsetval;
		else
			rn -= offsetval;
	}

	mem = get_memory(pstate, rn);
	if (!mem) /* out of bonds memory is just printed, continue */
		return 0;

	if (transfer->load)
		memcpy(rd, mem, 4);
	else
		memcpy(mem, rd, 4);

	if (!transfer->preindexing) {
		if (transfer->up)
			pstate->registers[transfer->rn] += offsetval;
		else
			pstate->registers[transfer->rn] -= offsetval;
	}

	return 0;
}

static int execute_branch(struct pi_state *pstate)
{
	int32_t offset;

	offset = pstate->pipeline.instruction.instr_bits.branch.offset;
	pstate->registers[R_PC] += offset;
	pstate->pipeline.fetched = false;
	pstate->pipeline.decoded = false;
	return 0;
}

static int (*instr_type_table[]) (struct pi_state *pstate) = {
	[INSTR_TYPE_HALT] = NULL,
	[INSTR_TYPE_DATA_PROC] = execute_data_proc,
	[INSTR_TYPE_MULT] = execute_mult,
	[INSTR_TYPE_TRANSFER] = execute_transfer,
	[INSTR_TYPE_BRANCH] = execute_branch
};

static int cond_uninmlemented(uint32_t cpsr)
{
	(void)cpsr;
	fprintf(stderr, EMU_ERR_UNDEF_COND);
	return -1;
}

static int cond_eq(uint32_t cpsr)
{
	return (cpsr & CPSR_BIT_Z);
}

static int cond_ne(uint32_t cpsr)
{
	return !(cpsr & CPSR_BIT_Z);
}

static int cond_ge(uint32_t cpsr)
{
	return !!(cpsr & CPSR_BIT_N) == !!(cpsr & CPSR_BIT_V);
}

static int cond_lt(uint32_t cpsr)
{
	return !!(cpsr & CPSR_BIT_N) != !!(cpsr & CPSR_BIT_V);
}

static int cond_gt(uint32_t cpsr)
{
	return cond_ne(cpsr) && cond_ge(cpsr);
}

static int cond_le(uint32_t cpsr)
{
	return cond_eq(cpsr) || cond_lt(cpsr);
}

static int cond_al(uint32_t cpsr)
{
	(void)cpsr;
	return true;
}

static int (*cond_table[INSTR_COND_COUNT]) (uint32_t cpsr) = {
	/* 0000 */[INSTR_COND_EQ] = cond_eq,
	/* 0001 */[INSTR_COND_NE] = cond_ne,
	/* 0010 */ /* not implemented */
	/* 0011 */ /* not implemented */
	/* 0100 */ /* not implemented */
	/* 0101 */ /* not implemented */
	/* 0110 */ /* not implemented */
	/* 0111 */ /* not implemented */
	/* 1000 */ /* not implemented */
	/* 1001 */ /* not implemented */
	/* 1010 */[INSTR_COND_GE] = cond_ge,
	/* 1011 */[INSTR_COND_LT] = cond_lt,
	/* 1100 */[INSTR_COND_GT] = cond_gt,
	/* 1101 */[INSTR_COND_LE] = cond_le,
	/* 1110 */[INSTR_COND_AL] = cond_al,
	/* 1111 */ /* not valid */
};

static int check_cond(struct pi_state *pstate)
{
	uint8_t cond;
	uint32_t cpsr;

	cond = pstate->pipeline.instruction.cond;
	cpsr = pstate->registers[R_CPSR];

	if (!cond_table[cond])
		return cond_uninmlemented(cpsr);
	return cond_table[cond](cpsr);
}

int execute(struct pi_state *pstate)
{
	enum instr_type type;

	pstate->pipeline.decoded = false;
	type = pstate->pipeline.instruction.type;

	if (type == INSTR_TYPE_HALT)
		return 1;

	if (check_cond(pstate))
		return instr_type_table[type](pstate);

	return 0;
}
