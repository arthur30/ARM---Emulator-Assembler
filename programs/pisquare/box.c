#include "box.h"

void box_draw_wipe(game_state_t *state, box_t *box)
{
	rect_fill(state, (rect_t){box->pos, box->size},
						(colour_t){1, 1, 1, 0});
}

void box_draw(game_state_t *state, box_t *box)
{
	rect_fill(state, (rect_t){box->pos, box->size}, box->colour);
}

void box_move(game_state_t *state, box_t *box)
{
	(void)state;
	box->pos.x = box->pos.x + box->vel.x;
	box->pos.y = box->pos.y + box->vel.y;
}
