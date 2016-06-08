#ifndef DMA_H
#define DMA_H

#include <stdlib.h>
#include <stdint.h>

#define DMA_BASE     0x20007000
#define DMA_OFFSET   0x100
#define DMA_CHANNELS 15

#define DMA_ENABLE_OFFSET 0xFF0

#define DMA_ENABLE_ALL 0xFFFF

#define DMA_CS_ACTIVE 0x1
#define DMA_CS_RESET  0x80000000

typedef struct dma_register {
	volatile uint32_t cs;
	volatile uint32_t conblk_ad;
	volatile uint32_t ti;
	volatile uint32_t source_ad;
	volatile uint32_t dest_ad;
	volatile uint32_t txfr_len;
	volatile uint32_t stride;
	volatile uint32_t nextconbk;
	volatile uint32_t debug;
} dma_register_t;

typedef struct dma_conblk {
	uint32_t ti;
	uint32_t source_ad;
	uint32_t dest_ad;
	uint32_t txfr_len;
	uint32_t stride;
	uint32_t nextconbk;
	uint32_t reserved1;
	uint32_t reserved2;
} dma_conblk_t;

int dma_init(void);
int dma(void *dst, const void *sr, size_t lenc);
int dma_check_active(int ch);
int dma_wait(int ch);

#endif
