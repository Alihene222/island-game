CC = clang++

INCFLAGS  = -iquotesrc
INCFLAGS += -Ilib/glfw/include
INCFLAGS += -Ilib/vulkan
INCFLAGS += -Ilib/vk_video

CCFLAGS = -std=c++20 -Wall -Wpedantic -Wextra -g -Wno-unused-parameter

LDFLAGS  = lib/glfw/build/src/libglfw3.a
LDFLAGS += -lvulkan

CCFLAGS += $(INCFLAGS)
CCFLAGS += $(LDFLAGS)

SRC = $(shell find src -name "*.cpp")
OBJ = $(SRC:.cpp=.o)
BIN = build

.PHONY: all clean

all: dirs libs build

libs:
	cd lib/glfw && cmake -S . -B build && make

dirs:
	mkdir -p ./$(BIN)

run: build
	$(BIN)/game

build: dirs $(OBJ)
	$(CC) -o $(BIN)/game $(filter %.o,$^) $(LDFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CCFLAGS)

clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)
	rm -rf lib/glfw/CMakeCache.txt
