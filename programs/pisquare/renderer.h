#ifndef RENDERER_H
#define RENDERER_H

#include <stdlib.h>
#include <stdint.h>

#include "vector2.h"
#include "color.h"

typedef struct graphics {
	uint32_t width;
	uint32_t height;
	uint32_t pitch;
	uint32_t bpp;
	uint8_t *mem;
	uint8_t *fb;
} graphics_t;

typedef struct cvertex {
	vector2_t pos;
	color_t color;
} cvertex_t;

graphics_t *graphics_create(size_t width, size_t height, size_t depth);
void graphics_blank(const graphics_t *device);
void graphics_draw(const graphics_t *device,
		   const cvertex_t *vertices, size_t len);
void graphics_draw_rectangle(const graphics_t *device, const color_t *color,
			     const vector2_t *pos, const vector2_t *hw);
void graphics_draw_image(const graphics_t *device, const vector2_t *pos,
			 const unsigned char *image, const color_t *tint);
void graphics_flush(const graphics_t *device);

#endif
