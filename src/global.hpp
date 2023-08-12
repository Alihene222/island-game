#pragma once

#include "platform/platform.hpp"
#include "util/time.hpp"
#include "gfx/renderer.hpp"
#include "gfx/vk/vk_global.hpp"
#include "state.hpp"
#include "state_game.hpp"

struct Renderer;

struct Global {
    bool debug;

    Platform *platform;
    Timer *timer;
    VkGlobal *vk_global;
    Renderer *renderer;

    // Current state
    State *state;

    // Possible states
    StateGame *game;
};

extern Global global;
