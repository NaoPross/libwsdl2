# Compiler
CPP    := c++
CFLAGS := -Wall -pedantic -std=c++17 -fPIC -shared -I include -DDEBUG -DWRAPSDL2_EXCEPTIONS
LFLAGS := -lSDL2

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,build/%.o,$(SRCS))

# Recipes
all: build/libwrapsdl2.so
	$(MAKE) -C test

.PHONY: test-all
.ONESHELL:
test-all:
	cd test
	./build/window_test
    

build/libwrapsdl2.so: $(OBJS)
	$(CPP) $(CFLAGS) -o $@ $(LFLAGS) $(OBJS)

build/%.o: %.cpp build
	$(CPP) $(CFLAGS) -o $@ -c $<

build:
	mkdir -p build
