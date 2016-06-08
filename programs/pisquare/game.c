#include "game.h"
#include "libarmc/rpi-random.h"

#define BOX_MAX 5

static void initialize_screen(game_state_t *state, colour_t colour)
{
	RPI_InitRandom();
	draw_rect(state,
		(rect_t){(vector_t){0, 0},
		       (vector_t){state->screen.width, state->screen.height} },
								colour);
}

void game_init(game_state_t *state)
{
	int velx;
	int vely;

	/* screen color */
	initialize_screen(state, (colour_t){255, 255, 255, 255});

	/* boxes */
	box_t **boxes = calloc(BOX_MAX, sizeof(box_t *));

	for (int i = 0; i < BOX_MAX; i++) {
		boxes[i] = box_new();

		boxes[i]->entity->size = (vector_t){10, 10};
		boxes[i]->entity->pos = (vector_t){i * 10, i * 10};
		velx = RPI_GetRandom() >> 30;
		vely = RPI_GetRandom() >> 30;

		boxes[i]->entity->vel = (vector_t){velx, vely};

		boxes[i]->colour = (colour_t){i*50, i*30, i*20, 255};
		draw_box(state, boxes[i]);
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
	(void)state;
	/* boxes */
	for (int i = 0; i < BOX_MAX; i++) {
		box_t *box = state->boxes[i];

		entity_t *entity = box->entity;

		if ((entity->size.x + entity->pos.x) >= state->screen.width) {
			entity->vel.x = -abs(entity->vel.x);
			/* entity->vel.y = -abs(entity->vel.y); */
		} else if (entity->pos.x <= 0) {
			entity->vel.x = abs(entity->vel.x);
			/* entity->vel.y = abs(entity->vel.y); */
		}

		if ((entity->size.y + entity->pos.y) >= state->screen.height) {
			/* entity->vel.x = -abs(entity->vel.x); */
			entity->vel.y = -abs(entity->vel.y);
		} else if (entity->pos.y <= 0) {
			/* entity->vel.x = abs(entity->vel.x); */
			entity->vel.y = abs(entity->vel.y);
		}

		wipe_box(state, box);
		move_box(state, box);
		draw_box(state, box);
	}
}

void game_draw(game_state_t *state)
{
	(void)state;
}
