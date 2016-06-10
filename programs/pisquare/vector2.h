#ifndef VECTOR2_H
#define VECTOR2_H

typedef struct vector2 {
	float x;
	float y;
} vector2_t;

float vector2_abs(const vector2_t *v);
void vector2_sub(vector2_t *dst, const vector2_t *a, const vector2_t *b);
float vector2_dist(const vector2_t *a, const vector2_t *b);
float vector2_between(const vector2_t *p,
		      const vector2_t *a, const vector2_t *b);

#endif
