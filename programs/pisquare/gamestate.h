#ifndef GAMESTATE
#define GAMESTATE

#include "typedefs.h"
#include "box.h"

typedef struct {
	screen_t screen;
	void *buffer;
	box_t **boxes;

	long double delta;
} game_state_t;

#endif
