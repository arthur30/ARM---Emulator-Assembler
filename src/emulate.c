#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define R_PC 15

struct instr_data_proc {
	bool immediate;
	uint8_t opcode;
	uint8_t rn;
	uint8_t rd;
	uint16_t op2;
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
	bool immediate;
	bool preindexing;
	bool up;
	bool load;
	uint8_t rn;
	uint8_t rd;
	uint16_t offset;
};

struct instr_branch {
	uint32_t offset;
};

struct instr {
	uint8_t cond;
	uint8_t type;
	union instr_bits_t {
		struct instr_data_proc data_proc;
		struct instr_mult mult;
		struct instr_transfer transfer;
		struct instr_branch branch;
	} instr_bits;
};

struct pi_state {
	uint8_t memory[1<<16];
	uint32_t registers[16];
	struct cpsr {
		bool n;
		bool z;
		bool c;
		bool v;
	} cpsr;
	struct pipeline {
		bool fetched;
		uint32_t instr_code;
		struct instr instruction;
	} pipeline;
};

static int fetch(struct pi_state *pstate)
{
	(void)pstate;
	return 0;
}

static int decode(struct pi_state *pstate)
{
	(void)pstate;
	return 0;
}

static int execute(struct pi_state *pstate)
{
	(void)pstate;
	return 0;
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	struct pi_state *pstate = calloc(1, sizeof(struct pi_state));

	while (true) {
		if (pstate->pipeline.instruction.type)
			execute(pstate);
		if (pstate->pipeline.fetched)
			decode(pstate);
		fetch(pstate);
	}

	return EXIT_SUCCESS;
}
