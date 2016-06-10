#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include "vector2.h"

typedef struct {
	vector2_t origin;
	vector2_t size;
} rect_t;

typedef struct {
	int width;
	int height;
	unsigned char *image;
} image_t;

#endif
