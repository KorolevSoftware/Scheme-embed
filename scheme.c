#include "scheme.h"
#include "parser.h"
#include "tokenizer.h"
#include "interpreter.h"
#include "preprocessor.h"
#include "stdio.h"

struct token_node* compile(char* text_code) {
	struct input_tokenizer input1;
	input1.stream = text_code;

	struct token_node* token_nodes = tokenizer(&input1);
	struct token_node* token_nodes_press = preprocessor(token_nodes);

	return token_nodes_press;
}

struct token_node* eval(struct token_node* code) {
	return run(code);
}