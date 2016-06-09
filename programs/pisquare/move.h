#ifndef MOVE
#define MOVE

#include <math.h>
#include "gamestate.h"
#include "entity.h"
#include "box.h"
#include "sprite.h"
#include "player.h"

void move_entity(game_state_t*, entity_t*);
void move_box(game_state_t*, box_t*);
void move_player(game_state_t*, player_t*);
void move_sprite(game_state_t*, sprite_t*);

#endif
