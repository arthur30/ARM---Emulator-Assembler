#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "color.h"

#define PLAYER_LIVES 3
#define PLAYER_DIRECTION 90
#define PLAYER_SPEED 200

typedef struct {
	entity_t *entity;
	float dir;
	float speed;
	float angular_vel;
	int lives;
	color_t color;
} player_t;

player_t *player_new(void);
void player_free(player_t *player);

#endif
