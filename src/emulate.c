#include "emulate_pi_state.h"
#include "emulate_fetch.h"
#include "emulate_decode.h"
#include "emulate_execute.h"
#include "emulate_errors.h"

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

	fprintf(stdout, "Registers:\n");
	for (i = 0; i < 13; i++) {
		val = pstate->registers[i];
		fprintf(stdout, "$%-2d : %10d (0x%08x)\n", i, val, val);
	}

	pc = pstate->registers[R_PC];
	fprintf(stdout, "PC  : %10d (0x%08x)\n", pc, pc);

	cpsr = (pstate->cpsr.n << 31) |
		(pstate->cpsr.z << 30) |
		(pstate->cpsr.c << 29) |
		(pstate->cpsr.v << 28);
	fprintf(stdout, "CPSR: %10d (0x%08x)\n", cpsr, cpsr);

	fprintf(stdout, "Non-zero memory:\n");
	mem = pstate->memory;
	address = 0;
	for (address = 0; address < PI_MEMORY_SIZE; address += 4) {
		if (mem[address] |
		    mem[address + 1] |
		    mem[address + 2] |
		    mem[address + 3])
			fprintf(stdout, "0x%08zx: 0x%02x%02x%02x%02x\n",
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
		fprintf(stderr, NOT_ENOUGH_ARGS);
		goto fail;
	}

	pstate = calloc(1, sizeof(struct pi_state));
	if (!pstate)
		goto fail;

	input = fopen(argv[1], "rb");
	if (!input)
		goto fail;

	if (!fread(&pstate->memory, 1, PI_MEMORY_SIZE, input))
		goto fail;
	if (ferror(input))
		goto fail;
	if (!feof(input)) {
		fprintf(stderr, BINARY_TOO_LARGE);
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
		fprintf(stderr, "%s (errno: %d)\n", strerror(errno), errno);
	return EXIT_FAILURE;
}
