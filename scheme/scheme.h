#pragma once
#include "tokenizer.h"

struct token_node* eval(struct token_node* code);
struct token_node* compile(char* text_code);