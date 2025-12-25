#include "Game.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <math.h>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/DebugProp.hpp"
extern std::vector<DebugProp*> m_debugRenderProps;
extern std::vector<DebugProp*> m_debugRenderScreenProps;
Clock* g_gameClock = nullptr;
void Game::StartUp() {
	Texture* fontSheet = g_theRenderer->CreateOrGetTextureFromFile("Data/Fonts/SquirrelFixedFont.png");
	m_font = new SpriteSheet(*fontSheet, IntVec2(16, 16));
	g_ScreenCamera = new Camera();
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(48, 24));
	
	g_RNG = new RandomNumberGenerator();

	m_player = new Player(this);
	g_thePlayerCamera = new Camera();
	g_thePlayerCamera->SetOrthoView(Vec2(-1, -1), Vec2(1, 1));
	MakeFrames();
	MakeSphere();
	MakeSphere2();
	m_cubeDepth = new Prop(this);
	m_cubeRotate = new Prop(this);
	*m_cubeDepth = MakeCube(1.f, 1.f, 1.f);
	m_cubeDepth->SetPosition(Vec3(-2.f,-2.f,0.f));
	*m_cubeRotate = MakeCube(1.f, 1.f, 1.f);
	m_cubeRotate->SetPosition(Vec3(2.f, 2.f, 0.f));
	m_cubeRotate->SetAngularVelocity(EulerAngles(30.f,30.f,30.f));
	g_gameClock = new Clock();
	
	DebugRenderConfig config;
	config.m_renderer = g_theRenderer;
	DebugRenderSystemStartUp(config);
	SubscribeEventCallbackFunction("DEBUG CLEAR", Command_DebugRenderClear);
	SubscribeEventCallbackFunction("DEBUG TOGGLE", Command_DebugRenderToggle);
	EnterAttractMode();
	
	DebugAddWorldBasis(Mat44(), -1.f);
	Mat44 xtrans(
		Vec3(0.f, 0.f, 1.f),
		Vec3(-1.f, 0.f, 0.f),
		Vec3(0.f, 1.f, 0.f),
		Vec3(1.f, -1.f, .5f)
	);
	DebugAddWorldText("X Axis", xtrans, .5f, Vec2(1.f, 0.f), -1.f, Rgba8(255, 0, 0));
	Mat44 yTrans(
		Vec3(0.f, 0.f, 1.f),
		Vec3(0.f, 1.f, 0.f),
		Vec3(1.f, 0.f, 0.f),
		Vec3(-1.f, -2.f, .5f)
	);
	DebugAddWorldText("Y Axis", yTrans, .5f, Vec2(0.f, 0.f), -1.f, Rgba8(0, 255, 0));
	Mat44 zTrans(
		Vec3(-.5f, .5f, 0.f),
		Vec3(0.f, 0.f, -1.f),
		Vec3(.5f, .5f, 0.f),
		Vec3(-.5f, -.5f, .5f)
	);
	DebugAddWorldText("Z Axis", zTrans, .5f, Vec2(1.f, 0.f), -1.f, Rgba8(0, 0, 255));
	
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
		m_player->Update(deltaSecond);
		float gameDeltaSecond = (float)g_gameClock->GetDeltaSeconds();
		UpdateCubes(gameDeltaSecond);
		m_cubeRotate->Update(gameDeltaSecond);
		m_cubeDepth->Update(gameDeltaSecond);
		m_sphere->Update(gameDeltaSecond);
		m_sphere2->Update(gameDeltaSecond);
		for (Prop frame : m_frame) {
			frame.Update(gameDeltaSecond);
		}
		/*int i = 0;
		for (DebugProp* prop : m_debugRenderProps) {
			prop->Update(gameDeltaSecond);
			prop->m_currentDuration += gameDeltaSecond;
			if (prop->m_duration != -1.f) {
				if (prop->m_currentDuration >= prop->m_duration) {
					m_debugRenderProps.erase(m_debugRenderProps.begin()+i);
				}
			}
			i++;
		}*/

		for (int i = 0; i < m_debugRenderProps.size(); i++) {
			DebugProp* prop = m_debugRenderProps[i];
			prop->Update(gameDeltaSecond);
			prop->m_currentDuration += gameDeltaSecond;
			if (prop->m_duration != -1.f) {
				if (prop->m_currentDuration >= prop->m_duration) {
					m_debugRenderProps.erase(m_debugRenderProps.begin() + i);
				}
			}
		}
		
		for (int i = 0; i < m_debugRenderScreenProps.size();i++) {
			DebugProp* prop = m_debugRenderScreenProps[i];
			prop->Update(gameDeltaSecond);
			prop->m_currentDuration += gameDeltaSecond;
			if (prop->m_duration != -1.f) {
				if (prop->m_currentDuration >= prop->m_duration) {
					m_debugRenderScreenProps.erase(m_debugRenderScreenProps.begin() + i);
				}
			}
		}
	}

	std::string positionText;
	positionText = "Current Position: x: " + std::to_string(m_player->GetPosition().x) + " /n y: " + std::to_string(m_player->GetPosition().y) + " /n z: " + std::to_string(m_player->GetPosition().z);
	std::string timeText;
	timeText = "FPS: " + std::to_string(g_gameClock->GetDeltaSeconds()) + "/nTotal Sec: " + std::to_string(g_gameClock->GetTotalSeconds());
	DebugAddScreenText(positionText, AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()), .5f, Vec2(1.f, .9f), 0.f);

	DebugAddScreenText(timeText, AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()), .5f, Vec2(0.f, .9f), 0.f);

}


