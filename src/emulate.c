#include "emulate_fetch.h"
#include "emulate_decode.h"
#include "emulate_execute.h"

#include "pi_state.h"
#include "pi_msgs.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#define NUMBER_OF_GENERAL_PURPOSE_REGISTERS 12

#define CSPR_NEGATIVE_FLAG_BIT  31
#define CSPR_ZERO_FLAG_BIT      30
#define CSPR_CARRY_OUT_FLAG_BIT 29
#define CSPR_OVERFLOW_FLAG_BIT  28

#define MEMORY_BOUNDARY_ALLIGNEMENT_BYTES 4

static void print_state(struct pi_state *pstate)
{
	size_t i;
	uint32_t val, pc, cpsr;
	uint8_t *mem;
	size_t address;

	fprintf(stdout, EMU_STATE_REG_HEAD);
	for (i = 0; i <= NUMBER_OF_GENERAL_PURPOSE_REGISTERS; i++) {
		val = pstate->registers[i];
		fprintf(stdout, EMU_STATE_REG_GEN, i, val, val);
	}

	pc = pstate->registers[R_PC];
	fprintf(stdout, EMU_STATE_REG_PC, pc, pc);

	cpsr =  (pstate->cpsr.n << CSPR_NEGATIVE_FLAG_BIT) |
		(pstate->cpsr.z << CSPR_ZERO_FLAG_BIT)     |
		(pstate->cpsr.c << CSPR_CARRY_OUT_FLAG_BIT)|
		(pstate->cpsr.v << CSPR_OVERFLOW_FLAG_BIT);
	fprintf(stdout, EMU_STATE_REG_CPSR, cpsr, cpsr);

	fprintf(stdout, EMU_STATE_MEM_HEAD);
	mem = pstate->memory;
	for (address = 0; address < PI_MEMORY_SIZE; address += MEMORY_BOUNDARY_ALLIGNEMENT_BYTES) {
		if (!((uint32_t *)mem)[address / MEMORY_BOUNDARY_ALLIGNEMENT_BYTES]) {
			/* skip if word at address is zero */
			continue;
		}
		fprintf(stdout, EMU_STATE_MEM_BGN, address);
		for (i = 0; i < 4; i++)
			fprintf(stdout, EMU_STATE_MEM_BYTE, mem[address + i]);
		fprintf(stdout, EMU_STATE_MEM_END);
	}
}

int main(int argc, char **argv)
{
	FILE *input;
	struct pi_state *pstate;

	if (argc != 2) {
		fprintf(stderr, EMU_ERR_ARGS);
		goto fail;
	}

	pstate = calloc(1, sizeof(struct pi_state));
	if (!pstate) {
		fprintf(stderr, PI_ERR_MEM, "pi state");
		goto fail;
	}

	input = fopen(argv[1], "rb");
	if (!input) {
		fprintf(stderr, PI_ERR_INPUT, argv[1], strerror(errno), errno);
		goto fail;
	}

	fread(&pstate->memory, 1, PI_MEMORY_SIZE, input);
	fgetc(input); /* so that EOF is set if binary is exactly 64KiB */
	if (ferror(input)) {
		fprintf(stderr, PI_ERR_INPUT_IO, strerror(errno), errno);
		goto fail;
	}
	if (!feof(input)) {
		fprintf(stderr, EMU_ERR_BIN_SIZE);
		goto fail;
	}

	for (;;) {
		if (pstate->pipeline.decoded)
			if (execute(pstate) && !errno)
				break;
		if (pstate->pipeline.fetched)
			if (decode(pstate))
				break;
		if (fetch(pstate))
			break;
		pstate->registers[R_PC] += MEMORY_BOUNDARY_ALLIGNEMENT_BYTES;
	}

	print_state(pstate);

	return EXIT_SUCCESS;

fail:
	if (errno)
		fprintf(stderr, PI_ERR_GENERIC, strerror(errno), errno);
	return EXIT_FAILURE;
}
