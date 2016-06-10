#include "renderer.h"

#include <stdlib.h>
#include <string.h>

#include "rpi-mailbox-interface.h"

#include "vector2.h"
#include "color.h"
#include "dma.h"

static size_t get_buf_size(const graphics_t *device)
{
	return device->width * device->height * (device->bpp >> 3);
}

graphics_t *graphics_create(size_t width, size_t height, size_t depth)
{
	rpi_mailbox_property_t *mp;
	graphics_t *device;

	device = malloc(sizeof(graphics_t));
	if (!device)
		goto fail;

	/* Initialise a framebuffer... */
	RPI_PropertyInit();
	RPI_PropertyAddTag(TAG_ALLOCATE_BUFFER);
	RPI_PropertyAddTag(TAG_SET_PHYSICAL_SIZE, width, height);
	RPI_PropertyAddTag(TAG_SET_VIRTUAL_SIZE, width, height);
	RPI_PropertyAddTag(TAG_SET_DEPTH, depth);
	RPI_PropertyAddTag(TAG_GET_PITCH);
	RPI_PropertyAddTag(TAG_GET_PHYSICAL_SIZE);
	RPI_PropertyAddTag(TAG_GET_DEPTH);
	RPI_PropertyProcess();

	mp = RPI_PropertyGet(TAG_GET_PHYSICAL_SIZE);
	if (mp) {
		device->width = mp->data.buffer_32[0];
		device->height = mp->data.buffer_32[1];
	}

	mp = RPI_PropertyGet(TAG_GET_DEPTH);
	if (mp)
		device->bpp = mp->data.buffer_32[0];

	mp = RPI_PropertyGet(TAG_GET_PITCH);
	if (mp)
		device->pitch = mp->data.buffer_32[0];

	mp = RPI_PropertyGet(TAG_ALLOCATE_BUFFER);
	if (mp)
		device->fb = (uint8_t *)(uintptr_t)mp->data.buffer_32[0];

	device->mem = malloc(get_buf_size(device));

	if (!device->mem)
		goto fail1;

	return device;

fail1:
	free(device);
fail:
	return NULL;
}

void graphics_blank(const graphics_t *device)
{
	dma_zero(device->mem, get_buf_size(device));
}

static color_t overrun;
static color_t *get_pixel(const graphics_t *device, size_t x, size_t y)
{
	size_t bpos;

	if (x >= device->width || y >= device->height)
		return &overrun;

	bpos = y * device->pitch + x * (device->bpp >> 3);

	return (void *)(&device->mem[bpos]);
}

static void draw_line(const graphics_t *device,
		      const cvertex_t *l1, const cvertex_t *l2)
{
	cvertex_t c;
	color_t *pixel;
	float pr;

	c.pos = l1->pos;

	for (; c.pos.x <= l2->pos.x; c.pos.x++) {
		pixel = get_pixel(device, (int)c.pos.x, (int)c.pos.y);
		pr = vector2_between(&c.pos, &l1->pos, &l2->pos);
		color_interpolate(&c.color, &l1->color, &l2->color, pr);

		pixel->r = c.color.r;
		pixel->g = c.color.g;
		pixel->b = c.color.b;
		if (device->bpp == 32)
			pixel->a = c.color.a;
	}

}

static void fill_bottom_flat_triangle(const graphics_t *device,
				      const cvertex_t *v1,
				      const cvertex_t *v2,
				      const cvertex_t *v3)
{
	cvertex_t l1;
	cvertex_t l2;

	float invslope1, invslope2;
	float curx1, curx2;
	float pr;
	int scanlineY;

	invslope1 = (v2->pos.x - v1->pos.x) / (v2->pos.y - v1->pos.y);
	invslope2 = (v3->pos.x - v1->pos.x) / (v3->pos.y - v1->pos.y);

	curx1 = v1->pos.x;
	curx2 = v1->pos.x;

	for (scanlineY = v1->pos.y; scanlineY <= v2->pos.y; scanlineY++) {
		l1.pos.y = l2.pos.y = scanlineY;
		l1.pos.x = curx1;
		l2.pos.x = curx2;
		pr = vector2_between(&l1.pos, &v1->pos, &v2->pos);
		color_interpolate(&l1.color, &v1->color, &v2->color, pr);
		pr = vector2_between(&l2.pos, &v1->pos, &v3->pos);
		color_interpolate(&l2.color, &v1->color, &v3->color, pr);

		draw_line(device, &l1, &l2);

		curx1 += invslope1;
		curx2 += invslope2;
	}
}

