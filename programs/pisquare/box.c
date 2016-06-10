#include "box.h"

#include <stdlib.h>

box_t *box_new(void)
{
	box_t *box = malloc(sizeof(box_t));

	box->color = (color_t){100, 150, 135, 255};
	box->entity = entity_new();

	return box;
}

void box_free(box_t *box)
{
	entity_free(box->entity);
	free(box);
}
