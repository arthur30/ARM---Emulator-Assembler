#ifndef ASSEMBLE_TOKENIZER_H
#define ASSEMBLE_TOKENIZER_H

struct instruction {
	char *label;
	char *mnemonic;
	char *op1;
	char *op2;
	char *op3;
	char *op4;
};

struct instruction tokens;

struct instruction tokenize(char *instr);

#endif
