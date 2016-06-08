#ifndef ENTITY
#define ENTITY
#include "typedefs.h"

typedef struct {
	vector_t size;
	vector_t pos;
	vector_t vel;
	vector_t acc;
} entity_t;

entity_t *entity_new(void);
rect_t entity_rect(entity_t *);

#endif
