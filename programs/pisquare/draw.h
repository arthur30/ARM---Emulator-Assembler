#ifndef DRAW
#define DRAW

#include "gamestate.h"
#include "player.h"
#include "box.h"
#include "sprite.h"
#include "minmax.h"

/* Generic Drawing Functions */
void draw_rect(game_state_t*, rect_t, colour_t);
void draw_sprite(game_state_t*, sprite_t*);

/* Box */
void draw_box(game_state_t*, box_t *box);

/* Players */
void draw_player(game_state_t *, player_t *player);
#endif
