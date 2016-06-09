#ifndef TEXT_H
#define TEXT_H

#include "gamestate.h"
#include "typedefs.h"

void print_text(game_state_t *state, const char *string, vector2_t pos);
void print_text_color(game_state_t *state, const char *string, vector2_t pos,
								color_t col);
unsigned char *get_letter(char letter);

#endif
