#include "game.h"

static box_t test, test2;

static void initialize_screen(game_state_t *state, colour_t colour)
{
	/*int offset;
	*int r, g, b, a;
	*int x, y;

	*for (y = 0; y < state->screen.height; y++) {
	*	for (x = 0; x < state->screen.width; x++) {
	*		offset = (x * (state->screen.bpp >> 3))
	*		       + (y * state->screen.pitch);

	*		r = (int)(colour.r * 0xFF) & 0xFF;
	*		g = (int)(colour.g * 0xFF) & 0xFF;
	*		b = (int)(colour.b * 0xFF) & 0xFF;
	*		a = (int)(colour.b * 0xFF) & 0xFF;

	*		state->screen.fb[offset++] = r;
	*		state->screen.fb[offset++] = g;
	*		state->screen.fb[offset++] = b;
	*		state->screen.fb[offset++] = a;
	*	}
	*}
	*/
	rect_fill(state,
	       (rect_t){{0, 0}, {state->screen.width, state->screen.height} },
	       colour);
}

void game_init(game_state_t *state)
{
	initialize_screen(state, (colour_t){1, 1, 1, 0});

	colour_t col = (colour_t){220, 120, 80, 255};

	test = (box_t){{20, 20}, {10, 10}, {2, 2}, {0, 0}, col};

	colour_t col2 = (colour_t){0, 0, 255, 255};

	test2 = (box_t){{20, 20}, {490, 10}, {-2, 2}, {0, 0}, col2};
}

void game_update(game_state_t *state, float delta)
{
	(void)delta;
	box_draw_wipe(state, &test);
	if ((test.size.x + test.pos.x) == state->screen.width) {
		test.vel.x = -abs(test.vel.x);
		test.vel.y = -abs(test.vel.y);
	} else if (test.pos.x == 0) {
		test.vel.x = abs(test.vel.x);
		test.vel.y = abs(test.vel.y);
	}
	box_move(state, &test);
	box_draw(state, &test);

	box_draw_wipe(state, &test2);
	if ((test2.size.x + test2.pos.x) == state->screen.width) {
		test2.vel.x = -abs(test2.vel.x);
		test2.vel.y = abs(test2.vel.y);
	} else if (test2.pos.x == 0) {
		test2.vel.x = abs(test2.vel.x);
		test2.vel.y = -abs(test2.vel.y);
	}
	box_move(state, &test2);
	box_draw(state, &test2);
}

void game_draw(game_state_t *state)
{
	(void)state;
}
