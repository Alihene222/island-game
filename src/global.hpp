#pragma once

#include "platform/platform.hpp"
#include "util/time.hpp"
#include "gfx/renderer.hpp"
#include "state.hpp"
#include "state_game.hpp"

struct Renderer;

struct Global {
    Platform *platform;
    Timer *timer;
    Renderer *renderer;

    // Current state
    State *state;

    // Possible states
    StateGame *game;
};

extern Global global;
