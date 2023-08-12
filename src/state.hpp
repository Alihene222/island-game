#pragma once

#include "util/util.hpp"

struct State {
    virtual ~State() = default;

    State() = default;
    State(const State &other) = delete;
    State(State &&other) = default;
    State &operator=(const State &other) = delete;
    State &operator=(State &&other) = default;

    virtual void init() {}
    virtual void tick() {}
    virtual void update() {}
    virtual void render() {}
    virtual void destroy() {}
};
