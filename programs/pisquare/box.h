#ifndef BOX_H
#define BOX_H

#include <stdbool.h>

#include "entity.h"
#include "color.h"

typedef struct {
	entity_t *entity;
	color_t color;
} box_t;

box_t *box_new(void);
void box_free(box_t *);

#endif
