
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "tokenizer.h"
#include <iso646.h>

static struct object* make_empty_object() {
	struct object* new_object = malloc(sizeof(struct object));
	new_object->next = NULL;
	return new_object;
}

static struct token* skip_expresion(struct token* stream) {
	while (stream->type != tt_rbc) {
		if (stream->type != tt_rbc) {
			stream++;
			return stream;
		}
	}
}

static char* make_str(const char* begin, const char* end) {
	size_t str_len = end - begin;
	if (str_len < 1) {
		return NULL;
	}
	char* str = calloc(str_len + 1, sizeof(char));
	strncpy(str, begin, str_len);
	str[str_len + 1] = "";

	return str;
}

struct object* make_number(const struct token* stream, size_t* index) {
	const struct token namber = stream[*index];
	struct object* new_object = make_empty_object();

	if (namber.type == tt_real) {
		new_object->type = ot_real;
		new_object->real.value = atof(namber.word_begin);
	}

	if (namber.type == tt_integer) {
		new_object->type = ot_integer;
		new_object->integer.value = atoi(namber.word_begin);
	}
	index++; //skip token
	return new_object;
}

struct object* make_function(struct token* stream, size_t* index) {
	const struct token func_name = *stream;
	struct object* new_object = make_empty_object();
	new_object->name = make_str(stream->word_begin, stream->word_end);

	while (stream->type != tt_rbc) {
		if (stream->type == tt_rbo) {
			struct object* expression = tokens_to_objects(stream, index);
		}

		if (stream->type)
			stream++;
	}
}


struct object* make_define(struct token_node* nodes) {
	const struct token ident_or_expression = nodes->my_token;

	//const struct token its = stream[(*index) + 1]; // number, func, lambda func
	struct object* new_object = make_empty_object();
	new_object->type = ot_define;

	if (ident_or_expression.type == tt_ident) {
		new_object->name = make_str(ident_or_expression.word_begin, ident_or_expression.word_end);
		new_object->next = tokens_to_objects(nodes->next_node);
	}
	//if (name_or_rbo.type == tt_ident) {
	//	(*index)++;
	//	new_object->next = tokens_to_objects(stream, index);
	//	new_object->name = make_str(name_or_rbo.word_begin, name_or_rbo.word_end);
	//}

	//if(name_or_rbo.type == tt_rbo) { // declarate function
	//	struct object** sequence = calloc(10, sizeof(struct object*));
	//	int sequence_len = 0;
	//	while (stream[*index].type != tt_rbc) {
	//		sequence[sequence_len] = tokens_to_objects(stream, index);
	//		sequence_len++;
	//	}
	//	(*index)++;

	//}

	return new_object;
}



struct object* make_call_function(const struct token* stream, size_t* index) {
	const struct token func_name = stream[*index];
	struct object* new_object_head = make_empty_object();
	new_object_head->name = make_str(func_name.word_begin, func_name.word_end);
	new_object_head->type = ot_fuction;
	struct object* new_object = new_object_head;
	(*index)++;
	struct token current_token = stream[*index];
	while (current_token.type != tt_rbc) {
		struct object* temp_object = NULL;
		if (current_token.type == tt_rbo) {
			temp_object = tokens_to_objects(stream, index);
		}

		if (current_token.type == tt_ident) {
			temp_object = make_empty_object();
			temp_object->name = make_str(current_token.word_begin, current_token.word_end);
			temp_object->type = ot_argument;
		}

		if (current_token.type == tt_real || current_token.type == tt_integer) {
			temp_object = make_number(stream, index);
		}

		if (temp_object == NULL) {
			printf("Error parsing");
			break;
		}
		new_object->next = temp_object;
		new_object = new_object->next;
		(*index)++;
		current_token = stream[*index];
	}
	(*index)++;
	return new_object_head;
}

struct object* make_object_sequence(struct token_node* nodes) {
	if (not nodes) {
		return NULL;
	}
	struct object* object = tokens_to_objects(nodes);
	object->next = make_object_sequence(nodes->next_node);
	return object;
}

struct object* make_lambda(struct token_node* nodes) {
	struct object* new_object = make_empty_object();
	new_object->type = ot_fuction;

	int arguments_count = expression_length(nodes->body);
	new_object->function.arguments_count = arguments_count;
	if (arguments_count > 0) {

	}
	int body_length = expression_length(nodes->next_node);


	//for (int body = 0; body < body_length; body++) {
	//	tokens_to_objects(nodes->next_node);

	//}
	new_object->function.body = make_object_sequence(nodes->next_node);

	return new_object;
}

struct object* make_ident(struct token_node* nodes) {
	const struct token ident = nodes->my_token;
	struct object* new_object = make_empty_object();
	new_object->type = ot_ident;
	new_object->name = make_str(ident.word_begin, ident.word_end);
	return new_object;
}

struct object* make_string(struct token_node* nodes) {
	struct object* new_object = make_empty_object();
	const struct token ident_or_expression = nodes->my_token;
	new_object->type = ot_string;
	new_object->name = make_str(ident_or_expression.word_begin, ident_or_expression.word_end);
	return new_object;
}

struct object* tokens_to_objects(struct token_node* nodes) {
	const struct token current_token = nodes->my_token;
	if (current_token.type == tt_expression) {
		return tokens_to_objects(nodes->body);
	}

	if (current_token.type == tt_define) {
		return make_define(nodes->next_node);
	}

	if (current_token.type == tt_lambda) {
		return make_lambda(nodes->next_node);
	}

	if (current_token.type == tt_string) {
		return make_string(nodes);
	}

	if (current_token.type == tt_ident) {
		return make_ident(nodes);
	}

};