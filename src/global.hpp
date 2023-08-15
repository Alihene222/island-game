#pragma once

#include "platform/platform.hpp"
#include "util/time.hpp"
#include "gfx/renderer.hpp"
#include "gfx/vk/vk_global.hpp"
#include "state.hpp"
#include "state_game.hpp"
#include "util/bump_allocator.hpp"

struct Renderer;

struct Global {
    bool debug;

    Platform *platform;
    Timer *timer;
    VKGlobal *vk_global;
    Renderer *renderer;

    // Current state
    State *state;

    // Per-frame allocations
    BumpAllocator frame_allocator;

    // Possible states
    StateGame *game;
};

extern Global global;
