#pragma once

#include "state.hpp"

struct StateGame : public State {
    StateGame();
    ~StateGame();

    void init() override;
    void tick() override;
    void update() override;
    void render() override;
    void destroy() override;
};
