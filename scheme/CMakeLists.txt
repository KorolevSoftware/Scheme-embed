cmake_minimum_required(VERSION 3.14)
project(Scheme-embed C)



set(SOURCES
  tokenizer.h
  tokenizer.c

  preprocessor.h 
  preprocessor.c

  interpreter.h
  interpreter.c

  scheme.h
  scheme.c
)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
  )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC scheme.h tokenizer.h)