#include "dma.h"

#include <stdlib.h>
#include <string.h>

static dma_conblk_t *dma_blocks_mem;
static dma_conblk_t *dma_blocks;

static volatile uint32_t *dma_enable = (void *)(DMA_BASE + DMA_ENABLE_OFFSET);

static volatile dma_register_t *get_channel(int channel)
{
	return (void *)(DMA_BASE + DMA_OFFSET * (uintptr_t)channel);
}

int dma_init(void)
{
	uintptr_t ptr;

	dma_blocks_mem = malloc(sizeof(dma_conblk_t) * (DMA_CHANNELS + 1));
	if (!dma_blocks_mem)
		return -1;
	ptr = ((uintptr_t)dma_blocks_mem+sizeof(dma_register_t)-1) &
		~sizeof(dma_register_t);
	dma_blocks = (void *)ptr;

	*dma_enable = DMA_ENABLE_ALL;

	return 0;
}

int dma_check_active(int ch)
{
	return get_channel(ch)->cs & DMA_CS_ACTIVE;
}

int dma(void *dst, const void *src, size_t len)
{
	int i;
	volatile dma_register_t *dma_reg;

	for (i = 0; i < DMA_CHANNELS; i++) {
		if (dma_check_active(i))
			continue;
		memset(&dma_blocks[i], 0, sizeof(dma_conblk_t));
		dma_blocks[i].ti = (1 << 4) | (1 << 8);
		dma_blocks[i].dest_ad = (uintptr_t)dst;
		dma_blocks[i].source_ad = (uintptr_t)src;
		dma_blocks[i].txfr_len = len;
		dma_reg = get_channel(i);
		dma_reg->conblk_ad = (uintptr_t)(void *)&dma_blocks[i];
		dma_reg->cs = DMA_CS_ACTIVE;
		return i;
	}

	return -1;
}

int dma_wait(int ch)
{
	while (dma_check_active(ch))
		;
	return 0;
}
