#ifndef GAME
#define GAME

#include <stdlib.h>
#include "math.h"
#include "gamestate.h"
#include "typedefs.h"
#include "sprite.h"
#include "box.h"
#include "draw.h"

void game_init(game_state_t *state);
void game_update(game_state_t *state, float delta);
void game_draw(game_state_t *state);

#endif
