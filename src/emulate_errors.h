#ifndef EMULATE_ERRORS_H
#define EMULATE_ERRORS_H

#define NOT_ENOUGH_ARGS "Provide an input binary as an argument\n"
#define BINARY_TOO_LARGE "Binary cannot be loaded into memory\n"

#define OUT_OF_BOUNDS_MEM \
	"Error: Out of bounds memory access at address 0x%08zx\n"
#define GPIO_PIN_ACCESS \
	"One GPIO pin from %d to %d has been accessed\n"
#define GPIO_PIN_OFF "PIN OFF\n"
#define GPIO_PIN_ON "PIN ON\n"

#endif
