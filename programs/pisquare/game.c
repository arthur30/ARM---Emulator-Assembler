#include "game.h"

#include <stdio.h>
#include "libarmc/rpi-gpio.h"
#include "rpi-random.h"
#include "renderer.h"
#include "random.h"
#include "text.h"
#include "box.h"
#include "player.h"
#include "move.h"
#include "draw.h"

static void add_box(game_state_t *state)
{
	int right = random_int(2);
	int sign = 2 * right - 1;
	float vel = sign * (random_int(100) + 50);

	box_t *box = box_new();

	box->entity->size = (vector2_t){15, 15};
	int randy = random_int(state->area.y - box->entity->size.y)
						+ box->entity->size.y/2;
	/*int randx = random_int(state->area.x - box->entity->size.x)
	*						+ box->entity->size.x/2;
	*/
	int posx = -box->entity->size.x + !right * (state->area.x
							+ box->entity->size.x);
	(void)vel;

	int grey = random_int(128) + 127;

	box->entity->pos = (vector2_t){posx, randy};
	box->entity->vel = (vector2_t){vel, 0};
	box->color = (color_t){grey, grey, grey, 255};

	++state->boxes_count;
	state->boxes = realloc(state->boxes,
					state->boxes_count * sizeof(box_t));

	state->boxes[state->boxes_count-1] = *box;
}

static void print_time(game_state_t *state)
{
	char str[10];
	int len = sprintf(str, "%.1f", state->timer_game);

	int x = state->device->width - len * 20;

	print_text(state, str, (vector2_t){x, 486});
}

static void print_lives(game_state_t *state)
{
	char str[10];

	sprintf(str, "LIVES %d", state->player->lives);
	print_text_color(state, str, (vector2_t){0, 486},
			 (color_t){ .r = 255, .g = 0, .b = 0, .a = 255});
}

static void print_io(game_state_t *state)
{
	char str[2];

	str[0] = RPI_GetGpioValue(state->player->right_pin) > 0 ? '1' : '0';
	str[1] = RPI_GetGpioValue(state->player->left_pin) > 0 ? '1' : '0';
	str[2] = 0;

	print_text(state, str, (vector2_t){200, 486});
}

static void print_fps(game_state_t *state)
{
	char str[2];

	sprintf(str, "%d", state->fps);
	print_text_color(state, str, (vector2_t){250, 486},
						(color_t){0, 255, 0, 255});
}

void game_init(game_state_t *state)
{
	RPI_InitRandom();
	/* game area */
	state->area = (vector2_t){state->device->width,
				  state->device->height - 32};

	/* boxes */
	state->boxes_count = 0;

	/* player */
	player_t *player = player_new();

	player->entity->pos = (vector2_t){256, 256};
	player->angular_vel = 0;
	player->color = (color_t){0, 255, 0, 255};
	player->right_pin = RPI_GPIO2;
	player->left_pin = RPI_GPIO3;

	RPI_SetGpioInput(player->right_pin);
	RPI_SetGpioInput(player->left_pin);

	RPI_GetGpio()->GPPUD = 2;

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
	state->timer_frame += state->delta;

	/* Diagnostics */
	if (state->timer_frame >= 1.f) {
		state->fps = state->frames_count;
		state->frames_count = 0;
		state->timer_frame = 0;
	}

	/* IO */
	if (RPI_GetGpioValue(state->player->right_pin) == 0)
		state->player->angular_vel = 270;
	else if (RPI_GetGpioValue(state->player->left_pin) == 0)
		state->player->angular_vel = -270;
	else
		state->player->angular_vel = 0;

	/* Boxes */
	/* Increase number of boxes over time */
	if (state->timer_box > BOX_TIMER
				&& state->boxes_count < BOX_COUNT_MAX) {
		state->timer_box = 0;
		add_box(state);
	}
	/* Move Boxes */
	for (int i = 0; i < state->boxes_count; i++)
		move_box(state, &state->boxes[i]);

	/* Player */
	move_player(state, state->player);

	/* Detect collisions */
	for (int i = 0; i < state->boxes_count; i++) {
		box_t box = state->boxes[i];
		(void)box;
	}

}

void game_draw(game_state_t *state)
{
	/* background */
	draw_background(state);

	/* boxes */
	for (int i = 0; i < state->boxes_count; i++)
		draw_box(state, &state->boxes[i]);


	/* player */
	draw_player(state, state->player);

	/* UI */
	print_time(state);
	print_lives(state);
	print_io(state);
	print_fps(state);

	/* Diagnostics */
	state->frames_count += 1;
}

void game_free(game_state_t *state)
{
	/* boxes */
	for (int i = 0; i < state->boxes_count; i++)
		box_free(&state->boxes[i]);

	free(state->boxes);
	free(state->player);
}
