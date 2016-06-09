#ifndef GAMESTATE
#define GAMESTATE

#include "typedefs.h"
#include "box.h"

#define BOX_TIMER 1.0f
#define BOX_COUNT_MAX 40

typedef struct {
	screen_t screen;
	void *buffer;
	box_t **boxes;

	long double delta;

	int box_count;

	/* Timers */
	long double timer_box;
} game_state_t;

#endif
