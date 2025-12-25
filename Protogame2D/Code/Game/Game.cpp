#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <math.h>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"


void Game::StartUp() {
	loadSound();
	 g_ScreenCamera = new Camera();
	 g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(1600, 800));
	
	g_RNG = new RandomNumberGenerator();


}
void Game::Update(float deltaSecond) {
	UNUSED(deltaSecond);
	if (m_attractMode) {
		XboxController const& controller = g_theInput->GetController(GetConnectedControllerID());
		if (controller.IsConnected() && (controller.WasButtonReleased(XBOX_BUTTON_A)|| controller.WasButtonReleased(XBOX_BUTTON_START))) {
			ExitAttractMode();

		}
		else if (g_theInput->WasKeyJustReleased(' ')|| g_theInput->WasKeyJustReleased('N')) {
			if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
				ExitAttractMode();
			}
			

		}
		
	}
	else {
		//not in attract mode
		HandleKeyPress();

	}
	
}


void Game::Render() {
	if (m_attractMode) {
		g_theRenderer->BeginCamera(*g_ScreenCamera);
		RenderAttractMode();
		g_theRenderer->EndCamera(*g_ScreenCamera);
	}
	else {
	}
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(16, 8));
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	g_theDevConsole->Render(AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()), g_theRenderer);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void Game::DebugRender() {

}

void Game::RenderAttractMode() {
	
}

void Game::EnterAttractMode() {
	m_attractMode = true;
}
void Game::ExitAttractMode() {
	m_attractMode = false;
}
void Game::HandleKeyPress() {
}

void Game::loadSound() {
	

}


Game::~Game() {

}