#include "game.h"


#define BOX_MAX 20

static void initialize_screen(game_state_t *state, colour_t colour)
{
	draw_rect(state,
		(rect_t){(vector_t){0, 0},
		       (vector_t){state->screen.width, state->screen.height} },
								colour);
}

void game_init(game_state_t *state)
{
	int vel;
	bool dir;

	RPI_InitRandom();

	/* screen color */
	initialize_screen(state, (colour_t){255, 255, 255, 255});

	/* boxes */
	box_t **boxes = calloc(BOX_MAX, sizeof(box_t *));

	for (int i = 0; i < BOX_MAX; i++) {
		boxes[i] = box_new();

		boxes[i]->entity->size = (vector_t){10, 10};
		boxes[i]->entity->pos = (vector_float_t){i * 10, 0};

		dir = random_int(2);
		vel = 50+random_int(80);

		boxes[i]->entity->vel = (vector_float_t){dir*vel, !dir*vel};

		boxes[i]->colour = (colour_t){i*5, i*7, i*8, 255};
		draw_box(state, boxes[i]);
	}
	state->boxes = boxes;
}
/*void game_prerender(game_state_t *state)
*{
*	(void)state;
*}
*/

void game_update(game_state_t *state)
{
	/* boxes */
	for (int i = 0; i < BOX_MAX; i++) {
		box_t *box = state->boxes[i];

		entity_t *entity = box->entity;

		move_box(state, box);

		if (entity->pos.x > state->screen.width) {
			entity->pos.x = -entity->size.x;
			entity->pos.y = random_int(state->screen.height
							-entity->size.y)
							+ entity->size.y/2;
		} else if (entity->pos.x + entity->size.x  < 0) {
			entity->pos.x = state->screen.width;
			entity->pos.y = random_int(state->screen.height
							-entity->size.y)
							+ entity->size.y/2;
		}
		if (entity->pos.y > state->screen.height) {
			entity->pos.y = -entity->size.y;
			entity->pos.x = random_int(state->screen.width
							-entity->size.x)
							+ entity->size.x/2;
		} else if (entity->pos.y + entity->size.y < 0) {
			entity->pos.y = state->screen.height;
			entity->pos.x = random_int(state->screen.width
							-entity->size.x)
							+ entity->size.x/2;
		}
	}
}

void game_draw(game_state_t *state)
{
	/* boxes */
	for (int i = 0; i < BOX_MAX; i++) {
		box_t *box = state->boxes[i];

		draw_box(state, box);
	}
}

void game_free(game_state_t *state)
{
	/* boxes */
	for (int i = 0; i < BOX_MAX; i++)
		box_free(state->boxes[i]);

	free(state->boxes);
}
