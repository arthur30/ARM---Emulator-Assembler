#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>
#include "typedefs.h"
#include "vector2.h"

typedef struct {
	vector2_t pos;
	vector2_t size;
	vector2_t vel;
} entity_t;

entity_t *entity_new(void);
void entity_free(entity_t *ent);
rect_t *entity_rect(rect_t *rect, entity_t *ent);
bool entity_test_collision(entity_t *a, entity_t *b);

#endif
