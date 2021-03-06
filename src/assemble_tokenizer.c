#include "assemble_tokenizer.h"

#include "pi_msgs.h"

#include <stdio.h>
#include <string.h>

#define CAPACITY_INCREMENT 16

static int token_list_extend(struct token_list *list)
{
	size_t nsize;

	list->capacity += CAPACITY_INCREMENT;
	nsize = list->capacity * sizeof(struct token);
	list->tokens = realloc(list->tokens, nsize);
	if (!list->tokens) {
		fprintf(stderr, PI_ERR_MEM, "extending token list");
		return -1;
	}

	return 0;
}

int token_list_alloc(struct token_list *list)
{
	list->size = 0;
	list->capacity = 0;
	list->tokens = NULL;
	return token_list_extend(list);
}

struct token *token_list_newtok(struct token_list *list)
{
	struct token *tok;

	if (list->size == list->capacity) {
		if (token_list_extend(list))
			return NULL;
	}

	tok = &list->tokens[list->size++];
	memset(tok, 0, sizeof(struct token));

	return tok;
}

struct token *token_list_getnext(struct token_list *list, size_t *pos)
{
	if (*pos < list->size) {
		++(*pos);
		return &list->tokens[*pos - 1];
	}

	return NULL;
}

int tokenize(FILE *input, struct token_list *tokens)
{
	size_t lineno;
	char *line, *tok, *str;
	size_t line_size, tok_size;
	struct token *ctoken;

	line = NULL;
	line_size = 0;
	lineno = 0;

	while (getline(&line, &line_size, input) > 0) {
		++lineno;

		str = line;
		while ((tok = strtok(str, " ,\t\n"))) {

			/* if comment found discard rest of line */
			if (tok[0] == ';')
				break;

			str = NULL;

			tok_size = strlen(tok);

			if (tok[0] == '[' || tok[0] == ']') {
				ctoken = token_list_newtok(tokens);
				ctoken->lineno = lineno;
				ctoken->colno = tok - line;
				switch (tok[0]) {
				case '[':
					ctoken->type = TOKEN_TYPE_BRACKET_OPEN;
					break;
				case ']':
					ctoken->type = TOKEN_TYPE_BRACKET_CLOSE;
					break;
				}
				++tok;
				--tok_size;
				if (tok_size == 0)
					continue;
			}

			ctoken = token_list_newtok(tokens);
			ctoken->lineno = lineno;
			ctoken->colno = tok - line;

			if (tok[0] == '=' || tok[0] == '#') {
				ctoken->type = TOKEN_TYPE_NUMBER;
				ctoken->num = strtoll(tok + 1, NULL, 0);
			} else {
				ctoken->type = TOKEN_TYPE_STRING;
				if (tok[tok_size - 1] == ']')
					ctoken->strlen = tok_size - 1;
				else
					ctoken->strlen = tok_size;
				ctoken->str = strndup(tok, ctoken->strlen);
			}

			if (tok[tok_size - 1] == ']') {
				ctoken = token_list_newtok(tokens);
				ctoken->lineno = lineno;
				ctoken->colno = &tok[tok_size - 1] - line;
				ctoken->type = TOKEN_TYPE_BRACKET_CLOSE;
			}
		}

		ctoken = token_list_newtok(tokens);
		ctoken->type = TOKEN_TYPE_NEWLINE;
	}

	return 0;
}
