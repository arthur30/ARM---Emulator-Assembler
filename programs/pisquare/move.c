#include "move.h"

void move_entity(game_state_t *state, entity_t *ent)
{
	long double delta = state->delta;
	ent->vel.x += delta * ent->acc.x;
	ent->vel.y += delta * ent->acc.y;
	ent->pos.x += delta * ent->vel.x;
	ent->pos.y += delta * ent->vel.y;
}


void move_box(game_state_t *state, box_t *box)
{
	move_entity(state, box->entity);
}

void move_player(game_state_t *state, player_t *player)
{
	player->entity->vel.x = player->speed * cos((double)player->dir/M_PI);
	player->entity->vel.y = player->speed * sin((double)player->dir/M_PI);

	move_entity(state, player->entity);
}

void move_sprite(game_state_t *state, sprite_t *sprt)
{
	move_entity(state, sprt->entity);
}
