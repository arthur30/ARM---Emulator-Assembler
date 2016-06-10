#include "draw.h"

#include "renderer.h"

#include "assets/assets.h"

void draw_splash(game_state_t *state)
{
	graphics_draw_image(state->device, &(vector2_t){0, 0},
			    splash_bin, NULL);

	cvertex_t tr[3] = {
		[0] = {
			.pos = { .x = 0, .y = 0 },
			.color = { .r = 255, .g = 0, .b = 0, .a = 255 }
		},
		[1] = {
			.pos = { .x = 0, .y = 200 },
			.color = { .r = 0, .g = 255, .b = 0, .a = 255 }
		},
		[2] = {
			.pos = { .x = 200, .y = 0 },
			.color = { .r = 0, .g = 0, .b = 255, .a = 255 }
		},
	};

	graphics_draw(state->device, tr, 3);

	graphics_flush(state->device);
}

void draw_background(game_state_t *state)
{
	graphics_draw_image(state->device, &(vector2_t){0, 0},
			    background_bin, NULL);
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
