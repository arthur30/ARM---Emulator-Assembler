#include "typedefs.h"
#include "entity.h"

typedef struct {
	ENTITY_MEMBERS;
	colour_t colour;
} box_t;

box_t *box_new(void);
void box_draw_wipe(box_t *box, screen_t*);
void box_draw(box_t*, screen_t*);
void box_move(box_t*);
