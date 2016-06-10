#include "random.h"

#include "rpi-random.h"

int random_int(int upper)
{
	return (RPI_GetRandom() % upper);
}
