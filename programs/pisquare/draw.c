#include "draw.h"

#include "renderer.h"

#include "assets/assets.h"

void draw_background(game_state_t *state)
{
	graphics_draw_image(state->device, &(vector2_t){0, 0}, background_bin);
}

void draw_rect(game_state_t *state, rect_t *rect, color_t col)
{
	graphics_draw_rectangle(state->device, &col, &rect->origin,
								&rect->size);
}

void draw_box(game_state_t *state, box_t *box)
{
	rect_t rect;

	draw_rect(state, entity_rect(&rect, box->entity), box->color);
}

void draw_player(game_state_t *state, player_t *player)
{
	rect_t rect;

	draw_rect(state, entity_rect(&rect, player->entity), player->color);
}
