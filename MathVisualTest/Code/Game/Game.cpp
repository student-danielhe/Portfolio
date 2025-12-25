#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <math.h>

void Game::StartUp() {
	 g_ScreenCamera = new Camera();
	 g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	 g_RNG = new RandomNumberGenerator();


}
void Game::Update(float deltaSecond) {
	HandleKeyPress();

}


void Game::Render() {

}



void Game::HandleKeyPress() {
	
}




Game::~Game() {

}

