
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

int key_word_by_name(char* stream, int word_lenght, char* name) {
	int string_size = strlen(name);
	if (string_size != word_lenght)
		return 0;
	if (memcmp(name, stream, string_size) != 0)
		return 0;
	return string_size;
}

#define key_word_func_by(fname) key_word_##fname
#define make_key_word_func(name, str_name) int key_word_##name(char* stream, int word_lenght){ \
return key_word_by_name(stream, word_lenght, str_name);\
}\

make_key_word_func(define, "define");
make_key_word_func(rbo, "(");
make_key_word_func(rbc, ")");
make_key_word_func(lambda, "lambda");

int key_word_func_by(identifier) (char* stream, int word_lenght) {
	if (strchr(stream, ',') or strchr(stream, '\\')) {
		return 0;
	}
	return word_lenght;
}

int key_word_func_by(real) (char* stream, int word_lenght) {
	char* enp_pos;
	strtof(stream, &enp_pos);
	int real_lenght = enp_pos - stream;
	if (real_lenght == word_lenght)
		return word_lenght;
	return 0;
}

int key_word_func_by(integer) (char* stream, int word_lenght) {
	char* enp_pos;
	strtol(stream, &enp_pos, 10);
	int integer_lenght = enp_pos - stream;
	if (integer_lenght == word_lenght)
		return word_lenght;
	return 0;
}

int key_word_func_by(string) (char* stream, int word_lenght) {
	if (*stream != '"')
		return 0;
	if (*(stream + word_lenght -1) != '"')
		return 0;

	return word_lenght;
}

struct token_node* make_token_node() {
	size_t node_size = sizeof(struct token_node);
	struct token_node* new_node = malloc(node_size);
	memset(new_node, 0, node_size);
	return new_node;
}

struct token_type_pair {
	enum token_type type;
	int (*func_cmp)(char*, int);
};

struct token_type_pair key_words [] = {
	{tt_define, key_word_func_by(define)},
	{tt_lambda, key_word_func_by(lambda)},
	{tt_rbo, key_word_func_by(rbo)},
	{tt_rbc, key_word_func_by(rbc)},
	{tt_integer, key_word_func_by(integer)},
	{tt_real, key_word_func_by(real)},
	{tt_string, key_word_func_by(string)},
	{tt_ident, key_word_func_by(identifier)},
};

char* get_word(char* stream) {
	if (*stream == ')' or *stream == '(')
		return ++stream;

	bool is_string = false;
	for (; *stream; stream++) {
		if (*stream == '"') {
			is_string = true;
			continue;
		}

		if (is_string and *stream == '"') {
			is_string = false;
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
		const int stream_offset = key_word.func_cmp(stream, word_lenght);
		
		if (stream_offset == 0)
			continue;

		input->stream = stream + stream_offset; // offset input stream

		if (key_word.type == tt_rbo) {
			node->body = tokenizer(input);
			node->next_node = tokenizer(input);
			return node;
		}

		if (key_word.type == tt_rbc) {
			return NULL;
		}

		node->my_token = (struct token) {
			.type = key_word.type,
			.word_begin = stream,
			.word_end = stream + stream_offset
		};

		node->next_node = tokenizer(input);
		return node;
	}

	printf("Error identificator: %.*s", word_lenght, stream);
	return NULL;
}
