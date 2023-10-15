#pragma once

enum token_type {
	tt_none,
	tt_expression,
	tt_define,
	tt_integer,
	tt_real, // float
	tt_rbo, // (
	tt_rbc, // )
	tt_string,
	tt_ident, // identificator
	tt_lambda,
	tt_endfile,
};

struct token {
	enum token_type type;
	const char* word_begin;
	const char* word_end;
};

struct token_node {
	struct token my_token;
	struct token_node* body;
	struct token_node* next_node;
};

struct input_tokenizer {
	char* stream;
};

struct token_node* tokenizer(struct input_tokenizer* input);
size_t expression_length(struct token_node* expression);