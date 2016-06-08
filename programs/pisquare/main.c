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

#include "gamestate.h"
#include "typedefs.h"
#include "sprite.h"
#include "box.h"

#define SCREEN_WIDTH    512
#define SCREEN_HEIGHT   512
#define SCREEN_DEPTH    32      /* 16 or 32-bit */

static screen_t screen;

static void initialize_screen(colour_t current_colour)
{
	int offset;
	int r, g, b, a;
	int x, y;

	for (y = 0; y < screen.height; y++) {
		for (x = 0; x < screen.width; x++) {
			offset = (x * (screen.bpp >> 3)) + (y * screen.pitch);

			r = (int)(current_colour.r * 0xFF) & 0xFF;
			g = (int)(current_colour.g * 0xFF) & 0xFF;
			b = (int)(current_colour.b * 0xFF) & 0xFF;
			a = (int)(current_colour.b * 0xFF) & 0xFF;

			screen.fb[offset++] = r;
			screen.fb[offset++] = g;
			screen.fb[offset++] = b;
			screen.fb[offset++] = a;
		}
	}
}

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

	initialize_screen({1,1,1,0});
	
	game_state_t state = {.screen = screen};

	colour_t col = {0, 0, 0, 255};
	box_t test = {{20, 20}, {10, 10}, {2, 2}, {0, 0}, col};
	colour_t col2 = {0, 0, 255, 255};
	box_t test2 = {{20, 20}, {490, 10}, {-2, 2}, {0, 0}, col2};

	while (1) {

		RPI_WaitMicroSeconds(8000);
		box_draw_wipe(&state, &test);
		if ((test.size.x + test.pos.x) == SCREEN_WIDTH) {
			test.vel.x = -abs(test.vel.x);
			test.vel.y = -abs(test.vel.y);
		} else if (test.pos.x == 0) {
			test.vel.x = abs(test.vel.x);
			test.vel.y = abs(test.vel.y);
		}
		box_move(state, &test);
		box_draw(&state, &test);

		box_draw_wipe(&state, &test2);
		if ((test2.size.x + test2.pos.x) == SCREEN_WIDTH) {
			test2.vel.x = -abs(test2.vel.x);
			test2.vel.y = abs(test2.vel.y);
		} else if (test2.pos.x == 0) {
			test2.vel.x = abs(test2.vel.x);
			test2.vel.y = -abs(test2.vel.y);
		}
		box_move(&state, &test2);
		box_draw(&state, &test2);
	}
}
