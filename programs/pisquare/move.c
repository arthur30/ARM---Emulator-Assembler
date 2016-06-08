#include "move.h"

void move_entity(game_state_t *state, entity_t *ent)
{
	(void)state;
	ent->vel.x += ent->acc.x;
	ent->vel.y += ent->acc.y;
	ent->pos.x += ent->vel.x;
	ent->pos.y += ent->vel.y;
}


void move_box(game_state_t *state, box_t *box)
{
	(void)state;
	move_entity(state, box->entity);
}

void move_sprite(game_state_t *state, sprite_t *sprt)
{
	(void)state;
	move_entity(state, sprt->entity);
}
