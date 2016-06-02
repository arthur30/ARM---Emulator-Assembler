#ifndef PI_STATE_H
#define PI_STATE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define PI_MEMORY_SIZE (1 << 16)
#define PI_REGISTER_COUNT 17
#define PI_WORD_SIZE 4

#define R_PC 15
#define R_CPSR 16

#define REG_ADDR_BIT_COUNT 4

#define CPSR_BIT_N (1 << 31)
#define CPSR_BIT_Z (1 << 30)
#define CPSR_BIT_C (1 << 29)
#define CPSR_BIT_V (1 << 28)

#define INSTR_BIT_IMM     (1 << 25)
#define INSTR_BIT_SETCOND (1 << 20)
#define INSTR_BIT_ACC     (1 << 21)
#define INSTR_BIT_INDEX   (1 << 24)
#define INSTR_BIT_UP      (1 << 23)
#define INSTR_BIT_LOAD    (1 << 20)
#define INSTR_BIT_CONST   (1 <<  4)

#define INSTR_COND_COUNT 15

#define COND_FIRST_BIT      28
#define COND_NUMBER_OF_BITS 4

#define INSTR_DATA_PROC_MASK 0x0C000000
#define INSTR_DATA_PROC_BITP 0x00000000

#define INSTR_DPI_OPCODE_COUNT 16

#define INSTR_SHIFT_COUNT 4

#define INSTR_MULT_MASK      0x0FC000F0
#define INSTR_MULT_BITP      0x00000090

#define INSTR_MULT_MASK       0x0FC000F0
#define INSTR_MULT_BITP       0x00000090

#define INSTR_TRANSFER_MASK   0x0C600000
#define INSTR_TRANSFER_BITP   0x04000000

#define INSTR_BRANCH_MASK     0x0F000000
#define INSTR_BRANCH_BITP     0x0A000000

#define GPIO_CONTROL_ADDRESS  0x20200000
#define GPIO_CONTROL_SIZE     12
#define GPIO_CLEARING_ADDRESS 0x20200028
#define GPIO_CLEARING_SIZE    4
#define GPIO_TURNON_ADDRESS   0x2020001C
#define GPIO_TURNON_SIZE      4

/* Data processing Operand2(Case: Immediate value) constants: */
#define OP2_IMMEDIATE_VALUE_FIRST_BIT           0
#define OP2_IMMEDIATE_VALUE_LAST_BIT            8
#define OP2_IMMEDIATE_VALUE_ROTATION_FIRST_BIT  8

/* Data processing Operand2(Case: Register) constants: */
#define OP2_REG_RM_FIRST_BIT			0
#define OP2_SHIFT_REG_TYPE_FIRST_BIT		5
#define OP2_SHIFT_REG_TYPE_BIT_COUNT		2
#define OP2_SHIFT_REG_CONST_FIRST_BIT		7
#define OP2_SHIFT_REG_CONST_BIT_COUNT		5
#define OP2_SHIFT_REG_REG_FIRST_BIT		8

/* Data processing constants: */
#define DATA_PROC_RD_FIRST_BIT     12
#define DATA_PROC_RN_FIRST_BIT     16
#define DATA_PROC_OPCODE_FIRST_BIT 21

/* Decode multiply constants: */
#define MULT_RM_REG_FIRST_BIT 0
#define MULT_RS_REG_FIRST_BIT 8
#define MULT_RN_REG_FIRST_BIT 12
#define MULT_RD_REG_FIRST_BIT 16

/* Single data transfer constants: */
#define TRANSFER_OFFSET_FIRST_BIT 0
#define TRANSFER_OFFSET_LAST_BIT  12
#define TRANSFER_RD_FIRST_BIT     12
#define TRANSFER_RN_FIRST_BIT     16

/* Branch constants: */
#define BRANCH_OFFSET_FIRST_BIT 0
#define BRANCH_OFFSET_BIT_COUNT	24

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
	INSTR_TYPE_HALT = 0,
	INSTR_TYPE_DATA_PROC = 1,
	INSTR_TYPE_MULT = 2,
	INSTR_TYPE_TRANSFER = 3,
	INSTR_TYPE_BRANCH = 4
};

enum instr_cond {
	INSTR_COND_EQ = 0,
	INSTR_COND_NE = 1,
	INSTR_COND_CS = 2,
	INSTR_COND_CC = 3,
	INSTR_COND_MI = 4,
	INSTR_COND_PL = 5,
	INSTR_COND_VS = 6,
	INSTR_COND_VC = 7,
	INSTR_COND_HI = 8,
	INSTR_COND_LS = 9,
	INSTR_COND_GE = 10,
	INSTR_COND_LT = 11,
	INSTR_COND_GT = 12,
	INSTR_COND_LE = 13,
	INSTR_COND_AL = 14,
};

enum instr_dpi_opcode {
	INSTR_DPI_AND = 0,
	INSTR_DPI_EOR = 1,
	INSTR_DPI_SUB = 2,
	INSTR_DPI_RSB = 3,
	INSTR_DPI_ADD = 4,
	INSTR_DPI_TST = 8,
	INSTR_DPI_TEQ = 9,
	INSTR_DPI_CMP = 10,
	INSTR_DPI_ORR = 12,
	INSTR_DPI_MOV = 13,
	INSTR_DPI_LSL = 16
};

enum instr_shift_type {
	INSTR_SHIFT_LSL = 0,
	INSTR_SHIFT_LSR = 1,
	INSTR_SHIFT_ASR = 2,
	INSTR_SHIFT_ROR = 3,
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

struct memrange {
	size_t base;
	size_t size;
	uint8_t *store;
	const char *msg;
};

struct pi_memory {
	size_t count;
	struct memrange *ranges;
};

struct pi_state {
	struct pi_memory memory;
	uint8_t gpio_control[GPIO_CONTROL_SIZE];
	uint32_t registers[PI_REGISTER_COUNT];
	struct pipeline {
		bool fetched;
		bool decoded;
		uint32_t instr_code;
		struct instr instruction;
	} pipeline;
};

#endif
