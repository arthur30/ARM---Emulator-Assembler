#ifndef GAMESTATE
#define GAMESTATE

#include "typedefs.h"
#include "box.h"

#define BOX_TIMER 0.5f
#define BOX_COUNT_MAX 40

typedef struct {
	screen_t screen;
	void *buffer;
	box_t **boxes;

	long double delta;

	unsigned short box_count;

	/* Timers */
	float timer_box;
} game_state_t;

#endif
