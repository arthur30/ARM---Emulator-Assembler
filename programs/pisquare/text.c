#include "text.h"

#include <string.h>

#include "assets/assets.h"

void print_text_color(game_state_t *state, const char *string, vector2_t pos,
		      color_t col)
{
	size_t len;
	vector2_t p;
	unsigned char *letter;

	len = strlen(string);
	p = pos;

	for (size_t i = 0; i < len; i++) {
		letter = get_letter(string[i]);
		graphics_draw_image(state->device, &p, letter, &col);
		p.x += 20;
	}

}

void print_text(game_state_t *state, const char *string, vector2_t pos)
{
	print_text_color(state, string, pos, (color_t){255, 255, 255, 255});
}

unsigned char *get_letter(char letter)
{
	switch (letter) {
	case 32:
		return font_SPACE_bin;
	case 33:
		return font_EX_bin;
	case 44:
		return font_COMMA_bin;
	case 46:
		return font_DOT_bin;
	case 48:
		return font_0_bin;
	case 49:
		return font_1_bin;
	case 50:
		return font_2_bin;
	case 51:
		return font_3_bin;
	case 52:
		return font_4_bin;
	case 53:
		return font_5_bin;
	case 54:
		return font_6_bin;
	case 55:
		return font_7_bin;
	case 56:
		return font_8_bin;
	case 57:
		return font_9_bin;
	case 58:
		return font_COLON_bin;
	case 63:
		return font_QUES_bin;
	case 65:
		return font_A_bin;
	case 66:
		return font_B_bin;
	case 67:
		return font_C_bin;
	case 68:
		return font_D_bin;
	case 69:
		return font_E_bin;
	case 70:
		return font_F_bin;
	case 71:
		return font_G_bin;
	case 72:
		return font_H_bin;
	case 73:
		return font_I_bin;
	case 74:
		return font_J_bin;
	case 75:
		return font_K_bin;
	case 76:
		return font_L_bin;
	case 77:
		return font_M_bin;
	case 78:
		return font_N_bin;
	case 79:
		return font_O_bin;
	case 80:
		return font_P_bin;
	case 81:
		return font_Q_bin;
	case 82:
		return font_R_bin;
	case 83:
		return font_S_bin;
	case 84:
		return font_T_bin;
	case 85:
		return font_U_bin;
	case 86:
		return font_V_bin;
	case 87:
		return font_W_bin;
	case 88:
		return font_X_bin;
	case 89:
		return font_Y_bin;
	case 90:
		return font_Z_bin;
	default:
		return font_COMMA_bin;
	}
}