static void fill_top_flat_triangle(const graphics_t *device,
				   const cvertex_t *v1,
				   const cvertex_t *v2,
				   const cvertex_t *v3)
{
	cvertex_t l1;
	cvertex_t l2;

	float invslope1, invslope2;
	float curx1, curx2;
	float pr;
	int scanlineY;

	invslope1 = (v3->pos.x - v1->pos.x) / (v3->pos.y - v1->pos.y);
	invslope2 = (v3->pos.x - v2->pos.x) / (v3->pos.y - v2->pos.y);

	curx1 = v3->pos.x;
	curx2 = v3->pos.x;

	for (scanlineY = v3->pos.y; scanlineY > v1->pos.y; scanlineY--) {
		l1.pos.y = l2.pos.y = scanlineY;
		l1.pos.x = curx1;
		l2.pos.x = curx2;
		pr = vector2_between(&l1.pos, &v1->pos, &v3->pos);
		color_interpolate(&l1.color, &v1->color, &v3->color, pr);
		pr = vector2_between(&l2.pos, &v2->pos, &v3->pos);
		color_interpolate(&l2.color, &v2->color, &v3->color, pr);

		draw_line(device, &l1, &l2);

		curx1 -= invslope1;
		curx2 -= invslope2;
	}
}

static int vert_cmp(const void *va, const void *vb)
{
	int ret;
	const cvertex_t * const *a;
	const cvertex_t * const *b;

	a = va;
	b = vb;

	ret = (*a)->pos.y - (*b)->pos.y;
	if (!ret)
		ret = (*a)->pos.x - (*b)->pos.x;

	return ret;
}

static void vert_sort(const cvertex_t **v1,
		      const cvertex_t **v2,
		      const cvertex_t **v3)
{
	const cvertex_t *vert[3];

	vert[0] = *v1;
	vert[1] = *v2;
	vert[2] = *v3;

	qsort(vert, 3, sizeof(cvertex_t *), vert_cmp);

	*v1 = vert[0];
	*v2 = vert[1];
	*v3 = vert[2];
}

static void rasterize(const graphics_t *device,
		      const cvertex_t *v1,
		      const cvertex_t *v2,
		      const cvertex_t *v3)
{
	vert_sort(&v1, &v2, &v3);

	cvertex_t v4;
	float pr;

	if ((int)v2->pos.y == (int)v3->pos.y) {
		fill_bottom_flat_triangle(device, v1, v2, v3);
	} else if ((int)v1->pos.y == (int)v2->pos.y) {
		fill_top_flat_triangle(device, v1, v2, v3);
	} else {
		v4.pos.x = (v1->pos.x + ((v2->pos.y - v1->pos.y) /
			   (v3->pos.y - v1->pos.y)) * (v3->pos.x - v1->pos.x));
		v4.pos.y = v2->pos.y;

		pr = vector2_between(&v4.pos, &v1->pos, &v3->pos);
		color_interpolate(&v4.color, &v1->color, &v3->color, pr);

		rasterize(device, v1, v2, &v4);
		rasterize(device, v2, &v4, v3);
	}
}

void graphics_draw(const graphics_t *device,
		   const cvertex_t *vertices, size_t len)
{
	size_t i;

	for (i = 0; i < len; i += 3) {
		rasterize(device, &vertices[i],
				  &vertices[i + 1],
				  &vertices[i + 2]);
	}
}

static cvertex_t rect[6];
void graphics_draw_rectangle(const graphics_t *device, const color_t *color,
			     const vector2_t *pos, const vector2_t *hw)
{
	rect[0].pos.x = pos->x;
	rect[0].pos.y = pos->y;
	rect[0].color = *color;

	rect[1].pos.x = pos->x;
	rect[1].pos.y = pos->y + hw->y;
	rect[1].color = *color;

	rect[2].pos.x = pos->x + hw->x;
	rect[2].pos.y = pos->y;
	rect[2].color = *color;

	rect[3].pos.x = pos->x + hw->x;
	rect[3].pos.y = pos->y;
	rect[3].color = *color;

	rect[4].pos.x = pos->x;
	rect[4].pos.y = pos->y + hw->y;
	rect[4].color = *color;

	rect[5].pos.x = pos->x + hw->x;
	rect[5].pos.y = pos->y + hw->y;
	rect[5].color = *color;

	graphics_draw(device, rect, 6);
}

static void graphics_draw_image_fast(const graphics_t *device,
				     const unsigned char *image, size_t imglen)
{
	size_t copysize;

	copysize = get_buf_size(device);
	if (imglen < copysize)
		copysize = imglen;

	dma_copy(device->mem, image + 8, copysize);
}

void graphics_draw_image(const graphics_t *device, const vector2_t *pos,
			 const unsigned char *image)
{
	size_t width, height;
	size_t x, y;
	size_t imgpos;
	color_t *p;

	memcpy(&height, image, 4);
	memcpy(&width, image + 4, 4);

	if (width == device->width) {
		graphics_draw_image_fast(device, image, height * width * 4);
		return;
	}

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			p = get_pixel(device, pos->x + x, pos->y + y);
			imgpos = 8 + (y * width * 4) + (x * 4);
			p->b = image[imgpos++];
			p->g = image[imgpos++];
			p->r = image[imgpos++];
			if (device->bpp == 32)
				p->a = image[imgpos++];
		}
	}
}

void graphics_flush(const graphics_t *device)
{
	dma_copy(device->fb, device->mem, get_buf_size(device));
}
