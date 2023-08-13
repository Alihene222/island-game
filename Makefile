CC = clang++

INCFLAGS  = -iquotesrc
INCFLAGS += -Ilib/glfw/include
INCFLAGS += -Ilib/vulkan
INCFLAGS += -Ilib/vk_video
INCFLAGS += -Ilib/glm

CCFLAGS = -std=c++20 -Wall -Wpedantic -Wextra -g -Wno-unused-command-line-argument

LDFLAGS  = lib/glfw/build/src/libglfw3.a
LDFLAGS += -lvulkan

CCFLAGS += $(INCFLAGS)
CCFLAGS += $(LDFLAGS)

SRC = $(shell find src -name "*.cpp")
OBJ = $(SRC:.cpp=.o)
BIN = build

SHADERS = $(shell find res/shaders -name "*.vert" -o -name "*.frag")
SHADERS_OUT = $(foreach shader,$(SHADERS),$(shader).spirv)
GLSLC = glslc

.PHONY: all clean

all: dirs libs shaders build

libs:
	cd lib/glfw && cmake -S . -B build && make

dirs:
	mkdir -p ./$(BIN)

%.vert.spirv: %.vert
	$(GLSLC) $< -o $@

%.frag.spirv: %.frag
	$(GLSLC) $< -o $@

shaders: $(SHADERS_OUT)

run: build
	$(BIN)/game

build: dirs shaders $(OBJ)
	$(CC) -o $(BIN)/game $(filter %.o,$^) $(LDFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CCFLAGS)

clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)
	rm -rf $(SHADERS_OUT)
	rm -rf lib/glfw/CMakeCache.txt
