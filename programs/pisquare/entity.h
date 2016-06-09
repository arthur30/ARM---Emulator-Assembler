#ifndef ENTITY
#define ENTITY

#include "typedefs.h"
#include "random.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
	vector_float_t pos;
	vector_t size;
	vector_float_t vel;
	vector_float_t acc;
} entity_t;

entity_t *entity_new(void);
void entity_free(entity_t *ent);
rect_t entity_rect(entity_t *);
bool entity_test_collision(entity_t*, entity_t*);

#endif
