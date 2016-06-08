#include "typedefs.h"
#include "entity.h"

typedef struct {
	ENTITY_MEMBERS;
	unsigned char *image;
} sprite_t;

//void sprite_new();
void sprite_draw(sprite_t*, screen_t*);
void sprite_move(sprite_t*);
