#include "random.h"

#include "rpi-random.h"

uint32_t random_int(uint32_t upper)
{
	return (RPI_GetRandom() % upper);
}
