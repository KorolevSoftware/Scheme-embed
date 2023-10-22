//EBNF grammar :
//<digit> = 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
//<number> = { <digit> }
//<op> = -| +| *| /
//<expression> = (<op> <expression> <expression>) | <number>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "tokenizer.h"
#include "iso646.h"

#define array_size(x) sizeof(x)/sizeof(x[0])

static char* make_string(const char* word, int length) {
	if (length < 1) {
		return NULL;
	}
	char* str = calloc(length + 1, sizeof(char));
	strncpy(str, word, length);
	str[length + 1] = "";
	return str;
}

struct token key_word_by_name(char* word, int word_lenght, char* token_name, enum token_type type) {
	int string_size = strlen(token_name);
	if (string_size != word_lenght)
		return (struct token) {.type = tt_none};
	if (memcmp(token_name, word, string_size) != 0)
		return (struct token) { .type = tt_none };
	return (struct token) { .type = type };
}

#define key_word_func_by(fname) key_word_##fname
#define make_key_word_func(name, token_name, token_type) struct token key_word_##name(char* word, int word_lenght, enum token_type type){ \
return key_word_by_name(word, word_lenght, token_name, token_type);\
}\

make_key_word_func(define, "define", tt_define);
make_key_word_func(rbo, "(", tt_rbo);
make_key_word_func(rbc, ")", tt_rbc);
make_key_word_func(lambda, "lambda", tt_lambda);
make_key_word_func(begin, "begin", tt_begin);
make_key_word_func(ifsc, "if", tt_if);


struct token key_word_func_by(identifier) (char* stream, int word_lenght) {
	if (strchr(stream, ',') or strchr(stream, '\\')) {
		return (struct token) { .type = tt_none };
	}
	return (struct token) { .type = tt_ident, .name = make_string(stream, word_lenght) };
}

struct token key_word_func_by(real) (char* stream, int word_lenght) {
	char* enp_pos;
	float value = strtof(stream, &enp_pos);
	int real_lenght = enp_pos - stream;
	if (real_lenght == word_lenght)
		return (struct token) { .type = tt_real, .real = value};
	return (struct token) { .type = tt_none };
}

struct token key_word_func_by(integer) (char* stream, int word_lenght) {
	char* enp_pos;
	int value = strtol(stream, &enp_pos, 10);
	int integer_lenght = enp_pos - stream;
	if (integer_lenght == word_lenght)
		return (struct token) { .type = tt_integer, .integer = value };
	return (struct token) { .type = tt_none };
}

struct token key_word_func_by(string) (char* stream, int word_lenght) {
	if (stream[0] != '"')
		return (struct token) { .type = tt_none };
	if (stream[word_lenght - 1] != '"')
		return (struct token) { .type = tt_none };

	return (struct token) { .type = tt_string, .string = make_string(stream+1, word_lenght-2) };
}

struct token_node* make_token_node() {
	size_t node_size = sizeof(struct token_node);
	struct token_node* new_node = malloc(node_size);
	memset(new_node, 0, node_size);
	return new_node;
}

struct token_type_pair {
	enum token_type type;
	struct token (*func_cmp)(char*, int);
};

struct token_type_pair key_words [] = {
	{tt_define, key_word_func_by(define)},
	{tt_lambda, key_word_func_by(lambda)},
	{tt_begin, key_word_func_by(begin)},
	{tt_rbo, key_word_func_by(rbo)},
	{tt_rbc, key_word_func_by(rbc)},
	{tt_integer, key_word_func_by(integer)},
	{tt_real, key_word_func_by(real)},
	{tt_if, key_word_func_by(ifsc)},

	/// Must be last
	{tt_string, key_word_func_by(string)},
	{tt_ident, key_word_func_by(identifier)},
};

static char* get_word(char* stream) {
	if (*stream == ')' or *stream == '(')
		return ++stream;

	bool is_string = false;
	for (; *stream; stream++) {
		if (not is_string and *stream == '"') {
			is_string = true;
			continue;
		}

		if (is_string and *stream == '"') {
			is_string = false;
			stream++;
			return stream;
		}

		if ((not is_string and isspace(*stream)) or *stream == ')' or *stream == '(') {
			return stream;
		}
	}
	return NULL;
}

struct token_node* tokenizer(struct input_tokenizer* input) {
	char* stream = input->stream;
	if (not *stream) {
		return NULL;
	}

	while (isspace(*stream)) {
		stream++;
	}

	struct token_node* node = make_token_node();

	const char* word = get_word(stream);
	const int word_lenght = word - stream;

	for (int key = 0; key < array_size(key_words); key++) {
		const struct token_type_pair key_word = key_words[key];
		struct token new_token = key_word.func_cmp(stream, word_lenght);
		
		if (new_token.type == tt_none)
			continue;

		input->stream = stream + word_lenght; // offset input stream

		if (key_word.type == tt_rbo) {
			node->my_token.type = tt_expression;
			node->body = tokenizer(input);
			node->next_node = tokenizer(input);
			return node;
		}

		if (key_word.type == tt_rbc) {
			return NULL;
		}

		node->my_token = new_token;
		node->next_node = tokenizer(input);
		return node;
	}

	printf("Error identificator: %.*s", word_lenght, stream);
	return NULL;
}

size_t expression_length(struct token_node* expression) {
	if (not expression) {
		return 0;
	}

	struct token_node* iter = expression;
	size_t length = 1;
	for (; iter->next_node; length++) {
		iter = iter->next_node;
	}
	return length;
}

size_t expression_body_length(struct token_node* expression) {
	if (not expression) {
		return 0;
	}

	struct token_node* iter = expression->body;
	size_t length = 1;
	for (; iter->next_node; length++) {
		iter = iter->next_node;
	}
	return length;
}
