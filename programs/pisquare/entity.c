#include "entity.h"

entity_t *entity_new(void)
{
	entity_t *ent = malloc(sizeof(entity_t));

	ent->pos = (vector_t){0, 0};
	ent->size = (vector_t){10, 10};
	ent->vel = (vector_t){0, 0};
	ent->acc = (vector_t){0, 0};

	return ent;
}

rect_t entity_rect(entity_t *ent)
{
	return (rect_t){ent->pos, ent->size};
}
