#include "emulate_fetch.h"
#include "emulate_pi_state.h"

#include <errno.h>

int fetch(struct pi_state *pstate)
{
	uint32_t nextinstr;
	uint8_t *mem;

	if (pstate->registers[R_PC] >= PI_MEMORY_SIZE) {
		errno = EFAULT;
		return -1;
	}

	nextinstr = 0;
	mem = &pstate->memory[pstate->registers[R_PC]];
	nextinstr |= (uint32_t)(*mem++);
	nextinstr |= (uint32_t)(*mem++) << 8;
	nextinstr |= (uint32_t)(*mem++) << 16;
	nextinstr |= (uint32_t)(*mem++) << 24;
	pstate->pipeline.fetched = true;
	pstate->pipeline.instr_code = nextinstr;
	return 0;
}
