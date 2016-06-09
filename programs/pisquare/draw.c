#include "draw.h"

void draw_rect(game_state_t *state, rect_t rect, colour_t col)
{
	int offset;
	int size_vertical =
		min(rect.size.y, state->area.y - rect.origin.y);
	int size_horizontal =
		min(rect.size.x, state->area.x - rect.origin.x);
	int offset_vertical = max(-rect.origin.y, 0);
	int offset_horizontal = max(-rect.origin.x, 0);

	for (int y = offset_vertical; y < size_vertical; y++) {
		for (int x = offset_horizontal; x < size_horizontal; x++) {
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

void draw_box(game_state_t *state, box_t *box)
{
	draw_rect(state, entity_rect(box->entity), box->colour);
}

void draw_player(game_state_t *state, player_t *player)
{
	/* Draw Player */
	draw_rect(state, entity_rect(player->entity), player->colour);

	/* Draw Direction */
	vector_t pos = (vector_t){player->entity->pos.x
			+ player->entity->vel.x * state->delta * 10 - 2,
			  player->entity->pos.y
			+ player->entity->vel.y * state->delta * 10 - 2};

	draw_rect(state, (rect_t){pos, {4, 4} }, player->colour);
}

void draw_sprite(game_state_t *state, sprite_t *sprt)
{
	int pixel_offset;
	int i = 0;

	(void)i;
	(void)pixel_offset;
	(void)sprt;
	(void)state;
	/*
	*for (int y = 0; y < sprt->size.y; y++) {
	*	for (int x = 0; x < sprt->size.x; x++) {
	*		pixel_offset = ((xf + x) * (screen->bpp >> 3)) +
	*				((yf + y) * screen->pitch);
	*		screen->fb[pixel_offset++] = (int)sprt->image[i++];
	*		screen->fb[pixel_offset++] = (int)sprt->image[i++];
	*		screen->fb[pixel_offset++] = (int)sprt->image[i++];
	*	}
	*}
	*/
}
