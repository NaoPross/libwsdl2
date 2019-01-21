# Compiler
CPP    := c++
CFLAGS := -Wall -pedantic -std=c++17 -fPIC -I ../include -DDEBUG
LFLAGS := ../build/libwrapsdl2.so -lSDL2 -lpthread

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,build/%.o,$(SRCS))

# Recipes
all: build/window_test

build/window_test: build/window_test.o
	$(CPP) $(CFLAGS) -o $@ $< $(LFLAGS)

build/%.o: %.cpp build
	$(CPP) $(CFLAGS) -o $@ -c $<

build:
	mkdir -p build
