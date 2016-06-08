#include "box.h"

box_t box_new(void)
{
	entity_t *ent = entity_new();

	return (box_t){ent, (colour_t){100, 150, 135, 255} };
}