void Game::Render() {
	if (m_attractMode) {
		//clear screen
		Rgba8 darkOrange = Rgba8(50, 50, 50);
		g_theRenderer->ClearScreen(darkOrange);
		
		RenderAttractMode();
		
	}
	else {
		//render game mode
		//clear screen

		Rgba8 darkOrange = Rgba8(100, 100, 100);
		g_theRenderer->ClearScreen(darkOrange);
		g_thePlayerCamera->SetPerspectiveView(2.f, 60.f, .1f, 100.f);
		Mat44 CamToRender(Vec4(0.f, -1.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 1.f, 0.f),
			Vec4(1.f, 0.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 0.f, 1.f));
		CamToRender.Transpose();
		g_thePlayerCamera->SetCameraToRenderTransform(CamToRender);
		g_theRenderer->BeginCamera(*g_thePlayerCamera);

		m_cubeDepth->Render();

		m_cubeRotate->Render();
		
		for (Prop frame : m_frame) {
			frame.Render();
		}
		m_sphere2->Render();
		m_sphere->Render();
		DebugRenderWorld(*g_thePlayerCamera);
		g_theRenderer->EndCamera(*g_thePlayerCamera);
		
	}
	//g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(48, 24));
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theDevConsole->Render(AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()), g_theRenderer);
	DebugRenderScreen(*g_ScreenCamera);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void Game::DebugRender() {

}

