#include "rpi-random.h"

#include <stdint.h>

static int random_initialised = 0;

void RPI_InitRandom(void)
{
	if (!random_initialised) {
		random_initialised = 1;

		*(volatile uint32_t *)RPI_HW_RNG_STATUS = RNG_WARMUP_COUNT;
		*(volatile uint32_t *)RPI_HW_RNG_CTRL = RPI_HW_RNG_CTRL_EN;
	}
}

uint32_t RPI_GetRandom(void)
{
	if (random_initialised) {
		while ((*(volatile uint32_t *)RPI_HW_RNG_STATUS >> 24) == 0);

		return (uint32_t)(*(volatile uint32_t *)RPI_HW_RNG_DATA);
	}

	return -1;
}
