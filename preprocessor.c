#include "preprocessor.h"
#include <stddef.h>
#include <iso646.h>

static struct token_node* make_lambda(struct token_node* params, struct token_node* body) {
	struct token_node* lambda = make_token_node();
	struct token_node* arguments_and_body = make_token_node();

	lambda->my_token.type = tt_lambda;
	lambda->body = params;
	
	size_t length = expression_length(body);

	if (length == 1) {
		lambda->next_node = body;
	} else {
		struct token_node* begin = make_token_node();
		begin->my_token.type = tt_begin;
		begin->next_node = body;
		lambda->next_node = begin;
	}
	return lambda;
}

static struct token_node* is_lambda(struct token_node* nodes) {
	const struct token_node* lambda_arguments = nodes->body;
	const struct token_node* lambda_body = nodes->next_node;
	return make_lambda(lambda_arguments, lambda_body);
}

static struct token_node* is_define(struct token_node* nodes) {
	const struct token ident_or_expression = nodes->my_token;
	struct token_node* define_node = make_token_node();
	define_node->my_token.type = tt_define;

	//struct token_node* new_object = make_empty_object();
	
	if (ident_or_expression.type == tt_expression) {
		struct token_node* arguments = nodes->body;
		char* ident_name = arguments->my_token.name;
		struct token_node* arguments_without_name = arguments->next_node;
		define_node->my_token.name = ident_name;
		define_node->body = make_lambda(arguments_without_name, nodes->next_node);
	}

	if (ident_or_expression.type == tt_ident) {
		define_node->my_token.name = ident_or_expression.name;
		define_node->body = preprocessor(nodes->next_node);
	}
	return define_node;
}


struct token_node* preprocessor(struct token_node* tokens) {
	if (tokens == NULL) {
		return NULL;
	}
	const struct token current_token = tokens->my_token;
	struct token_node* result = NULL;
	if (current_token.type == tt_expression) {
		result = preprocessor(tokens->body);
		if (result == NULL) {
			result = tokens; // is call function
		}
	}

	if (current_token.type == tt_define) {
		result = is_define(tokens->next_node);
	}

	if (current_token.type == tt_lambda) {
		result = is_lambda(tokens->next_node);
	}

	if (current_token.type == tt_begin) {
		result = tokens;// is_lambda(tokens->next_node);
	}

	if (current_token.type == tt_real or current_token.type == tt_integer or current_token.type == tt_string) {
		result = tokens;
	}
	if (tokens->next_node and result) {
		result->next_node = preprocessor(tokens->next_node);
	}
	return result;
}