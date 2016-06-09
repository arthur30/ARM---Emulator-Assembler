#ifndef PLAYER
#define PLAYER

#include "typedefs.h"
#include "entity.h"

#define PLAYER_LIVES 3
#define PLAYER_DIRECTION 90
#define PLAYER_SPEED 40

typedef struct {
	entity_t *entity;
	int dir;
	float speed;
	int lives;
	colour_t colour;
} player_t;

player_t *player_new(void);
void player_free(player_t *player);

#endif
