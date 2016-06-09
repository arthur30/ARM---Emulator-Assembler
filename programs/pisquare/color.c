#include "color.h"

void color_interpolate(color_t *dst, const color_t *a, const color_t *b,
		       float p)
{
	dst->r = a->r + (uint8_t)((b->r - a->r) * p);
	dst->b = a->b + (uint8_t)((b->b - a->b) * p);
	dst->g = a->g + (uint8_t)((b->g - a->g) * p);
	dst->a = a->a + (uint8_t)((b->a - a->a) * p);
}
