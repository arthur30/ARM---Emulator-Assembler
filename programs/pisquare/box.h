#ifndef BOX
#define BOX
#include "typedefs.h"
#include "entity.h"

typedef struct {
	ENTITY_MEMBERS;
	colour_t colour;
} box_t;

box_t *box_new(void);
void box_draw_wipe(game_state_t*, box_t*);
void box_draw(game_state_t*, box_t*);
void box_move(game_state_t*, box_t*);

#endif
