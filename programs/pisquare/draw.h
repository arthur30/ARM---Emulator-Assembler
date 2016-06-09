#ifndef DRAW_H
#define DRAW_H

#include "gamestate.h"
#include "player.h"
#include "box.h"
#include "sprite.h"
#include "minmax.h"

/* Generic Drawing Functions */
void draw_rect(game_state_t *state, rect_t rect, color_t col);
void draw_sprite(game_state_t *state, sprite_t *sprite);

/* Box */
void draw_box(game_state_t *state, box_t *box);

/* Players */
void draw_player(game_state_t *state, player_t *player);

#endif
