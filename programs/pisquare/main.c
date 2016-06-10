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
#include "dma.h"
#include "draw.h"
#include "player.h"

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

	/* Initialize DMA */
	dma_init();

	state.device = graphics_create(SCREEN_WIDTH,
				       SCREEN_HEIGHT,
				       SCREEN_DEPTH);
	state.player_count = 0;

	draw_splash(&state);

	RPI_WaitMicroSeconds(800000);

	RPI_SetGpioInput(PLAYER_1_RIGHT);
	RPI_GetGpio()->GPPUD = 2;
	RPI_SetGpioInput(PLAYER_1_LEFT);
	RPI_GetGpio()->GPPUD = 2;

	while (RPI_GetGpioValue(PLAYER_1_RIGHT) != 0) {
		draw_background(&state);
		print_text(&state, "MENU",
				(vector2_t){221, 161});
		print_text(&state, "L : SELECT    R : START",
				(vector2_t){36, 484});

		print_text(&state, "1 PLAYER", (vector2_t){181, 201});
		print_text(&state, "2 PLAYER", (vector2_t){181, 231});
		print_text(&state, "3 PLAYER", (vector2_t){181, 261});
		print_text(&state, "4 PLAYER", (vector2_t){181, 291});

		if (RPI_GetGpioValue(PLAYER_1_LEFT) == 0)
			state.player_count = (state.player_count + 1) % 4;

		switch (state.player_count + 1) {
		case 1:
			print_text_color(&state, "1 PLAYER",
					(vector2_t){181, 201},
					(color_t){0, 255, 0, 255});
			break;
		case 2:
			print_text_color(&state, "2 PLAYER",
					(vector2_t){181, 231},
					(color_t){0, 255, 0, 255});
			break;
		case 3:
			print_text_color(&state, "3 PLAYER",
					(vector2_t){181, 261},
					(color_t){0, 255, 0, 255});
			break;
		case 4:
			print_text_color(&state, "4 PLAYER",
					(vector2_t){181, 291},
					(color_t){0, 255, 0, 255});
			break;
		}

		graphics_flush(state.device);
	}

	game_init(&state);

	uint32_t diff, new;

	float secondsElapsed;

	rpi_sys_timer_t *timer = RPI_GetSystemTimer();
	uint32_t prev = timer->counter_lo;

	RPI_SetGpioInput(RPI_GPIO18);

	while (1) {
		/* time delta */
		new = timer->counter_lo;
		diff = new - prev;
		prev = new;
		secondsElapsed = (float)diff/1000000;

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
