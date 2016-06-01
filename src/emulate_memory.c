#include "emulate_memory.h"

#include "pi_state.h"
#include "pi_msgs.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

static struct memrange ranges[] = {
	{0, PI_MEMORY_SIZE, NULL, NULL},
	{GPIO_CONTROL_ADDRESS, PI_WORD_SIZE, NULL,
	 EMU_RUN_GPIO_PIN_ACCESS1},
	{GPIO_CONTROL_ADDRESS + PI_WORD_SIZE, PI_WORD_SIZE, NULL,
	 EMU_RUN_GPIO_PIN_ACCESS2},
	{GPIO_CONTROL_ADDRESS + 2 * PI_WORD_SIZE, PI_WORD_SIZE, NULL,
	 EMU_RUN_GPIO_PIN_ACCESS3},
	{GPIO_TURNON_ADDRESS, PI_WORD_SIZE, NULL, EMU_RUN_GPIO_PIN_ON},
	{GPIO_CLEARING_ADDRESS, PI_WORD_SIZE, NULL, EMU_RUN_GPIO_PIN_OFF},
};

int init_pi_memory(struct pi_state *pstate)
{
	size_t i;
	size_t size;
	uint32_t val;

	size = sizeof(ranges) / sizeof(struct memrange);

	pstate->memory.count = size;
	pstate->memory.ranges = malloc(sizeof(ranges));
	if (!pstate->memory.ranges)
		return -1;
	memcpy(pstate->memory.ranges, ranges, sizeof(ranges));

	for (i = 0; i < size; i++) {
		pstate->memory.ranges[i].store = calloc(ranges[i].size, 1);
		if (!pstate->memory.ranges[i].store)
			return -1;
	}

	val = GPIO_CONTROL_ADDRESS;
	memcpy(pstate->memory.ranges[1].store, &val, PI_WORD_SIZE);
	val += PI_WORD_SIZE;
	memcpy(pstate->memory.ranges[2].store, &val, PI_WORD_SIZE);
	val += PI_WORD_SIZE;
	memcpy(pstate->memory.ranges[3].store, &val, PI_WORD_SIZE);

	return 0;
}

static int memrange_cmp(const void *vaddress, const void *vrange)
{
	const size_t *address;
	const struct memrange *range;

	address = vaddress;
	range = vrange;

	if (*address < range->base)
		return -1;
	if (*address >= range->base + range->size)
		return 1;
	return 0;
}

uint8_t *get_memory(struct pi_state *pstate, size_t address)
{
	struct memrange *range;

	range = bsearch(&address, pstate->memory.ranges,
		      pstate->memory.count, sizeof(struct memrange),
		      memrange_cmp);
	if (range) {
		if (range->msg)
			fprintf(stdout, "%s", range->msg);
		return &range->store[address - range->base];
	}

	errno = EFAULT;
	fprintf(stdout, EMU_RUN_OUT_OF_BOUNDS_MEM, address);

	return NULL;
}
