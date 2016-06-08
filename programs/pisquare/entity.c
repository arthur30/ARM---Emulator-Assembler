#include "entity.h"

entity_t *entity_new(void)
{
	return &(entity_t){{0, 0}, {10, 10}, {0, 0}, {0, 0} };
}

rect_t entity_rect(entity_t *ent)
{
	return (rect_t){ent->pos, ent->size};
}
