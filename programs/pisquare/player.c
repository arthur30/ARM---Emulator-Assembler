#include "player.h"

#include <stdlib.h>

player_t *player_new(void)
{
	player_t *new = malloc(sizeof(player_t));

	new->entity = entity_new();
	new->entity->size = (vector2_t){5, 5};
	new->lives = PLAYER_LIVES;
	new->dir = PLAYER_DIRECTION;
	new->speed = PLAYER_SPEED;
	new->color = (color_t){0, 0, 255, 255};

	return new;
}

void player_free(player_t *player)
{
	entity_free(player->entity);
	free(player);
}
