#include "state_game.hpp"

#include "global.hpp"

StateGame::StateGame() {
    this->init();
}

StateGame::~StateGame() {
    this->destroy();
}

void StateGame::init() {

}

void StateGame::tick() {

}

void StateGame::update() {

}

void StateGame::render() {
    global.renderer->render();
}

void StateGame::destroy() {

}
