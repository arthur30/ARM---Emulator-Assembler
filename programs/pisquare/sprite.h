#ifndef SPRITE
#define SPRITE

#include "typedefs.h"
#include "entity.h"

typedef struct {
	entity_t *entity;
	unsigned char *image;
} sprite_t;

#endif
