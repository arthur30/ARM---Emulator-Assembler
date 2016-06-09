#include "entity.h"

entity_t *entity_new(void)
{
	entity_t *ent = malloc(sizeof(entity_t));

	ent->pos = (vector_float_t){0, 0};
	ent->size = (vector_t){10, 10};
	ent->vel = (vector_float_t){0, 0};
	ent->acc = (vector_float_t){0, 0};

	return ent;
}
void entity_free(entity_t *ent)
{
	free(ent);
}

rect_t entity_rect(entity_t *ent)
{
	return (rect_t){{ent->pos.x, ent->pos.y}, ent->size};
}

bool entity_test_collision(entity_t *e1, entity_t *e2)
{
	return (abs(e1->pos.x - e2->pos.x) * 2 <
					(e1->size.x + e2->size.x));
}
