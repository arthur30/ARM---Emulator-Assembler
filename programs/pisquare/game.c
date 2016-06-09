#include "game.h"

static void initialize_screen(game_state_t *state, colour_t colour)
{
	draw_rect(state,
		(rect_t){(vector_t){0, 0},
		       (vector_t){state->screen.width, state->screen.height} },
								colour);
}

static void add_box(game_state_t *state)
{
	bool right = random_int(2);
	int sign = 2 * right - 1;
	float vel = sign * (random_int(700) + 100);

	box_t *box = box_new();

	box->entity->size = (vector_t){10, 10};
	int randy = random_int(state->area.y - box->entity->size.x)
						+ box->entity->size.x/2;

	int posx = -box->entity->size.x + !right * (state->area.x
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

static void print_time(game_state_t *state)
{
	char str[10];
	int len = sprintf(str, "%.1f", (double)state->timer_game);

	int x = state->screen.width - len * 20;

	print_text(state, str, (vector_t){x, 486});
}

static void print_lives(game_state_t *state)
{
	char str[10];

	sprintf(str, "LIVES %d", state->player->lives);
	print_text_colour(state, str, (vector_t){0, 486},
						(colour_t){255, 0, 0, 255});
}

void game_init(game_state_t *state)
{
	RPI_InitRandom();
	/* game area */
	state->area = (vector_t){state->screen.width,
				     state->screen.height - 32};

	/* screen color */
	initialize_screen(state, (colour_t){0, 0, 0, 255});

	/* boxes */
	state->box_count = 0;

	/* player */
	player_t *player = player_new();

	player->entity->pos = (vector_float_t){256, 256};
	player->angular_vel = 180;

	state->player = player;

	/* timers */
	state->timer_box = 0;
	state->timer_game = 0;
}

void game_update(game_state_t *state)
{
	/* Timers */
	state->timer_box += state->delta;
	state->timer_game += state->delta;

	/* Boxes */
	/* Increase number of boxes over time */
	if (state->timer_box > BOX_TIMER && state->box_count < BOX_COUNT_MAX) {
		state->timer_box = 0;
		add_box(state);
	}
	/* Move Boxes */
	for (int i = 0; i < state->box_count; i++) {
		box_t *box = state->boxes[i];

		move_box(state, box);
	}
	/* Player */
	move_player(state, state->player);

	/* Detect collisions */
	for (int i = 0; i < state->box_count; i++) {
		box_t *box = state->boxes[i];
		(void)box;
	}

}

void game_draw(game_state_t *state)
{
	/* boxes */
	for (int i = 0; i < state->box_count; i++) {
		box_t *box = state->boxes[i];

		draw_box(state, box);
	}
	/* player */
	draw_player(state, state->player);

	/* UI */
	print_time(state);
	print_lives(state);
}

void game_free(game_state_t *state)
{
	/* boxes */
	for (int i = 0; i < state->box_count; i++)
		box_free(state->boxes[i]);

	free(state->boxes);
	free(state->player);
}
