#ifndef MOVE_H
#define MOVE_H

#include "gamestate.h"
#include "entity.h"
#include "box.h"
#include "sprite.h"
#include "player.h"

void move_entity(game_state_t *state, entity_t *ent);
void move_box(game_state_t *state, box_t *box);
void move_player(game_state_t *state, player_t *player);
void move_sprite(game_state_t *state, sprite_t *player);

#endif
