#ifndef PI_STATE_H
#define PI_STATE_H

#include <stdint.h>
#include <stdbool.h>

#define PI_MEMORY_SIZE (1 << 16) /* Size of memory: 2^16 = 65536 bytes */
#define PI_REGISTER_COUNT 16 /* number of available registers */

#define R_PC 15 /* Program counter is register 15 */
#define NUMBER_OF_BITS_TO_EXTRACT_FOR_REGISTERS 4 /* each register is represented by 4 bits */

#define INSTR_BIT_IMM     (1 << 25)
#define INSTR_BIT_SETCOND (1 << 20)
#define INSTR_BIT_ACC     (1 << 21)
#define INSTR_BIT_INDEX   (1 << 24)
#define INSTR_BIT_UP      (1 << 23)
#define INSTR_BIT_LOAD    (1 << 20)
#define INSTR_BIT_CONST   (1 <<  4)

#define COND_FIRST_BIT      28
#define COND_NUMBER_OF_BITS 4

/* All masks and bit patterns needed for decoding the instruction type: */

#define INSTR_DATA_PROC_MASK  0x0C000000
#define INSTR_DATA_PROC_BITP  0x00000000

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
#define OP2_IMMEDIATE_VALUE_NUMBER_OF_ROTATIONS 4
#define OP2_IMMEDIATE_VALUE_ROTATION_FIRST_BIT  8

/* Data processing Operand2(Case: Register) constants: */
#define OP2_REGISTER_RM_FIRST_BIT                  0
#define OP2_REGISTER_SHIFT_TYPE_FIRST_BIT          5
#define OP2_REGISTER_SHIFT_TYPE_NUMBER_OF_BITS     2
#define OP2_REGISTER_SHIFT_CONSTANT_FIRST_BIT      7
#define OP2_REGISTER_SHIFT_CONSTANT_NUMBER_OF_BITS 5
#define OP2_REGISTER_SHIFT_REGISTER_FIRST_BIT      8

/* Data processing constants: */
#define DATA_PROC_RD_FIRST_BIT     12
#define DATA_PROC_RN_FIRST_BIT     16
#define DATA_PROC_OPCODE_FIRST_BIT 21

/* Decode multiply constants: */
#define MULT_RM_REGISTER_FIRST_BIT 0
#define MULT_RS_REGISTER_FIRST_BIT 8
#define MULT_RN_REGISTER_FIRST_BIT 12
#define MULT_RD_REGISTER_FIRST_BIT 16

/* Single data transfer constants: */
#define TRANSFER_OFFSET_FIRST_BIT 0
#define TRANSFER_OFFSET_LAST_BIT  12
#define TRANSFER_RD_FIRST_BIT     12
#define TRANSFER_RN_FIRST_BIT     16

/* Branch constants: */
#define BRANCH_OFFSET_FIRST_BIT 0
#define BRANCH_OFFSET_LAST_BIT  24

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
	uint8_t gpio_control[GPIO_CONTROL_SIZE];
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
