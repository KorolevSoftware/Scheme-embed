#include "interpreter.h"
#include "parser.h"
#include <iso646.h>
#include <string.h>

static struct token_node* global_objects[10];
static int global_objects_count = 0;

static void add_variable(char* name, struct token_node* variable) {
	global_objects[global_objects_count] = variable;
	global_objects[global_objects_count]->my_token.name = name;
	global_objects_count++;
}

static struct token_node* get_variable(char* name) {
	for (int var = 0; var < global_objects_count; var++) {
		if (strcmp(global_objects[var]->my_token.name, name) == 0) {
			return global_objects[var];
		}
	}
	return NULL;
}

static struct token_node* set_define(struct token_node* stream) {
	const struct token_node* name = stream;
	const struct token_node* var_or_lambda = stream->body;
	add_variable(name->my_token.name, var_or_lambda);
	return NULL;
}

static struct token_node* get_define(struct token_node* stream) {
	const struct token_node* name = stream;
	return get_variable(name->my_token.name);
}

static struct token_node* plus_func(struct token_node* stream) {
	struct token_node* result = make_token_node();
	result->my_token.type = tt_integer;
	struct token_node* args_iter = stream->next_node;
	while (args_iter) {
		struct token_node* value = execute(args_iter);
		result->my_token.integer += value->my_token.integer;
		args_iter = args_iter->next_node;
	}
	return result;
}

static struct token_node* print_func(struct token_node* stream) {
	const struct token_node* result = stream->next_node;
	printf("%d\n", execute(result)->my_token.integer);
	return NULL;
}

static struct token_node* call_function_native(struct token_node* stream) {
	if (strcmp(stream->my_token.name, "+") == 0) {
		return plus_func(stream);
	}

	if (strcmp(stream->my_token.name, "print") == 0) {
		return print_func(stream);
	}
}

static struct token_node* call_begin(struct token_node* stream) {
	struct token_node* iter = stream->next_node;
	while (iter->next_node) {
		execute(iter);
		iter = iter->next_node;
	}
	return execute(iter);
}

static struct token_node* call_function(struct token_node* stream) {
	if (stream->my_token.type == tt_begin) {
		return call_begin(stream);
	}
	const struct token_node* lambda = execute(stream);
	if (lambda == NULL) {
		return call_function_native(stream);
	}
	size_t call_arguments_count = expression_length(stream->next_node);
	const struct token_node* lambda_args_name = lambda->body;

	struct token_node* call_arg_iter = stream->next_node;
	struct token_node* call_lambda_arg_iter = lambda_args_name;
	while (call_arg_iter) {
		struct token_node* variable = execute(call_arg_iter);
		add_variable(call_lambda_arg_iter->my_token.name, variable);
		call_arg_iter = call_arg_iter->next_node;
		call_lambda_arg_iter = call_lambda_arg_iter->next_node;
	}

	return execute(lambda->next_node);
}




struct token_node* run(struct token_node* stream) {
	struct token_node* iter = stream;
	while (iter) {
		execute(iter);
		iter = iter->next_node;
	}
}

struct token_node* execute(struct token_node* stream) {
	const struct token instruction = stream->my_token;

	if (instruction.type == tt_define) {
		return set_define(stream);
	}

	if (instruction.type == tt_expression) {
		return call_function(stream->body);
	}

	if (instruction.type == tt_ident) {
		return get_define(stream);
	}

	if (instruction.type == tt_begin) {
		return call_begin(stream);
	}

	if (instruction.type == tt_integer or instruction.type == tt_real) {
		return stream;
	}

	/*if (stream->next_node) {
		return execute(stream->next_node);
	}*/

	return NULL;
}

void init_interpreter() {

}
