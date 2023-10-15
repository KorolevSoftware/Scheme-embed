#pragma once
#include <stdbool.h>
#include <stdio.h>

enum object_type {
	ot_real,
	ot_integer,
	ot_fuction,
	ot_fuction_native,
	ot_string,
	ot_boolean,
	ot_define,
	ot_emply,
	ot_argument,
	ot_ident,
};


struct object {
	enum object_type type;
	char* name;
	struct object** next;
	union data {
		struct {
			float value;
		} real;

		struct {
			int value;
		} integer;

		struct {
			struct object** arguments_name;
			struct object* body;
			int arguments_count;
		} function; // scheme function code

		struct {
			struct object* (*func)(struct object**, int); // (arguments, count)
		} function_native;

		struct {
			char* value;
		} string;

		struct {
			bool value;
		} boolean;
	};
};

struct object* tokens_to_objects(struct token_node* nodes);