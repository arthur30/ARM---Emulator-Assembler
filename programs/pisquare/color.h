#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

typedef struct color {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
} color_t;

void color_interpolate(color_t *dst, const color_t *a, const color_t *b,
		       float p);

#endif
