#include "move.h"

static double mod(double x, double y)
{
	if (x > 0)
		return x > y ? x - y : x;
	else
		return x < y ? x + 2 * y : x + y;
}

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
	entity_t *entity = box->entity;

	if (entity->pos.x > state->area.x) {
		entity->pos.x = -entity->size.x;
		entity->pos.y = random_int(state->area.y
						-entity->size.y)
						+ entity->size.y/2;
	} else if (entity->pos.x + entity->size.x  < 0) {
		entity->pos.x = state->area.x;
		entity->pos.y = random_int(state->area.y
						-entity->size.y)
						+ entity->size.y/2;
	}
	if (entity->pos.y > state->area.y) {
		entity->pos.y = -entity->size.y;
		entity->pos.x = random_int(state->area.x
						-entity->size.x)
						+ entity->size.x/2;
	} else if (entity->pos.y + entity->size.y < 0) {
		entity->pos.y = state->area.y;
		entity->pos.x = random_int(state->area.x
						-entity->size.x)
						+ entity->size.x/2;
	}
	move_entity(state, entity);
}

void move_player(game_state_t *state, player_t *player)
{
	player->dir = mod(player->dir + player->angular_vel * state->delta,
									360);
	player->entity->vel.x = player->speed * cos(M_PI * player->dir/180.f);
	player->entity->vel.y = player->speed * sin(M_PI * player->dir/180.f);

	move_entity(state, player->entity);
}

void move_sprite(game_state_t *state, sprite_t *sprt)
{
	move_entity(state, sprt->entity);
}
