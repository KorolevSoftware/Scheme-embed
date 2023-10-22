#include "scheme/scheme.h"

const char* program1 = "(define (dd x) (+ x x))";
const char* program2 = "(define aa (+ 11 55))";
const char* program3 = "(print (dd (+ 11 55)))";
const char* program4 = "(define hello-world(lambda() \"Hello World\" (+ 10 11))";
const char* program5 = "(define ss 10)";
const char* program6 = "(print (hello-world))";
const char* program7 = "(define ss 10)";
const char* program8 = "(print ss)";
//const char* program9 = "(define ee 123) (print ee)";

const char* program9 = "(define ss 10) (define (ff) (begin (print ss) (print 22) (print (+ 6 6)) (10))) (print (ff) \"hello\")";

int main(int argc, char* args[]) {
	//struct token_node* programC1 = compile(program1);
	//struct token_node* programC3 = compile(program3);
	//struct token_node* programC4 = compile(program4);
	//struct token_node* programC6 = compile(program6);
	//struct token_node* programC5 = compile(program5);
	//struct token_node* programC7 = compile(program7);
	//struct token_node* programC8 = compile(program8);
	struct token_node* programC9 = compile(program9);

	//eval(programC1);
	//eval(programC3);
	//eval(programC4);
	//eval(programC6);
	//eval(programC5);
	//eval(programC7);
	//eval(programC8);
	eval(programC9);
}