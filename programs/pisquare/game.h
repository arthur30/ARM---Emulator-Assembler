#ifndef GAME
#define GAME

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "math.h"
#include "gamestate.h"
#include "typedefs.h"
#include "sprite.h"
#include "box.h"
#include "draw.h"
#include "move.h"
#include "random.h"
#include "minmax.h"
#include "text.h"

#define BOX_TIMER 1.0f
#define BOX_COUNT_MAX 5

void game_init(game_state_t *state);
void game_update(game_state_t *state);
void game_draw(game_state_t *state);
void game_free(game_state_t *state);

#endif
