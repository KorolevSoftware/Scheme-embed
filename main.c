#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define or ||
#define array_size(x) sizeof(x)/sizeof(x[0])
#define key_word_func_by(fname) key_word_##fname
#define make_key_word_func(name, str_name) int key_word_##name(char* stream, int word_lenght){ \
int string_size = strlen(str_name);\
if (string_size != word_lenght)\
	return 0;\
if (memcmp(str_name, stream, string_size) != 0)\
	return 0;\
return string_size;\
}

enum token_type {
	tt_define,
	tt_integer,
	tt_real, // float
	tt_rbo, // (
	tt_rbc, // )
	tt_ident, // identificator
	//tt_plus, // +
	//tt_minus, // -
	tt_lambda,
};


//struct object {
//	typeid
//};

make_key_word_func(define, "define");
make_key_word_func(rbo, "(");
make_key_word_func(rbc, ")");
make_key_word_func(plus, "+");
make_key_word_func(minus, "-");
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


struct token_type_pair {
	enum token_type type;
	int (*func_cmp)(char *, int);
};

struct token_type_pair key_words [] = {
	{tt_define, key_word_func_by(define)},
	{tt_lambda, key_word_func_by(lambda)},
	{tt_rbo, key_word_func_by(rbo)},
	{tt_rbc, key_word_func_by(rbc)},
	//{tt_plus, key_word_func_by(plus)},
	//{tt_minus, key_word_func_by(minus)},
	{tt_integer, key_word_func_by(integer)},
	{tt_real, key_word_func_by(real)},
	{tt_ident, key_word_func_by(identifier)},
};

struct token {
	enum token_type type;
	const char* word_begin;
	const char* word_end;
};

struct token token_list [100];
int token_list_index = 0;

void add_token(struct token new_token) {
	token_list[token_list_index] = new_token;
	token_list_index++;
}

char* get_word(char* stream) {
	if (*stream == ')' or *stream == '(')
		return ++stream;

	for (; *stream; stream++) {
		if (isspace(*stream) or *stream == ')' or *stream == '(') {
			return stream;
		}
	}
	return NULL;
}

enum defines_type {
	variable,
	function,
	lambda,
};

struct defines {
	int value;
	char name[10];
};

struct defines_function_native {
	char name[10];
	int (*func)(int key_word_idx);
};

struct defines_function {
	char name[10];
	int index_arguments;
	int index_body;
};

struct defines global_variables[10];
int global_variables_count = 0;

struct defines local_variables[10];
int local_variables_count = 0;

struct defines_function gloabl_function[10];
int gloabl_function_count = 0;


struct defines_function_native gloabl_function_native[10];
int defines_function_native_count = 1;

int plus(int key_word) {
	int t = 0;
	int sum = 0;
	key_word++;
	while (true) {
		if (token_list[key_word].type == tt_rbc) {
			break;
		}
		sum += eval(key_word++);
	}
	return sum;
}

void init_native_function() {
	gloabl_function_native[0] =
		(struct defines_function_native){ .name = "+", .func = plus };
}

bool is_define_variable(int key_word_idx) {
	const struct token key_word = token_list[key_word_idx];
	const struct token name = token_list[key_word_idx + 1];
	const struct token value = token_list[key_word_idx + 2];

	if (key_word.type == tt_define &&
		name.type == tt_ident &&
		(value.type == tt_real || value.type == tt_integer)) {

		struct defines* temp_def = &global_variables[global_variables_count];
		strncpy(temp_def->name, name.word_begin, name.word_end - name.word_begin);
		temp_def->value = atoi(value.word_begin);

		global_variables_count++;
		return true;
	}
	return false;
}

bool is_define_function(int key_word_idx) {
	const struct token key_word = token_list[key_word_idx];
	const struct token rbo = token_list[key_word_idx + 1];
	const struct token name = token_list[key_word_idx + 2];

	if (key_word.type == tt_define &&
		name.type == tt_ident &&
		rbo.type == tt_rbo) {
		struct defines_function* temp_def = &gloabl_function[gloabl_function_count];

		strncpy(temp_def->name, name.word_begin, name.word_end - name.word_begin);
		
		temp_def->index_arguments = key_word_idx + 3;
		// search close round board
		int index_body = temp_def->index_arguments;
		for (; token_list[index_body].type != tt_rbo; index_body++);
		temp_def->index_body = index_body;
		gloabl_function_count++;
		return true;
	}
	return false;
}



