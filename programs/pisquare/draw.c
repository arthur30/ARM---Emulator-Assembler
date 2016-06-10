#include "draw.h"

#include "renderer.h"

#include "assets/assets.h"

void draw_background(game_state_t *state)
{
	graphics_draw_image(state->device, &(vector2_t){0, 0}, background_bin);
}

void draw_rect(game_state_t *state, rect_t rect, color_t col)
{

	graphics_draw_rectangle(state->device, &col, &rect.origin, &rect.size);
}

void draw_box(game_state_t *state, box_t *box)
{
	draw_rect(state, entity_rect(box->entity), box->color);
}

void draw_player(game_state_t *state, player_t *player)
{
	/* Draw Player */
	draw_rect(state, entity_rect(player->entity), player->color);
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
