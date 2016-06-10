#include "box.h"

#include <stdlib.h>

box_t *box_new(void)
{
	box_t *box = malloc(sizeof(box_t));

	box->color = (color_t){.r = 100, .g = 150, .b = 135, .a = 255};
	box->entity = entity_new();

	return box;
}

void box_free(box_t *box)
{
	entity_free(box->entity);
	free(box);
}
