#ifndef GAMESTATE
#define GAMESTATE

#include "typedefs.h"
#include "player.h"
#include "box.h"

typedef struct {
	screen_t screen;
	vector_t area;
	void *buffer;

	long double delta;

	player_t *player;

	box_t **boxes;

	int box_count;

	/* Timers */
	long double timer_box;
	long double timer_game;
} game_state_t;

#endif
