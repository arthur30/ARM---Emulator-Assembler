#include "box.h"

box_t *box_new(void)
{
	box_t *box = malloc(sizeof(box_t));

	box->colour = (colour_t){100, 150, 135, 255};
	box->entity = entity_new();

	return box;
}

void box_free(box_t *box)
{
	entity_free(box->entity);
	free(box);
}
