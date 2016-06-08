#include "game.h"

static void initialize_screen(game_state_t *state, colour_t colour)
{
	draw_rect(state,
		(rect_t){(vector_t){0, 0},
		       (vector_t){state->screen.width, state->screen.height} },
								colour);
}

void game_init(game_state_t *state)
{
	/* screen color */
	initialize_screen(state, (colour_t){200, 50, 200, 0});

	/* boxes */
	box_t *boxes = calloc(20, sizeof(box_t));

	for (int i = 0; i < 20; i++) {
		boxes[i] = box_new();
		boxes[i].entity->pos = (vector_t){24 * i, 24 * i};
		boxes[i].entity->vel = (vector_t){10-i, i-10};
		boxes[i].colour = (colour_t){12*i, 4*i, 8*i, 255};
	}
	state->boxes = boxes;
}
/*void game_prerender(game_state_t *state)
*{
*	(void)state;
*}
*/

void game_update(game_state_t *state, float delta)
{
	(void)delta;
	/* boxes */
	for (int i = 0; i < 20; i++) {
		box_t box = state->boxes[i];

		entity_t *entity = box.entity;

		if ((entity->size.x + entity->pos.x) >= state->screen.width) {
			entity->vel.x = -abs(entity->vel.x);
			entity->vel.y = -abs(entity->vel.y);
		} else if (entity->pos.x <= 0) {
			entity->vel.x = abs(entity->vel.x);
			entity->vel.y = abs(entity->vel.y);
		}
		move_box(state, &box);
		draw_box(state, &box);
	}
}

void game_draw(game_state_t *state)
{
	(void)state;
}
