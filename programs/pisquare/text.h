#ifndef TEXT_H
#define TEXT_H

#include "gamestate.h"
#include "typedefs.h"

void print_text(game_state_t *state, const char *string, vector_t pos);
unsigned char *get_letter(char letter);

#endif
