#include "game.h"

static void initialize_screen(game_state_t *state, colour_t colour)
{
	draw_rect(state,
		(rect_t){(vector_t){0, 0},
		       (vector_t){state->screen.width, state->screen.height} },
								colour);
}
#ifdef DEBUG
static void fill_random(game_state_t *state)
{
	int rand_r = random_int(256);
	int rand_g = random_int(256);
	int rand_b = random_int(256);

	initialize_screen(state, (colour_t){rand_r, rand_g, rand_b, 255});
}
#endif

static void add_box(game_state_t *state)
{
	bool right = random_int(2);
	int sign = 2 * right - 1;
	float vel = sign * random_int(500);

	box_t *box = box_new();

	box->entity->size = (vector_t){10, 10};
	int randy = random_int(state->screen.height - box->entity->size.x)
						+ box->entity->size.x/2;

	int posx = -box->entity->size.x + !right * (state->screen.width
							+ box->entity->size.x);
	int rand_r = random_int(256);
	int rand_g = random_int(256);
	int rand_b = random_int(256);

	box->entity->pos = (vector_float_t){posx, randy};
	box->entity->vel = (vector_float_t){vel, 0};
	box->colour = (colour_t){rand_r, rand_g, rand_b, 255};

	++state->box_count;
	state->boxes = realloc(state->boxes,
					state->box_count * sizeof(box_t *));
	state->boxes[state->box_count-1] = box;
}

void game_init(game_state_t *state)
{
	RPI_InitRandom();

	/* screen color */
	initialize_screen(state, (colour_t){104, 200, 169, 255});

	/* boxes */
	state->box_count = 0;
}

void game_update(game_state_t *state)
{
	/* Timers */
	state->timer_box += state->delta;

	/* Boxes */
	/* Increase number of boxes over time */
	if (state->timer_box > BOX_TIMER && state->box_count < BOX_COUNT_MAX) {
		state->timer_box = 0;
		add_box(state);
	}

	for (int i = 0; i < state->box_count; i++) {
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
	for (int i = 0; i < state->box_count; i++) {
		box_t *box = state->boxes[i];

		draw_box(state, box);
	}
}

void game_free(game_state_t *state)
{
	/* boxes */
	for (int i = 0; i < state->box_count; i++)
		box_free(state->boxes[i]);

	free(state->boxes);
}
