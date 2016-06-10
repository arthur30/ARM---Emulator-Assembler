#include "dma.h"

#include <stdlib.h>
#include <string.h>

#include "rpi-mailbox-interface.h"

static dma_conblk_t *dma_blocks_mem;
static volatile dma_conblk_t *dma_blocks;

static volatile uint32_t *dma_enable = (void *)(DMA_BASE + DMA_ENABLE_OFFSET);

static uint32_t dma_usable;

static uint32_t zero;

static volatile dma_register_t *get_channel(int channel)
{
	return (void *)(DMA_BASE + DMA_OFFSET * (uintptr_t)channel);
}

int dma_init(void)
{
	uintptr_t ptr;
	rpi_mailbox_property_t *mp;

	*dma_enable = DMA_ENABLE_ALL;

	zero = 0;

	RPI_PropertyInit();
	RPI_PropertyAddTag(TAG_GET_DMA_CHANNELS);
	RPI_PropertyProcess();

	mp = RPI_PropertyGet(TAG_GET_PHYSICAL_SIZE);
	if (mp)
		dma_usable = mp->data.buffer_32[0];

	dma_blocks_mem = malloc(sizeof(dma_conblk_t) * (DMA_CHANNELS + 1));
	if (!dma_blocks_mem)
		return -1;
	ptr = ((uintptr_t)dma_blocks_mem+sizeof(dma_conblk_t)-1) &
		~(sizeof(dma_conblk_t) - 1);
	dma_blocks = (void *)ptr;

	return 0;
}

int dma_check_active(int ch)
{
	return (get_channel(ch)->cs & DMA_CS_ACTIVE);
}

int dma_check_usable(int ch)
{
	return dma_usable & (1 << ch);
}

static int dma(void *dst, const void *src, size_t len, uint32_t ti)
{
	int i;
	volatile dma_register_t *dma_reg;

	for (i = 0; i < DMA_CHANNELS; i++) {
		if (!dma_check_usable(i) && dma_check_active(i))
			continue;
		dma_blocks[i].ti = ti;
		dma_blocks[i].source_ad = (uintptr_t)src;
		dma_blocks[i].dest_ad = (uintptr_t)dst;
		dma_blocks[i].txfr_len = len;
		dma_blocks[i].stride = 0;
		dma_blocks[i].nextconbk = 0;
		dma_reg = get_channel(i);
		dma_reg->conblk_ad = (uintptr_t)(void *)&dma_blocks[i];
		dma_reg->cs |= DMA_CS_ACTIVE;
		
		return i;
	}

	return -1;
}

int dma_copy(void *dst, const void *src, size_t len)
{
	return dma(dst, src, len, DMA_TI_DEST_INC | DMA_TI_SRC_INC);
}

int dma_fill(void *dst, const void *src, size_t len)
{
	return dma(dst, src, len, DMA_TI_DEST_INC);
}

int dma_zero(void *dst, size_t len)
{
	return dma_fill(dst, &zero, len);
}
