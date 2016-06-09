#include "text.h"
#include <string.h>

#include "assets/font/A.h"
#include "assets/font/B.h"
#include "assets/font/C.h"
#include "assets/font/D.h"
#include "assets/font/E.h"
#include "assets/font/F.h"
#include "assets/font/G.h"
#include "assets/font/H.h"
#include "assets/font/I.h"
#include "assets/font/J.h"
#include "assets/font/K.h"
#include "assets/font/L.h"
#include "assets/font/M.h"
#include "assets/font/N.h"
#include "assets/font/O.h"
#include "assets/font/P.h"
#include "assets/font/Q.h"
#include "assets/font/R.h"
#include "assets/font/S.h"
#include "assets/font/T.h"
#include "assets/font/U.h"
#include "assets/font/V.h"
#include "assets/font/W.h"
#include "assets/font/X.h"
#include "assets/font/Y.h"
#include "assets/font/Z.h"
#include "assets/font/0.h"
#include "assets/font/1.h"
#include "assets/font/2.h"
#include "assets/font/3.h"
#include "assets/font/4.h"
#include "assets/font/5.h"
#include "assets/font/6.h"
#include "assets/font/7.h"
#include "assets/font/8.h"
#include "assets/font/9.h"
#include "assets/font/DOT.h"
#include "assets/font/QUES.h"
#include "assets/font/COMMA.h"
#include "assets/font/COLON.h"
#include "assets/font/SPACE.h"
#include "assets/font/EX.h"

void print_text_colour(game_state_t *state, const char *string, vector_t pos,
		colour_t col)
{
	int len;
	int offset;
	int disp;
	int j;
	unsigned char *letter;

	len = strlen(string);
	disp = 0;
	j = 8;

	for (int i = 0; i < len; i++) {
		letter = get_letter(string[i]);
		for (int y = 0; y < 20; y++) {
			for (int x = 0; x < 20; x++) {
				offset = (x + pos.x + disp)
					* (state->screen.bpp >> 3)
				+ ((y + pos.y) * state->screen.pitch);
				if (letter[j+3] != 0x00) {
					state->screen.fb[offset++] =
					letter[(int)(col.b * (float)j++/255)];
					state->screen.fb[offset++] =
					letter[(int)(col.g * (float)j++/255)];
					state->screen.fb[offset++] =
					letter[(int)(col.r * (float)j++/255)];
					state->screen.fb[offset++] =
					letter[j++];
				} else
					j += 4;
			}
		}
		disp += 20;
		j = 8;
	}

}

void print_text(game_state_t *state, const char *string, vector_t pos)
{
	print_text_colour(state, string, pos, (colour_t){255, 255, 255, 255});
}

unsigned char *get_letter(char letter)
{
	switch (letter) {
	case 32:
		return SPACE_bin;
	case 33:
		return EX_bin;
	case 44:
		return COMMA_bin;
	case 46:
		return DOT_bin;
	case 48:
		return __0_bin;
	case 49:
		return __1_bin;
	case 50:
		return __2_bin;
	case 51:
		return __3_bin;
	case 52:
		return __4_bin;
	case 53:
		return __5_bin;
	case 54:
		return __6_bin;
	case 55:
		return __7_bin;
	case 56:
		return __8_bin;
	case 57:
		return __9_bin;
	case 58:
		return COLON_bin;
	case 63:
		return QUES_bin;
	case 65:
		return A_bin;
	case 66:
		return B_bin;
	case 67:
		return C_bin;
	case 68:
		return D_bin;
	case 69:
		return E_bin;
	case 70:
		return F_bin;
	case 71:
		return G_bin;
	case 72:
		return H_bin;
	case 73:
		return I_bin;
	case 74:
		return J_bin;
	case 75:
		return K_bin;
	case 76:
		return L_bin;
	case 77:
		return M_bin;
	case 78:
		return N_bin;
	case 79:
		return O_bin;
	case 80:
		return P_bin;
	case 81:
		return Q_bin;
	case 82:
		return R_bin;
	case 83:
		return S_bin;
	case 84:
		return T_bin;
	case 85:
		return U_bin;
	case 86:
		return V_bin;
	case 87:
		return W_bin;
	case 88:
		return X_bin;
	case 89:
		return Y_bin;
	case 90:
		return Z_bin;
	default:
		return COMMA_bin;
	}
}
