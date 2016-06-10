#include "entity.h"
#include <stdlib.h>
#include <math.h>

entity_t *entity_new(void)
{
	entity_t *ent = malloc(sizeof(entity_t));

	ent->pos = (vector2_t){0, 0};
	ent->size = (vector2_t){10, 10};
	ent->vel = (vector2_t){0, 0};
	ent->acc = (vector2_t){0, 0};

	return ent;
}
void entity_free(entity_t *ent)
{
	free(ent);
}

rect_t *entity_rect(rect_t *rect, entity_t *ent)
{
	rect->origin = ent->pos;
	rect->size = ent->size;
	return rect;
}

bool entity_test_collision(entity_t *e1, entity_t *e2)
{
	return (fabsf(e1->pos.x - e2->pos.x) * 2 <
					(e1->size.x + e2->size.x));
}
