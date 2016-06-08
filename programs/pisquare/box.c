#include "box.h"

box_t *box_new(void)
{
	box_t *box = malloc(sizeof(box_t));

	box->colour = (colour_t){100, 150, 135, 255};

	entity_t *ent = entity_new();

	box->entity = ent;

	return box;
}

