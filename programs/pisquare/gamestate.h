#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "renderer.h"
#include "player.h"
#include "box.h"

typedef struct {
	graphics_t *device;

	vector2_t area;

	float delta;

	player_t *player;

	box_t *boxes;

	int boxes_count;
	int frames_count;
	int fps;

	/* Timers */
	float timer_frame;
	float timer_box;
	float timer_game;
} game_state_t;

#endif
