#include "main.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "rpi-main.h"
#include "rpi-armtimer.h"
#include "rpi-gpio.h"
#include "rpi-interrupts.h"
#include "rpi-mailbox-interface.h"
#include "rpi-systimer.h"

#include "game.h"

#define SCREEN_WIDTH    512
#define SCREEN_HEIGHT   512
#define SCREEN_DEPTH    32      /* 16 or 32-bit */

static game_state_t state;
static screen_t screen;

void kernel_main(unsigned int r0, unsigned int r1, unsigned int atags)
{
	(void)r0;
	(void)r1;
	(void)atags;

	screen.height = SCREEN_HEIGHT;
	screen.width = SCREEN_WIDTH;
	screen.bpp = SCREEN_DEPTH;
	screen.pitch = 0;
	screen.fb = NULL;

	rpi_mailbox_property_t *mp;

	/* Write 1 to the LED init nibble in the Function Select GPIO
	 * peripheral register to enable LED pin as an output
	 */
	RPI_GetGpio()->LED_GPFSEL |= LED_GPFBIT;

	/* Enable the timer interrupt IRQ */
	RPI_GetIrqController()->Enable_Basic_IRQs = RPI_BASIC_ARM_TIMER_IRQ;

	/* Setup the system timer interrupt */
	/* Timer frequency = Clk/256 * 0x400 */
	RPI_GetArmTimer()->Load = 0x400;

	/* Setup the ARM Timer */
	RPI_GetArmTimer()->Control =
		RPI_ARMTIMER_CTRL_23BIT |
		RPI_ARMTIMER_CTRL_ENABLE |
		RPI_ARMTIMER_CTRL_INT_ENABLE |
		RPI_ARMTIMER_CTRL_PRESCALE_256;

	/* Enable interrupts! */
	_enable_interrupts();

	RPI_PropertyInit();
	RPI_PropertyAddTag(TAG_GET_BOARD_MODEL);
	RPI_PropertyProcess();

	mp = RPI_PropertyGet(TAG_GET_BOARD_MODEL);

	/* Ensure the ARM is running at it's maximum rate */
	RPI_PropertyInit();
	RPI_PropertyAddTag(TAG_SET_CLOCK_RATE, TAG_CLOCK_ARM,
					mp->data.buffer_32[1]);
	RPI_PropertyProcess();

	RPI_PropertyInit();
	RPI_PropertyAddTag(TAG_GET_CLOCK_RATE, TAG_CLOCK_ARM);
	RPI_PropertyProcess();

	/* Initialise a framebuffer... */
	RPI_PropertyInit();
	RPI_PropertyAddTag(TAG_ALLOCATE_BUFFER);
	RPI_PropertyAddTag(TAG_SET_PHYSICAL_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT);
	RPI_PropertyAddTag(TAG_SET_VIRTUAL_SIZE, SCREEN_WIDTH,
						SCREEN_HEIGHT * 2);
	RPI_PropertyAddTag(TAG_SET_DEPTH, SCREEN_DEPTH);
	RPI_PropertyAddTag(TAG_GET_PITCH);
	RPI_PropertyAddTag(TAG_GET_PHYSICAL_SIZE);
	RPI_PropertyAddTag(TAG_GET_DEPTH);
	RPI_PropertyProcess();

	mp = RPI_PropertyGet(TAG_GET_PHYSICAL_SIZE);
	if (mp) {
		screen.width = mp->data.buffer_32[0];
		screen.height = mp->data.buffer_32[1];
	}

	mp = RPI_PropertyGet(TAG_GET_DEPTH);
	if (mp)
		screen.bpp = mp->data.buffer_32[0];

	mp = RPI_PropertyGet(TAG_GET_PITCH);
	if (mp)
		screen.pitch = mp->data.buffer_32[0];

	mp = RPI_PropertyGet(TAG_ALLOCATE_BUFFER);
	if (mp)
		screen.fb = (unsigned char *)mp->data.buffer_32[0];

	state = (game_state_t){.screen = screen};

	game_init(&state);

	while (1) {
		/*time difference
		*modify buffer ('wipe') / pre-render
		*update
		*write full frame
		*dma transfer
		*/
		game_update(&state, 0);
		/* game_draw(&state);*/
		RPI_WaitMicroSeconds(8000);
	}
}
