#include "player.h"

player_t *player_new(void)
{
	player_t *new = malloc(sizeof(player_t));

	new->entity = entity_new();
	new->entity->size = (vector_t){30, 30};
	new->lives = PLAYER_LIVES;
	new->dir = PLAYER_DIRECTION;
	new->speed = PLAYER_SPEED;
	new->colour = (colour_t){0, 0, 255, 255};

	return new;
}

void player_free(player_t *player)
{
	entity_free(player->entity);
	free(player);
}
