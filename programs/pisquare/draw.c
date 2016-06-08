#include "draw.h"

void rect_fill(game_state_t *state, rect_t rect, colour_t col)
{
	int offset;

	for (int y = 0; y < rect.size.y; y++) {
		for (int x = 0; x < rect.size.x; x++) {
			offset = (x + rect.origin.x)
			       * (state->screen.bpp >> 3)
			       + ((y + rect.origin.y) * state->screen.pitch);
			state->screen.fb[offset++] = col.b;
			state->screen.fb[offset++] = col.g;
			state->screen.fb[offset++] = col.r;
			state->screen.fb[offset++] = col.a;
		}
	}
}
