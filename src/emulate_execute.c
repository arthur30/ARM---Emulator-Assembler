#include "emulate_execute.h"
#include "emulate_pi_state.h"
#include "emulate_errors.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define SHIFT_ROR 3

static void set_cpsr_c(struct cpsrreg *cpsr, bool val)
{
	if (cpsr)
		cpsr->c = val;
}

static void set_cpsr_zn(uint32_t res, struct cpsrreg *cpsr)
{
	if (cpsr) {
		cpsr->z = !res;
		cpsr->n = !!(res >> 31);
	}
}

static int execute_halt(struct pi_state *pstate)
{
	(void)pstate;
	errno = 0;
	return 1;
}

static int
op_undef(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr)
{
	(void)rd;
	(void)rn;
	(void)op2;
	(void)cpsr;

	printf("Hit undefined operation\n");
	errno = EINVAL;
	return -1;
}

static int
op_and(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr)
{
	uint32_t res = rn & op2;

	set_cpsr_zn(res, cpsr);
	*rd = res;
	return 0;
}

static int
op_eor(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr)
{
	uint32_t res = rn ^ op2;

	set_cpsr_zn(res, cpsr);
	*rd = res;
	return 0;
}

static int
op_sub(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr)
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
op_rsb(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr)
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
op_add(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr)
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
op_tst(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr)
{
	(void)rd;
	uint32_t res = rn & op2;

	set_cpsr_zn(res, cpsr);
	return 0;
}

static int
op_teq(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr)
{
	(void)rd;
	uint32_t res = rn ^ op2;

	set_cpsr_zn(res, cpsr);
	return 0;
}

static int
op_cmp(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr)
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
op_orr(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr)
{
	uint32_t res = rn | op2;

	set_cpsr_zn(res, cpsr);
	*rd = res;
	return 0;
}

static int
op_mov(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr)
{
	(void)rn;
	uint32_t res = op2;

	set_cpsr_zn(res, cpsr);
	*rd = res;
	return 0;
}

