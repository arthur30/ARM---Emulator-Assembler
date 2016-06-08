#ifndef BOX
#define BOX

#include "entity.h"

typedef struct {
	entity_t *entity;
	colour_t colour;
} box_t;

box_t box_new(void);

#endif
