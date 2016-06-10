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
#include "text.h"
#include "renderer.h"

#define SCREEN_WIDTH    512
#define SCREEN_HEIGHT   512
#define SCREEN_DEPTH    32      /* 16 or 32-bit */

static game_state_t state;

void kernel_main(unsigned int r0, unsigned int r1, unsigned int atags)
{
	rpi_mailbox_property_t *mp;

	(void)r0;
	(void)r1;
	(void)atags;

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

	state.device = graphics_create(SCREEN_WIDTH,
				       SCREEN_HEIGHT,
				       SCREEN_DEPTH);

	game_splash(&state);
	graphics_flush(state.device);

	RPI_WaitMicroSeconds(100000);

	game_init(&state);

	uint32_t diff, new;

	long double secondsElapsed;

	rpi_sys_timer_t *timer = RPI_GetSystemTimer();
	uint32_t prev = timer->counter_lo;

	RPI_SetGpioInput(RPI_GPIO18);

	while (1) {
		/* time delta */
		new = timer->counter_lo;
		diff = new - prev;
		prev = new;
		secondsElapsed = (long double)diff/1000000;

		state.delta = secondsElapsed;

		/* update */
		game_update(&state);
		/* draw */
		game_draw(&state);
		/*write full frame*/
		graphics_flush(state.device);

		/* check game state transition conditions */
		/* perform transition */
	}
}
