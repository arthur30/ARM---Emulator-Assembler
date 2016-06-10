#ifndef SPRITE_H
#define SPRITE_H

#include "entity.h"

typedef struct {
	entity_t *entity;
	unsigned char *image;
} sprite_t;

#endif
