#ifndef MOVE
#define MOVE

#include "gamestate.h"
#include "entity.h"
#include "box.h"
#include "sprite.h"

void move_entity(game_state_t*, entity_t*);
void move_box(game_state_t*, box_t*);
void move_sprite(game_state_t*, sprite_t*);

#endif
