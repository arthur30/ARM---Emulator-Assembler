#include "emulate_pi_state.h"
#include "emulate_fetch.h"
#include "emulate_decode.h"
#include "emulate_execute.h"

#include "pi_msgs.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

static void print_state(struct pi_state *pstate)
{
	int i;
	uint32_t val, pc, cpsr;
	uint8_t *mem;
	size_t address;

	fprintf(stdout, EMU_STATE_REG_HEAD);
	for (i = 0; i < 13; i++) {
		val = pstate->registers[i];
		fprintf(stdout, EMU_STATE_REG_GEN, i, val, val);
	}

	pc = pstate->registers[R_PC];
	fprintf(stdout, EMU_STATE_REG_PC, pc, pc);

	cpsr = (pstate->cpsr.n << 31) |
		(pstate->cpsr.z << 30) |
		(pstate->cpsr.c << 29) |
		(pstate->cpsr.v << 28);
	fprintf(stdout, EMU_STATE_REG_CPSR, cpsr, cpsr);

	fprintf(stdout, EMU_STATE_MEM_HEAD);
	mem = pstate->memory;
	address = 0;
	for (address = 0; address < PI_MEMORY_SIZE; address += 4) {
		if (mem[address] |
		    mem[address + 1] |
		    mem[address + 2] |
		    mem[address + 3])
			fprintf(stdout, EMU_STATE_MEM_VAL,
				address,
				pstate->memory[address],
				pstate->memory[address + 1],
				pstate->memory[address + 2],
				pstate->memory[address + 3]);
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
		pstate->registers[R_PC] += 4;
	}

	print_state(pstate);

	return EXIT_SUCCESS;

fail:
	if (errno)
		fprintf(stderr, PI_ERR_GENERIC, strerror(errno), errno);
	return EXIT_FAILURE;
}
