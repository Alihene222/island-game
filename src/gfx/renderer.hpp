#pragma once

#include "gfx.hpp"
#include "vk/instance.hpp"

struct Renderer {
    std::unique_ptr<vkn::Instance> vk_instance;

    Renderer();

    ~Renderer();

    void render();
};
