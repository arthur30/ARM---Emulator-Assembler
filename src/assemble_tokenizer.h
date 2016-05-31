#ifndef ASSEMBLE_TOKENIZER_H
#define ASSEMBLE_TOKENIZER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

enum token_type {
	TOKEN_TYPE_STRING,
	TOKEN_TYPE_NUMBER,
	TOKEN_TYPE_BRACKET_OPEN,
	TOKEN_TYPE_BRACKET_CLOSE,
	TOKEN_TYPE_NEWLINE
};

struct token {
	enum token_type type;
	size_t lineno;
	size_t colno;
	char *str;
	size_t strlen;
	int64_t num;
};

struct token_list {
	size_t size;
	size_t capacity;
	struct token *tokens;
};

int token_list_alloc(struct token_list *list);
struct token *token_list_newtok(struct token_list *list);
struct token *token_list_getnext(struct token_list *list, size_t *pos);
int tokenize(FILE *input, struct token_list *tokens);

#endif
