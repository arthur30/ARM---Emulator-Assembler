#ifndef PI_ERRORS_H
#define PI_ERRORS_H

#define PI_ERR_INPUT     "Could not open input file '%s': %s (%d)\n"
#define PI_ERR_INPUT_IO  "IO error on input file: %s (%d)\n"
#define PI_ERR_OUTPUT    "Could not open output file '%s': %s (%d)\n"
#define PI_ERR_OUTPUT_IO "IO error on output file: %s (%d)\n"
#define PI_ERR_MEM       "Could not allocate memory for: %s\n"
#define PI_ERR_GENERIC   "Error: %s (%d)\n"

#define EMU_ERR_ARGS         "Run as: emulate <input>\n"
#define EMU_ERR_BIN_SIZE     "Error: Binary too large, does not fit in memory\n"
#define EMU_ERR_EXEC         "Error: while executing instruction\n"
#define EMU_ERR_DECODE       "Error: while decoding instruction\n"
#define EMU_ERR_FETCH        "Error: while fetching instruction\n"
#define EMU_ERR_UNDEF_DPI_OP "Error: Undefined dpi opcode\n"
#define EMU_ERR_UNDEF_COND   "Error: Undefined dpi opcode\n"

#define EMU_RUN_OUT_OF_BOUNDS_MEM \
	"Error: Out of bounds memory access at address 0x%08zx\n"
#define EMU_RUN_GPIO_PIN_ACCESS1 \
	"One GPIO pin from 0 to 9 has been accessed\n"
#define EMU_RUN_GPIO_PIN_ACCESS2 \
	"One GPIO pin from 10 to 19 has been accessed\n"
#define EMU_RUN_GPIO_PIN_ACCESS3 \
	"One GPIO pin from 20 to 29 has been accessed\n"
#define EMU_RUN_GPIO_PIN_OFF "PIN OFF\n"
#define EMU_RUN_GPIO_PIN_ON  "PIN ON\n"

#define EMU_STATE_REG_HEAD "Registers:\n"
#define EMU_STATE_REG_GEN  "$%-2zd : %10d (0x%08x)\n"
#define EMU_STATE_REG_PC   "PC  : %10d (0x%08x)\n"
#define EMU_STATE_REG_CPSR "CPSR: %10d (0x%08x)\n"
#define EMU_STATE_MEM_HEAD "Non-zero memory:\n"
#define EMU_STATE_MEM_BGN  "0x%08zx: 0x"
#define EMU_STATE_MEM_BYTE "%02x"
#define EMU_STATE_MEM_END  "\n"

#define ASS_ERR_ARGS          "Run as: assemble <input> <output>\n"
#define ASS_ERR_TOKENIZE      "Error: Tokenization failed\n"
#define ASS_ERR_PARSE_NEAR    "Error: Parsing failed near line %zd col %zd\n"
#define ASS_ERR_PARSE_EOF     "Error: Parsing failed, unexpected end of file\n"
#define ASS_ERR_PASS1         "Error: First pass failed\n"
#define ASS_ERR_PASS2         "Error: Second pass failed\n"
#define ASS_ERR_JUMP_TARGET   "Error: No such label: %s\n"
#define ASS_ERR_INVALID_INSTR "Error: Invalid instruction\n"
#define ASS_ERR_OP2_FIT       "Error: Cannot fit '%zd' into operand2 value\n"
#define ASS_ERR_STR_IMM       "Error: Invalid str at immediate address\n"

#endif
