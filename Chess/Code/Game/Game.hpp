#pragma once
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Player.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/ObjUtils.hpp"
#include "ChessPlayer.hpp"
//#include "Engine/Input/InputSystem.hpp"
class Game {
public:

	RandomNumberGenerator* g_RNG = nullptr;
	bool m_attractMode = true;
	Camera* g_ScreenCamera = nullptr;
	std::vector<LightSource> m_sources;
	
	std::string m_playerName = "Player1";
	std::string m_enemyName = "Player2";
	bool m_offerDraw=false;

	ChessPlayer* m_blackPlayer = nullptr;
	ChessPlayer* m_whitePlayer = nullptr;
	bool m_drafting = true;
	bool m_whiteTurn = false;
public:
	void StartUp();
	void InitializeEvents();
	void Update(float deltaSecond);
	void Render();
	void RenderDrafting();
	void BeginFrame();
	void EndFrame();
	Game(){}
	~Game();
	void DebugRender();
	void RenderAttractMode();
	void EnterAttractMode();
	void ExitAttractMode();
	void HandleKeyPress();
	void LoadSound();

	void SendNetworkCommand(std::string const& commandLine);
	void ReceiveNetworkCommand();
	std::string GetGameState();

	static bool HandleChessMove(EventArgs& args);
	static bool HandlePrintBoardState(EventArgs& args);
	static bool HandleChessServerInfo(EventArgs& args);
	static bool HandleChessListen(EventArgs& args);
	static bool HandleChessConnect(EventArgs& args);
	static bool HandleChessDisconnect(EventArgs& args);
	static bool HandleChessPlayerInfo(EventArgs& args);
	static bool HandleChessBegin(EventArgs& args);
	static bool HandleChessValidate(EventArgs& args);
	static bool HandleChessResign(EventArgs& args);
	static bool HandleChessOfferDraw(EventArgs& args);
	static bool HandleChessAcceptDraw(EventArgs& args);
	static bool HandleChessRejectDraw(EventArgs& args);
	static bool HandleRemoteCommand(EventArgs& args);

	static SoundID g_Vesus;
	static SoundID g_Pawn;
	static SoundID g_Pawn2;
	static SoundID g_Rook;
	static SoundID g_Knight;
	static SoundID g_Bishop;
	static SoundID g_Queen;
	static SoundID g_King;
	static SoundID g_Menu;
	static SoundID g_Match;
	static SoundPlaybackID g_MenuPlayback;
	static SoundPlaybackID g_MatchPlayback;

private:
	bool m_IsDebugging=false;
};