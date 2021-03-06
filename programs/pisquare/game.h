#ifndef GAME_H
#define GAME_H

#include "gamestate.h"

#define BOX_TIMER 1.0f
#define BOX_COUNT_MAX 16

void game_init(game_state_t *state);
void game_splash(game_state_t *state);
void game_update(game_state_t *state);
void game_draw(game_state_t *state);
void game_free(game_state_t *state);

#endif
