#ifndef RPI_RANDOM_H
#define RPI_RANDOM_H

#include "rpi-base.h"

#define RPI_HW_RNG_BASE		(PERIPHERAL_BASE + 0x104000)
#define RPI_HW_RNG_CTRL		(RPI_HW_RNG_BASE + 0x00)
#define RPI_HW_RNG_CTRL_EN	0x01
#define RPI_HW_RNG_STATUS	(RPI_HW_RNG_BASE + 0x04)
#define RPI_HW_RNG_DATA		(RPI_HW_RNG_BASE + 0x08)
#define RNG_WARMUP_COUNT	0x40000

void RPI_InitRandom(void);
uint32_t RPI_GetRandom(void);

#endif
