#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "renderer.h"
#include "player.h"
#include "box.h"

typedef struct {
	graphics_t *device;

	vector2_t area;

	long double delta;

	player_t *player;

	box_t **boxes;

	int box_count;

	/* Timers */
	long double timer_box;
	long double timer_game;
} game_state_t;

#endif
