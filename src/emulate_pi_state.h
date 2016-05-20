#ifndef EMULATE_PI_STATE_H
#define EMULATE_PI_STATE_H

#include <stdint.h>
#include <stdbool.h>

#define PI_MEMORY_SIZE (1 << 16)
#define PI_REGISTER_COUNT 16
#define R_PC 15

struct shift_reg {
	bool constant;
	uint8_t shift_type;
	uint8_t rm;
	union {
		uint8_t integer;
		uint8_t rs;
	} amount;
};

struct instr_op2 {
	bool immediate;
	union {
		struct {
			uint8_t rotate;
			uint8_t imm;
		} imm;
		struct shift_reg reg;
	} offset;
};

struct instr_offset {
	bool immediate;
	union {
		struct shift_reg reg;
		uint16_t imm;
	} offset;
};

struct instr_data_proc {
	bool setcond;
	uint8_t opcode;
	uint8_t rn;
	uint8_t rd;
	struct instr_op2 op2;
};

struct instr_mult {
	bool accumulate;
	bool setcond;
	uint8_t rd;
	uint8_t rn;
	uint8_t rs;
	uint8_t rm;
};

struct instr_transfer {
	bool preindexing;
	bool up;
	bool load;
	uint8_t rn;
	uint8_t rd;
	struct instr_offset offset;
};

struct instr_branch {
	int32_t offset;
};

enum instr_type {
	HALT = 0,
	DATA_PROC = 1,
	MULT = 2,
	TRANSFER = 3,
	BRANCH = 4
};

struct instr {
	uint8_t cond;
	enum instr_type type;
	union instr_bits_t {
		struct instr_data_proc data_proc;
		struct instr_mult mult;
		struct instr_transfer transfer;
		struct instr_branch branch;
	} instr_bits;
};

struct cpsrreg {
	bool n;
	bool z;
	bool c;
	bool v;
};

struct pi_state {
	uint8_t memory[PI_MEMORY_SIZE];
	uint32_t registers[PI_REGISTER_COUNT];
	struct cpsrreg cpsr;
	struct pipeline {
		bool fetched;
		bool decoded;
		uint32_t instr_code;
		struct instr instruction;
	} pipeline;
};

#endif