bool is_get_variable(int key_word_idx, int* number) {
	const struct token var_name_or_number = token_list[key_word_idx];
	const struct token next_token = token_list[key_word_idx + 1];
	if (var_name_or_number.type == tt_ident) {
		for (int index = 0; index < global_variables_count; index++) {
			struct defines* temp_var = &global_variables[index];
			if (strncmp(temp_var->name, var_name_or_number.word_begin, strlen(temp_var->name)) == 0) {
				*number = temp_var->value;
				return true;
			}
		}
	}

	if (var_name_or_number.type == tt_ident) {
		for (int index = 0; index < local_variables_count; index++) {
			struct defines* temp_var = &local_variables[index];
			if (strncmp(temp_var->name, var_name_or_number.word_begin, strlen(temp_var->name)) == 0) {
				*number = temp_var->value;
				return true;
			}
		}
	}

	if (var_name_or_number.type == tt_integer) {
		*number = atoi(var_name_or_number.word_begin);
		return true;
	}

	return false;
}

bool is_call_function(int key_word_idx) {
	const struct token func_name = token_list[key_word_idx];

	if (func_name.type != tt_ident) {
		return false;
	}
	int program_call_define = -1;
	for (int program_define = 0; program_define < gloabl_function_count; program_define++) {
		if (strncmp(
			func_name.word_begin,
			gloabl_function[program_define].name,
			func_name.word_end - func_name.word_begin) == 0
			) {
			program_call_define = program_define;
			break;
		}
	}
	int program_call_native = -1;
	for (int program_define = 0; program_define < defines_function_native_count; program_define++) {
		if (strncmp(
			func_name.word_begin,
			gloabl_function_native[program_define].name,
			func_name.word_end - func_name.word_begin) == 0
			) {
			program_call_native = program_define;
			break;
		}
	}

	if (program_call_define == -1 && program_call_native == -1) {
		printf("Error function dont define");
		return;
	}


	// put arguments to local variable
	if (program_call_define != -1) {
		int argument_f = gloabl_function[program_call_define].index_arguments;
		for (; token_list[argument_f].type != tt_rbc; argument_f++) {
			struct defines* temp_var = &local_variables[local_variables_count];
			int name_len = strlen(temp_var->name);
			strncpy(temp_var->name, token_list[argument_f].word_begin, name_len);
			temp_var->value = eval(key_word_idx + 1);
			local_variables_count++;
		}
		int rr = eval(gloabl_function[program_call_define].index_body);
	}

	if (program_call_native != -1) {
		int rr = gloabl_function_native[program_call_native].func(key_word_idx);
	}
}



int eval(int key_word_idx) {
	const struct token key_word = token_list[key_word_idx];

	if (key_word.type == tt_rbo || key_word.type == tt_rbc) {
		key_word_idx += 1;
	}

	if (is_define_variable(key_word_idx)) {
		printf("new variable");
	}

	if (is_define_function(key_word_idx)) {
		printf("new function");
	}

	if (is_call_function(key_word_idx)) {
		printf("new function");
	}
	int number;
	if (is_get_variable(key_word_idx, &number)) {
		printf("new get variable");
		return number;
	}
}
void tokenizer(char* stream) {
	while (*stream) {
		if (isspace(*stream)) {
			stream++;
			continue;
		}
		
		const char* word = get_word(stream);
		const int word_lenght = word - stream;
		bool find_key_word_match = false;
		for (int key = 0; key < array_size(key_words); key++) {
			const struct token_type_pair key_word = key_words[key];
			const stream_offset = key_word.func_cmp(stream, word_lenght);

			if (stream_offset == 0)
				continue;

			add_token((struct token) { 
				.type = key_word.type,
				.word_begin = stream,
				.word_end = stream + stream_offset});

			stream = stream + stream_offset;
			find_key_word_match = true;
			break;
		}
		if (!find_key_word_match) {
			printf("Error identificator: %.*s", word_lenght, stream);
			break;
		}
		//printf("%c", *stream);
	}
	printf("End");
}

const char* program1 = "(define (dd x) (+ x x))";
//const char* program1 = "(10)";
const char* program2 = "(define aa 10)";
const char* program3 = "(dd 10)";

int main(int argc, char* args[]) {
	init_native_function();
	tokenizer(program1);
	eval(0);
	int offset1 = token_list_index;

	tokenizer(program2);
	eval(offset1);
	int offset2 = token_list_index;

	tokenizer(program3);
	eval(offset2);
	int offset3 = token_list_index;
}