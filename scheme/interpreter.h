#pragma once

#include "tokenizer.h"


struct token_node* execute(struct token_node* stream);
struct token_node* run(struct token_node* stream);
void init_interpreter(void);