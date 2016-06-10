#include "vector2.h"

#include <math.h>

static int floateq(float a, float b)
{
	return fabsf(a - b) < 0.00001f;
}

float vector2_abs(const vector2_t *v)
{
	return sqrtf(v->x * v->x + v->y * v->y);
}

void vector2_sub(vector2_t *dst, const vector2_t *a, const vector2_t *b)
{
	dst->x = a->x - b->x;
	dst->y = a->y - b->y;
}

float vector2_dist(const vector2_t *a, const vector2_t *b)
{
	vector2_t tmp;

	vector2_sub(&tmp, a, b);
	return vector2_abs(&tmp);
}

float vector2_between(const vector2_t *p,
		      const vector2_t *a, const vector2_t *b)
{
	if (floateq(a->x, b->x) && floateq(a->y, b->y))
		return 0;
	return vector2_dist(a, p) / vector2_dist(a, b);
}
