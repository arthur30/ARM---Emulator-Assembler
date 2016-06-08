#include "box.h"

void box_draw_wipe(box_t *box, screen_t *screen)
{
	draw_wipe((rect_t){{box->pos.x,box->pos.y}, 
			   {box->size.x, box->size.y}}, screen);
}

void box_draw(box_t *box, screen_t *screen)
{
	int offset;
	colour_t col = box->colour;
        for (int y = 0; y < box->size.y; y++ )
        {
            for (int x = 0; x < box->size.x; x++ )
            {
                offset = ((x + box->pos.x) * ( screen->bpp >> 3 )) 
		       + ((y + box->pos.y) * screen->pitch);
		screen->fb[offset++] = col.r; 
                screen->fb[offset++] = col.g;
		screen->fb[offset++] = col.b;
		screen->fb[offset++] = col.a;
            }
        }
}

void box_move(box_t *box)
{
	box->pos.x = box->pos.x + box->vel.x;
	box->pos.y = box->pos.y + box->vel.y;
}
