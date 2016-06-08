#include "entity.h"

void draw_wipe(rect_t rect, screen_t *screen)
{
	//replace rect with background
	int offset;
        for(int y = 0; y < rect.size.y; y++ )
        {
            for(int x = 0; x < rect.size.x; x++ )
            {
        	offset = ( (x+rect.origin.x) * ( screen->bpp >> 3 ) ) + ( (y+rect.origin.y) * screen->pitch );
		screen->fb[ offset++ ] = 255;
		screen->fb[ offset++ ] = 255;
		screen->fb[ offset++ ] = 255;
		screen->fb[ offset++ ] = 255;
            }
	}
}
