#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "instructions.h"

struct mnemonic {
	char *str;
	int n;
};

struct mnemonic dict_dpi[] = {
	"and", 0,
	"eor", 1,
	"sub", 2,
	"rsb", 3,
	"add", 4,
	"orr", 12,
	"mov", 13,
	"tst", 8,
	"teq", 9,
	"cmp", 10,
	0, 0
};

struct mnemonic dict_branch[] = {
	"beq", 0,
	"bne", 1,
	"bge", 10,
	"blt", 11,
	"bgt", 12,
	"ble", 13,
	"b", 14,
	"bal", 14,
	0, 0
};

/* dpi > 0 to search for DPI keys */

int key_to_int(char *key, int dpi)
{
	int i = 0;
	struct mnemonic *dict;
	dict = dict_branch;
	if (dpi) {
		dict = dict_dpi;
	}
	char *cand = dict[i].str;
	while (cand) {
		if (strcmp(cand, key) == 0)
			return dict[i].n;
		cand = dict[++i].str;	
	}
	return -1;
}

/* All functions return a 32bit integer. */

uint32_t instr_dpi(void)
{
	/*
	 * Instruction Result:
	 * COND 00 I OpCode S -Rn- -Rd- Operand2
	 * COND => 1110
	 * OpCode => See from Spec.
	 * S => (tst, teq, cmp -> 1) || (rest -> 0)
	 * I => Set if Operand2 is an immediate const.
	 * Operand2 => <#expression> || Rm{,<shift>}
	 */

	/* int opcode = key_to_int(OPCODE_FROM_STRUCT, 1); */
	int cond = 14 << 28;
	return 0;
}

uint32_t instr_multiply(void)
{
	/*
	 * Instruction Result:
	 * COND 0000 00AS -Rd- -Rn- -Rs- 1001 -Rm-
	 * COND => 1110
	 * A => (mul -> 0) || (mla -> 1)
	 * S => 0
	 * mla r1, r2, r3, r4 => r1 = (r2 x r3) + r4
	 * mul r1, r2, r3 => r1 = r2 x r3
	 */
	uint32_t cond = 14 << 28;
	uint32_t a = 1 << 21;
	uint32_t s = 0;
	uint32_t rd;
	uint32_t rn;
	uint32_t rs;
	uint32_t rm;

	return cond + a + s + rd + rn + rs + 144 + rm;
}

uint32_t instr_sdt(void)
{
	/*
	 * Instruction Result:
	 * COND 01IP U00L -Rn- -Rd- Offset
	 * L => (ldr -> 1) || (sdr -> 0)
	 * I => Set if Offset is shifted register.
	 * U => Set if Offset is added to base reg.
	 * P => Pre/Post indexing bit. See spec pg9.
	 */
	return 0;
}

uint32_t instr_branch(void)
{
	/*
	 * Instruction Result:
	 * COND 1010 Offset
	 * COND => See from the spec.
	 */
	return 0;
}