static int (*data_proc_table[16])
(uint32_t *rd, uint32_t rn, uint32_t op2, struct cpsrreg *cpsr) = {
	/* 0000 */ op_and,
	/* 0001 */ op_eor,
	/* 0010 */ op_sub,
	/* 0011 */ op_rsb,
	/* 0100 */ op_add,
	/* 0101 */ op_undef,
	/* 0110 */ op_undef,
	/* 0111 */ op_undef,
	/* 1000 */ op_tst,
	/* 1001 */ op_teq,
	/* 1010 */ op_cmp,
	/* 1011 */ op_undef,
	/* 1100 */ op_orr,
	/* 1101 */ op_mov,
	/* 1110 */ op_undef,
	/* 1111 */ op_undef
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

static int (*shift_table[4])
(uint32_t *r, uint32_t val, uint32_t amount, bool *carry) = {
	/* 00 */ shift_lsl,
	/* 01 */ shift_lsr,
	/* 10 */ shift_asr,
	/* 11 */ shift_ror
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
		type = SHIFT_ROR;
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
	struct cpsrreg *cpsr;
	uint32_t *rd;
	uint32_t rn;
	uint32_t op2val;
	uint8_t opcode;
	bool carry;

	data_proc = &pstate->pipeline.instruction.instr_bits.data_proc;
	op2 = &data_proc->op2;
	carry = pstate->cpsr.c;

	rd = &pstate->registers[data_proc->rd];
	rn = pstate->registers[data_proc->rn];
	get_op2(&op2val, op2, pstate, &carry);

	cpsr = data_proc->setcond ? &pstate->cpsr : NULL;
	set_cpsr_c(cpsr, carry);

	opcode = data_proc->opcode;
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
	set_cpsr_zn(*rd, &pstate->cpsr);

	return 0;
}

static uint8_t gpio_dummy[4];
static uint8_t *get_gpio(struct pi_state *pstate, size_t address)
{
	int range, low, high;
	size_t i;
	uint32_t addr;

	if (address >= GPIO_CONTROL_ADDRESS &&
	    address - GPIO_CONTROL_ADDRESS < GPIO_CONTROL_SIZE) {
		range = (address - GPIO_CONTROL_ADDRESS) / 4;
		low = range * 10;
		high = range * 10 + 9;

		for (i = 0; i < GPIO_CONTROL_SIZE; i += 4) {
			addr = GPIO_CONTROL_ADDRESS + i;
			memcpy(&pstate->gpio_control[i], &addr, sizeof(addr));
		}

		fprintf(stdout, GPIO_PIN_ACCESS, low, high);
		return &pstate->gpio_control[address - GPIO_CONTROL_ADDRESS];
	}

	if (address >= GPIO_CLEARING_ADDRESS &&
	    address - GPIO_CLEARING_ADDRESS < GPIO_CONTROL_SIZE) {
		fprintf(stdout, GPIO_PIN_OFF);
		return gpio_dummy;
	}

	if (address >= GPIO_TURNON_ADDRESS &&
	    address - GPIO_TURNON_ADDRESS < GPIO_TURNON_SIZE) {
		fprintf(stdout, GPIO_PIN_ON);
		return gpio_dummy;
	}

	return NULL;

}

static uint8_t *get_memory(struct pi_state *pstate, size_t address)
{
	uint8_t *gpio = get_gpio(pstate, address);

	if (gpio)
		return gpio;

	if (address + 4 >= PI_MEMORY_SIZE) {
		fprintf(stdout, OUT_OF_BOUNDS_MEM, address);
		errno = EINVAL;
		return NULL;
	}

	return &pstate->memory[address];
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
	carry = pstate->cpsr.c;

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
	if (!mem)
		return -1;

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

static int (*instr_type_table[5]) (struct pi_state *pstate) = {
	execute_halt,
	execute_data_proc,
	execute_mult,
	execute_transfer,
	execute_branch
};

static int cond_uninmlemented(struct cpsrreg *cpsr)
{
	(void)cpsr;
	printf("Got unimplemented condition\n");
	return -1;
}

static int cond_eq(struct cpsrreg *cpsr)
{
	return cpsr->z;
}

static int cond_ne(struct cpsrreg *cpsr)
{
	return !cpsr->z;
}

static int cond_ge(struct cpsrreg *cpsr)
{
	return cpsr->n == cpsr->v;
}

static int cond_lt(struct cpsrreg *cpsr)
{
	return cpsr->n != cpsr->v;
}

static int cond_gt(struct cpsrreg *cpsr)
{
	return !cpsr->z && (cpsr->n == cpsr->v);
}

static int cond_le(struct cpsrreg *cpsr)
{
	return cpsr->z || (cpsr->n != cpsr->v);
}

static int cond_al(struct cpsrreg *cpsr)
{
	(void)cpsr;
	return true;
}

static int (*cond_table[16]) (struct cpsrreg *cpsr) = {
	/* 0000 */ cond_eq,
	/* 0001 */ cond_ne,
	/* 0010 */ cond_uninmlemented,
	/* 0011 */ cond_uninmlemented,
	/* 0100 */ cond_uninmlemented,
	/* 0101 */ cond_uninmlemented,
	/* 0110 */ cond_uninmlemented,
	/* 0111 */ cond_uninmlemented,
	/* 1000 */ cond_uninmlemented,
	/* 1001 */ cond_uninmlemented,
	/* 1010 */ cond_ge,
	/* 1011 */ cond_lt,
	/* 1100 */ cond_gt,
	/* 1101 */ cond_le,
	/* 1110 */ cond_al,
	/* 1111 */ cond_uninmlemented,
};

static int check_cond(struct pi_state *pstate)
{
	return cond_table[pstate->pipeline.instruction.cond](&pstate->cpsr);
}

int execute(struct pi_state *pstate)
{
	enum instr_type type;
	int cond_check_res;

	cond_check_res = check_cond(pstate);
	if (cond_check_res == -1)
		return -1;

	pstate->pipeline.decoded = false;
	type = pstate->pipeline.instruction.type;

	if (cond_check_res || type == HALT)
		return instr_type_table[type](pstate);
	return 0;
}
