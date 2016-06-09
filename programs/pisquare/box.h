#ifndef BOX
#define BOX

#include "entity.h"
#include <stdlib.h>

typedef struct {
	entity_t *entity;
	colour_t colour;
} box_t;

box_t *box_new(void);
void box_free(box_t *);

#endif
