#include "emulate_pi_state.h"
#include "emulate_fetch.h"
#include "emulate_decode.h"
#include "emulate_execute.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

static void print_state(struct pi_state *pstate)
{
	fprintf(stdout, "Registers:\n");
	for (int i = 0; i < 13; i++) {
		uint32_t val = pstate->registers[i];

		fprintf(stdout, "$%-2d : %10d (0x%08x)\n", i, val, val);
	}

	uint32_t pc = pstate->registers[R_PC];

	fprintf(stdout, "PC  : %10d (0x%08x)\n", pc, pc);

	uint32_t cpsr = (pstate->cpsr.n << 31) |
			(pstate->cpsr.z << 30) |
			(pstate->cpsr.c << 29) |
			(pstate->cpsr.v << 28);

	fprintf(stdout, "CPSR: %10d (0x%08x)\n", cpsr, cpsr);

	fprintf(stdout, "Non-zero memory:\n");

	uint8_t *mem = pstate->memory;
	size_t address = 0;

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

	struct pi_state *pstate = calloc(1, sizeof(struct pi_state));

	if (!pstate) {
		fprintf(stderr, "Not enough memory for Pi state\n");
		exit(1);
	}

	if (argc != 2) {
		fprintf(stderr, "Provide an input binary as an argument\n");
		exit(1);
	}

	input = fopen(argv[1], "rb");

	if (!input)
		fprintf(stderr, "Could not open input file\n");

	fread(&pstate->memory, PI_MEMORY_SIZE, 1, input);

	while (true) {
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
}