void Game::RenderAttractMode() {
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	std::vector<Vertex_PCU> verts;
	AddVertsForRect(verts, Vec2(4, 4), Vec2(12, 4), Vec2(12, 12), Vec2(4, 12), Rgba8(255, 255, 255),AABB2());
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void Game::EnterAttractMode() {
	m_attractMode = true;
	DebugRenderSetHidden();
}
void Game::ExitAttractMode() {
	m_attractMode = false;
	DebugRenderSetVisible();
}
void Game::HandleKeyPress() {
	if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
		if (g_theInput->WasKeyJustReleased('P')) {
			g_gameClock->TogglePause();
		}

		if (g_theInput->WasKeyJustReleased('O')) {
			g_gameClock->StepSingleFrame();
		}

		if (g_theInput->WasKeyJustPressed('T')) {
			g_gameClock->SetTimeScale(.1f);
		}
		if (g_theInput->WasKeyJustReleased('T'))
		{
			g_gameClock->SetTimeScale(1.f);
		}
		if (g_theInput->WasKeyJustReleased('1'))
		{
			DebugAddWorldCylinder(m_player->GetPosition()+m_player->GetFwdVector()*20.f, m_player->GetPosition() - m_player->GetFwdVector() * 20.f,.0625f, 10.f, 
				Rgba8(255, 255, 0), Rgba8(255, 255, 0), DebugRenderMode::X_RAY);
		}

		if (g_theInput->WasKeyJustReleased('2'))
		{
			DebugAddWorldSphere(Vec3(m_player->GetPosition().x,m_player->GetPosition().y,0.f), 1.f, 60.f, Rgba8(150, 75, 0));
		}

		if (g_theInput->WasKeyJustReleased('3'))
		{
			DebugAddWorldWireSphere(m_player->GetPosition() + m_player->GetFwdVector()*2.f, 1.f, 5.f, Rgba8(0,255,0),Rgba8(255,0,0));
		}
		if (g_theInput->WasKeyJustReleased('4'))
		{
			DebugAddBasis(m_player->GetTransform(), .5f, 5.f, 20.f);
		}
		if (g_theInput->WasKeyJustReleased('5'))
		{
			std::string positionText;
			positionText = "Current Position: x: " + std::to_string(m_player->GetPosition().x) + " y: " + std::to_string(m_player->GetPosition().y) + " z: " + std::to_string(m_player->GetPosition().z);

			//DebugAddWorldText("Test Test Test", Mat44::MakeFullOpposing(m_player->GetTransform()), .5f, Vec2(.5f, .5f), -1.f, Rgba8(0, 0, 255));
			DebugAddWorldBillboardText(positionText, m_player->GetPosition(), .125f, Vec2(1.f, 1.f), 10.f, Rgba8(255, 255, 255), Rgba8(255, 0, 0));
		}
		if (g_theInput->WasKeyJustReleased('6'))
		{
			DebugAddWorldWireCylinder(m_player->GetPosition() , 
				m_player->GetPosition()+ Vec3(.000001f, .000001f, 1.f),
				.5f, 10.f,Rgba8(255,255,255),Rgba8(255,0,0));

		}
		if (g_theInput->WasKeyJustReleased('7'))
		{

			std::string Text;
			Text = "Current Orientation: /nyaw: " + std::to_string(m_player->GetOrientation().x) + 
				" /npitch: " + std::to_string(m_player->GetOrientation().y) + 
				" /nroll: " + std::to_string(m_player->GetOrientation().z);

			DebugAddScreenText(Text, AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()),3.f,Vec2(.5f,.5f),5.f);
		}
	}
	
}

void Game::loadSound() {

}


Game::~Game() {
	DebugRenderSystemShutdown();
}

void Game::RenderCube() {
	g_theRenderer->BeginCamera(*g_thePlayerCamera);
	std::vector<Vertex_PCU> verts;
	//+x
	AddVertsForQuad3D(verts, Vec3(-.5f, -.5f, -.5f), Vec3(-.5f, .5f, -.5f), Vec3(-.5f, .5f, .5f), Vec3(-.5f, -.5f, .5f), Rgba8(255, 0, 0));
	//-x
	AddVertsForQuad3D(verts, Vec3(.5f, .5f, -.5f), Vec3(.5f, -.5f, -.5f), Vec3(.5f, -.5f, .5f), Vec3(.5f, .5f, .5f), Rgba8(0, 255, 255));
	//+y
	AddVertsForQuad3D(verts, Vec3(.5f, -.5f, -.5f), Vec3(-.5f, -.5f, -.5f), Vec3(-.5f, -.5f, .5f), Vec3(.5f, -.5f, .5f), Rgba8(0, 255, 0));
	//-y
	AddVertsForQuad3D(verts, Vec3(-.5f, .5f, -.5f), Vec3(.5f, .5f, -.5f), Vec3(.5f, .5f, .5f), Vec3(-.5f, .5f, .5f), Rgba8(255, 0, 255));
	//+z
	AddVertsForQuad3D(verts, Vec3(-.5f, .5f, -.5f), Vec3(-.5f, -.5f, -.5f), Vec3(.5f, -.5f, -.5f), Vec3(.5f, .5f, -.5f), Rgba8(0, 0, 255));
	//-z
	AddVertsForQuad3D(verts, Vec3(.5f, .5f, .5f), Vec3(.5f, -.5f, .5f), Vec3(-.5f, -.5f, .5f), Vec3(-.5f, .5f, .5f), Rgba8(255, 255, 0));
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->EndCamera(*g_thePlayerCamera);
}

