#include "sprite.h"

void sprite_draw(sprite_t *sprt, screen_t *screen)
{
	int pixel_offset;
	int i = 0;
	int xf, yf;

	xf = sprt->pos.x + sprt->vel.x;
	yf = sprt->pos.y + sprt->vel.y;

	for (int y = 0; y < sprt->size.y; y++) {
		for (int x = 0; x < sprt->size.x; x++) {
			pixel_offset = ((x + sprt->pos.x) *
						(screen->bpp >> 3))
					+ ((y + sprt->pos.y) * screen->pitch);
			screen->fb[pixel_offset++] = 255;
			screen->fb[pixel_offset++] = 255;
			screen->fb[pixel_offset++] = 255;
		}
	}

	for (int y = 0; y < sprt->size.y; y++) {
		for (int x = 0; x < sprt->size.x; x++) {
			pixel_offset = ((xf + x) * (screen->bpp >> 3)) +
					((yf + y) * screen->pitch);
			screen->fb[pixel_offset++] = (int)sprt->image[i++];
			screen->fb[pixel_offset++] = (int)sprt->image[i++];
			screen->fb[pixel_offset++] = (int)sprt->image[i++];
		}
	}
	sprt->pos.x = xf;
	sprt->pos.y = yf;
}

void sprite_move(sprite_t *sprt)
{
	sprt->pos.x = sprt->pos.x + sprt->vel.x;
	sprt->pos.y = sprt->pos.y + sprt->vel.y;
}
