CC = clang++

OS = $(shell uname)

INCFLAGS  = -iquotesrc
INCFLAGS += -Ilib/glfw/include
INCFLAGS += -Ilib/bgfx/include
INCFLAGS += -Ilib/bx/include
INCFLAGS += -Ilib/bimg/include

CCFLAGS = -std=c++20 -Wall -Wpedantic -Wextra -g -Wno-unused-parameter

# TODO: cross-platform compatibility
BGFX_BIN = lib/bgfx/.build/linux64_gcc/bin
BGFX_CONFIG = Debug
BGFX_TARGET = linux-debug64

LDFLAGS  = lib/glfw/build/src/libglfw3.a
LDFLAGS += $(BGFX_BIN)/libbgfx$(BGFX_CONFIG).a
LDFLAGS += $(BGFX_BIN)/libbx$(BGFX_CONFIG).a
LDFLAGS += $(BGFX_BIN)/libbimg$(BGFX_CONFIG).a

CCFLAGS += $(INCFLAGS)
CCFLAGS += $(LDFLAGS)

SRC = $(shell find src -name "*.cpp")
OBJ = $(SRC:.cpp=.o)
BIN = build

# TODO: cross-compatibility
SHADERS_PATH = res/shaders
SHADERS	= $(shell find $(SHADERS_PATH)/* -maxdepth 1 | grep -E ".*/(vs|fs).*.sc")
SHADERS_OUT = $(SHADERS:.sc=.bin)
SHADERC = lib/bgfx/.build/linux64_gcc/bin/shaderc$(BGFX_CONFIG)
SHADER_TARGET = spirv
SHADER_PLATFORM = linux

.PHONY: all clean

all: dirs libs shaders build

libs:
	cd lib/bgfx && make $(BGFX_TARGET)
	cd lib/bx && make $(BGFX_TARGET)
	cd lib/bimg && make $(BGFX_TARGET)
	cd lib/glfw && cmake -S . -B build && make

dirs:
	mkdir -p ./$(BIN)

run: shaders build
	$(BIN)/game

shaders: $(SHADERS_OUT)

%.bin: %.sc
	$(SHADERC) --type $(shell echo $(notdir $@) | cut -c 1)	\
		-i lib/bgfx/src \
		--platform $(SHADER_PLATFORM) \
		--varyingdef $(dir $@)varying.def.sc \
		-p $(SHADER_TARGET) \
		--verbose \
		-f $< \
		-o $@

build: dirs $(OBJ)
	$(CC) -o $(BIN)/game $(filter %.o,$^) $(LDFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CCFLAGS)

clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)
	rm -rf lib/glfw/CMakeCache.txt