Prop Game::MakeCube(float xLen, float yLen, float zLen, Rgba8 color) {
	Prop cube(this);
	if (color == Rgba8(1, 2, 3)) {
		std::vector<Vertex_PCU> verts;
		AddVertsForQuad3D(verts, Vec3(-.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(-.5f * xLen, .5f * yLen, -.5f * zLen), Vec3(-.5f * xLen, .5f * yLen, .5f * zLen), Vec3(-.5f * xLen, -.5f * yLen, .5f * zLen), Rgba8(255, 0, 0));
		AddVertsForQuad3D(verts, Vec3(.5f * xLen, .5f * yLen, -.5f * zLen), Vec3(.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(.5f * xLen, -.5f * yLen, .5f * zLen), Vec3(.5f * xLen, .5f * yLen, .5f * zLen), Rgba8(0, 255, 255));
		AddVertsForQuad3D(verts, Vec3(.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(-.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(-.5f * xLen, -.5f * yLen, .5f * zLen), Vec3(.5f * xLen, -.5f * yLen, .5f * zLen), Rgba8(0, 255, 0));
		AddVertsForQuad3D(verts, Vec3(-.5f * xLen, .5f * yLen, -.5f * zLen), Vec3(.5f * xLen, .5f * yLen, -.5f * zLen), Vec3(.5f * xLen, .5f * yLen, .5f * zLen), Vec3(-.5f * xLen, .5f * yLen, .5f * zLen), Rgba8(255, 0, 255));
		AddVertsForQuad3D(verts, Vec3(-.5f * xLen, .5f * yLen, -.5f * zLen), Vec3(-.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(.5f * xLen, .5f * yLen, -.5f * zLen), Rgba8(0, 0, 255));
		AddVertsForQuad3D(verts, Vec3(.5f * xLen, .5f * yLen, .5f * zLen), Vec3(.5f * xLen, -.5f * yLen, .5f * zLen), Vec3(-.5f * xLen, -.5f * yLen, .5f * zLen), Vec3(-.5f * xLen, .5f * yLen, .5f * zLen), Rgba8(255, 255, 0));
		cube.SetVerts(verts);
	}
	else {
		std::vector<Vertex_PCU> verts;
		AddVertsForQuad3D(verts, Vec3(-.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(-.5f * xLen, .5f * yLen, -.5f * zLen), Vec3(-.5f * xLen, .5f * yLen, .5f * zLen), Vec3(-.5f * xLen, -.5f * yLen, .5f * zLen), color);
		AddVertsForQuad3D(verts, Vec3(.5f * xLen, .5f * yLen, -.5f * zLen), Vec3(.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(.5f * xLen, -.5f * yLen, .5f * zLen), Vec3(.5f * xLen, .5f * yLen, .5f * zLen), color);
		AddVertsForQuad3D(verts, Vec3(.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(-.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(-.5f * xLen, -.5f * yLen, .5f * zLen), Vec3(.5f * xLen, -.5f * yLen, .5f * zLen), color);
		AddVertsForQuad3D(verts, Vec3(-.5f * xLen, .5f * yLen, -.5f * zLen), Vec3(.5f * xLen, .5f * yLen, -.5f * zLen), Vec3(.5f * xLen, .5f * yLen, .5f * zLen), Vec3(-.5f * xLen, .5f * yLen, .5f * zLen), color);
		AddVertsForQuad3D(verts, Vec3(-.5f * xLen, .5f * yLen, -.5f * zLen), Vec3(-.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(.5f * xLen, -.5f * yLen, -.5f * zLen), Vec3(.5f * xLen, .5f * yLen, -.5f * zLen), color);
		AddVertsForQuad3D(verts, Vec3(.5f * xLen, .5f * yLen, .5f * zLen), Vec3(.5f * xLen, -.5f * yLen, .5f * zLen), Vec3(-.5f * xLen, -.5f * yLen, .5f * zLen), Vec3(-.5f * xLen, .5f * yLen, .5f * zLen), color);
		cube.SetVerts(verts);
	}
	
	return cube;
}

void Game::MakeFrames() {
	for (int i = -50; i <= 50; i++) {
		
		if (i % 5 == 0) {
			float thickness = 0.1f;
			if (i == 0) {
				thickness = 0.4f;
			}
			Prop horizontal = MakeCube(thickness, 100.f, thickness, Rgba8(0, 255, 0));
			horizontal.SetPosition(Vec3((float)i, 0.f, 0.f));
			m_frame.push_back(horizontal);

			Prop veritcal = MakeCube(100.f, thickness, thickness, Rgba8(255, 0, 0));
			veritcal.SetPosition(Vec3(0.f, (float)i, 0.f));
			m_frame.push_back(veritcal);
		}
		else {
			Prop horizontal = MakeCube(.05f, 100.f, .05f, Rgba8(127, 127, 127));
			horizontal.SetPosition(Vec3((float)i, 0.f, 0.f));
			m_frame.push_back(horizontal);
			Prop veritcal = MakeCube(100.f, .05f, .05f, Rgba8(127, 127, 127));
			veritcal.SetPosition(Vec3(0.f, (float)i, 0.f));
			m_frame.push_back(veritcal);
		}
		
	}

}
void Game::MakeSphere() {
	std::vector<Vertex_PCU> verts;
	for (int sliceNum = 0; sliceNum < 32; sliceNum++) {
		float startDeg = (float)sliceNum * 360.f / 32.f;
		float endDeg = (float)(sliceNum + 1) * 360.f / 32.f;
		float degPerStack = 180.f / 16.f;
		Rgba8 black = Rgba8(0, 0, 0);
		for (int stackNum = 0; stackNum < 16; stackNum++) {
			char c = '0' + (stackNum + sliceNum) % 8;
			int charIndex = c;
			AABB2 UV;
			m_font->GetSpriteUVs(UV.m_mins, UV.m_maxs, charIndex);
			if (stackNum == 0) {
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg,stackNum*degPerStack-90.f, 1.01f),black, Vec2(UV.GetCenter().x, UV.m_maxs.y)));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(endDeg, (stackNum + 1) * degPerStack - 90.f, 1.01f), black, Vec2(UV.m_maxs.x, UV.m_mins.y)));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg, (stackNum+1) * degPerStack-90.f, 1.01f), black, UV.m_mins));
				
			}
			else if (stackNum == 15) {
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg, stackNum * degPerStack-90.f, 1.01f), black, Vec2(UV.m_mins.x,UV.m_maxs.y)));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(endDeg, stackNum * degPerStack-90.f, 1.01f), black, UV.m_maxs));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg, (stackNum + 1) * degPerStack - 90.f, 1.01f), black, Vec2(UV.GetCenter().x, UV.m_mins.y)));
			}
			else {
				AddVertsForQuad3D(verts,
					Vec3::MakeFromPolarDegree(startDeg, stackNum * degPerStack-90.f, 1.01f),
					Vec3::MakeFromPolarDegree(endDeg, stackNum * degPerStack-90.f, 1.01f),
					Vec3::MakeFromPolarDegree(endDeg, (stackNum + 1) * degPerStack-90.f, 1.01f),
					Vec3::MakeFromPolarDegree(startDeg, (stackNum + 1) * degPerStack-90.f, 1.01f),
					black,UV);
			}
		}
	}
	m_sphere = new Prop(this);
	m_sphere->SetVerts(verts);
	m_sphere->SetAngularVelocity(EulerAngles(30.f,30.f,30.f));
	m_sphere->SetPosition(Vec3(10.f,-5.f,1.f));
	m_sphere->SetTexture(&m_font->GetTexture());
}
void Game::MakeSphere2() {
	std::vector<Vertex_PCU> verts;
	for (int sliceNum = 0; sliceNum < 32; sliceNum++) {
		float startDeg = (float)sliceNum * 360.f / 32.f;
		float endDeg = (float)(sliceNum + 1) * 360.f / 32.f;
		float degPerStack = 180.f / 16.f;
		std::vector<Rgba8> colors;
		colors.push_back(Rgba8(0, 255, 0));
		colors.push_back(Rgba8(255, 255, 0));
		colors.push_back(Rgba8(0, 0, 0));
		colors.push_back(Rgba8(255, 0, 0));
		colors.push_back(Rgba8(0, 0, 255));
		colors.push_back(Rgba8(255, 0, 255));
		colors.push_back(Rgba8(0, 255, 255));
		colors.push_back(Rgba8(255, 255, 255));
		for (int stackNum = 0; stackNum < 16; stackNum++) {
			if (stackNum == 0) {
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg, stackNum * degPerStack - 90.f), colors[(stackNum + sliceNum) % 8]));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(endDeg, (stackNum + 1) * degPerStack - 90.f), colors[(stackNum + sliceNum) % 8]));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg, (stackNum + 1) * degPerStack - 90.f), colors[(stackNum + sliceNum) % 8]));

			}
			else if (stackNum == 15) {
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg, stackNum * degPerStack - 90.f), colors[(stackNum + sliceNum) % 8]));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(endDeg, stackNum * degPerStack - 90.f), colors[(stackNum + sliceNum) % 8]));
				verts.push_back(Vertex_PCU(Vec3::MakeFromPolarDegree(startDeg, (stackNum + 1) * degPerStack - 90.f), colors[(stackNum + sliceNum) % 8]));

			}
			else {
				AddVertsForQuad3D(verts,
					Vec3::MakeFromPolarDegree(startDeg, stackNum * degPerStack - 90.f),
					Vec3::MakeFromPolarDegree(endDeg, stackNum * degPerStack - 90.f),
					Vec3::MakeFromPolarDegree(endDeg, (stackNum + 1) * degPerStack - 90.f),
					Vec3::MakeFromPolarDegree(startDeg, (stackNum + 1) * degPerStack - 90.f),
					colors[(stackNum + sliceNum) % 8]);
			}
		}
	}
	m_sphere2 = new Prop(this);
	m_sphere2->SetVerts(verts);
	m_sphere2->SetAngularVelocity(EulerAngles(30.f, 30.f, 30.f));
	m_sphere2->SetPosition(Vec3(10.f, -5.f, 1.f));
}

void Game::UpdateCubes(float deltaSeconds) {
	//change color
	if (m_darkening) {
		m_cube2Color-=50.f*deltaSeconds;
		if (m_cube2Color <= 0.f) {
			m_cube2Color = 0.f;
			m_darkening = false;
		}
	}
	else {
		m_cube2Color+=50.f*deltaSeconds;
		if (m_cube2Color >= 255.f) {
			m_cube2Color = 255.f;
			m_darkening = true;
		}
	}
	
	m_cubeDepth->SetColor(Rgba8((unsigned char)RoundDownToInt(m_cube2Color), (unsigned char)RoundDownToInt(m_cube2Color), (unsigned char)RoundDownToInt(m_cube2Color)));
}

void Game::BeginFrame() {
	DebugRenderBeginFrame();
}
void Game::EndFrame() {
	DebugRenderEndFrame();
}