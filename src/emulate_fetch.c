#include "emulate_fetch.h"
#include "emulate_memory.h"

#include "pi_state.h"

#include <string.h>
#include <errno.h>

/* fetch instruction from memory */
int fetch(struct pi_state *pstate)
{
	uint32_t nextinstr;
	uint8_t *mem;

	if (pstate->registers[R_PC] >= PI_MEMORY_SIZE) {
		errno = EFAULT;
		return -1;
	}

	nextinstr = 0;
	mem = get_memory(pstate, pstate->registers[R_PC]);
	memcpy(&nextinstr, mem, sizeof(nextinstr));
	pstate->pipeline.fetched = true;
	pstate->pipeline.instr_code = nextinstr;
	return 0;
}
