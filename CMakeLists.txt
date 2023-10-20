cmake_minimum_required(VERSION 3.14)
project(Scheme-embed C)


# add_executable(${PROJECT_NAME} scheme.c)
add_executable(${PROJECT_NAME} main.c tokenizer.h tokenizer.c parser.h parser.c scheme.h scheme.c interpreter.h interpreter.c preprocessor.h preprocessor.c)
target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall
)