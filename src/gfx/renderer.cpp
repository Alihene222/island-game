#include "renderer.hpp"

#include "global.hpp"

Renderer::Renderer() {
    this->vk_instance = std::make_unique<vkn::Instance>();
}

Renderer::~Renderer() {

}

void Renderer::render() {

}
