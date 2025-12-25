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
#include "ChessPiece.hpp"
#include "ChessMatch.hpp"
#include "ChessBoard.hpp"
#include "App.hpp"
#include "Engine/Network/NetworkSystem.hpp"

StaticModel* m_woman = nullptr;
BitmapFont* g_font = nullptr;
Clock* g_gameClock = nullptr;
Player* g_player = nullptr;

SoundID Game::g_Vesus  = MISSING_SOUND_ID;
SoundID Game::g_Pawn   = MISSING_SOUND_ID;
SoundID Game::g_Pawn2  = MISSING_SOUND_ID;
SoundID Game::g_Rook   = MISSING_SOUND_ID;
SoundID Game::g_Knight = MISSING_SOUND_ID;
SoundID Game::g_Bishop = MISSING_SOUND_ID;
SoundID Game::g_Queen  = MISSING_SOUND_ID;
SoundID Game::g_King   = MISSING_SOUND_ID;
SoundID Game::g_Menu   = MISSING_SOUND_ID;
SoundID Game::g_Match  = MISSING_SOUND_ID;
SoundPlaybackID Game::g_MenuPlayback = MISSING_SOUND_ID;
SoundPlaybackID Game::g_MatchPlayback = MISSING_SOUND_ID;

extern std::vector<DebugProp*> m_debugRenderProps;
extern std::vector<DebugProp*> m_debugRenderScreenProps;
void Game::StartUp() {
	LoadSound();
	g_font =g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	g_ScreenCamera = new Camera();
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	
	g_RNG = new RandomNumberGenerator();

	g_player = new Player(this);
	g_thePlayerCamera = new Camera();
	g_thePlayerCamera->SetOrthoView(Vec2(-1, -1), Vec2(1, 1));
	g_gameClock = new Clock();

	g_theRenderer->SetSampleMode(SamplerMode::BILINEAR_WRAP, 0);
	g_theRenderer->SetSampleMode(SamplerMode::BILINEAR_WRAP, 1);
	g_theRenderer->SetSampleMode(SamplerMode::BILINEAR_WRAP, 2);

	DebugRenderConfig config;
	config.m_renderer = g_theRenderer;
	DebugRenderSystemStartUp(config);
	SubscribeEventCallbackFunction("DEBUG CLEAR", Command_DebugRenderClear);
	SubscribeEventCallbackFunction("DEBUG TOGGLE", Command_DebugRenderToggle);
	EnterAttractMode();
	
	//m_woman = g_theRenderer->CreateStaticModel("Data/Obj/Woman.xml");

	LightSource source = g_theRenderer->CreateSpotLight(10.f, 4.f, 6.f, Vec3(0.f, 0.f, 5.f), Rgba8::GREEN, Vec3(1, 1, -1).GetNormalized(), 90.f, 180.f);
	LightSource source2 = g_theRenderer->CreateSpotLight(10.f, 2.f, 3.f, Vec3(5.f, 5.f, .5f), Rgba8::RED);

	//m_sources.push_back(source);
	//m_sources.push_back(source2);
	g_theRenderer->SetLightConstants(Vec3(1, 1, -1).GetNormalized(), 2.f, .5f, Rgba8::WHITE, m_sources);
	InitializeEvents();

	m_whitePlayer = new ChessPlayer(Faction::WHITE);
	m_blackPlayer = new ChessPlayer(Faction::BLACK);

}

void Game::InitializeEvents()
{
	SubscribeEventCallbackFunction("PRINTBOARDSTATE", Game::HandlePrintBoardState);
	SubscribeEventCallbackFunction("CHESSMOVE", Game::HandleChessMove);
	SubscribeEventCallbackFunction("CHESSSERVERINFO", Game::HandleChessServerInfo);
	SubscribeEventCallbackFunction("CHESSLISTEN", Game::HandleChessListen);
	SubscribeEventCallbackFunction("CHESSCONNECT", Game::HandleChessConnect);
	SubscribeEventCallbackFunction("CHESSDISCONNECT", Game::HandleChessDisconnect);
	SubscribeEventCallbackFunction("CHESSPLAYERINFO", Game::HandleChessPlayerInfo);
	SubscribeEventCallbackFunction("CHESSBEGIN", Game::HandleChessBegin);
	SubscribeEventCallbackFunction("CHESSVALIDATE", Game::HandleChessValidate);
	SubscribeEventCallbackFunction("CHESSRESIGN", Game::HandleChessResign);
	SubscribeEventCallbackFunction("CHESSOFFERDRAW", Game::HandleChessOfferDraw);
	SubscribeEventCallbackFunction("CHESSACCEPTDRAW", Game::HandleChessAcceptDraw);
	SubscribeEventCallbackFunction("CHESSREJECTDRAW", Game::HandleChessRejectDraw);
	SubscribeEventCallbackFunction("REMOTECMD", Game::HandleRemoteCommand);
}

void Game::Update(float deltaSecond) {
	UNUSED(deltaSecond);
	if (m_attractMode) {
		HandleKeyPress();
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
		float gameDeltaSecond = (float)g_gameClock->GetDeltaSeconds();
		g_theRenderer->SetPerFrameConstants((float)g_gameClock->GetTotalSeconds());
		if(g_theMatch->m_freeCamera)
		    g_player->Update(deltaSecond);
		g_theMatch->Update(gameDeltaSecond);
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

	ReceiveNetworkCommand();
}


void Game::Render() {
	if (m_attractMode) {
		//clear screen
		Rgba8 darkOrange = Rgba8(255, 0, 255);
		g_theRenderer->ClearScreen(darkOrange);
		if(m_drafting)
		    RenderDrafting();
		else
		    RenderAttractMode();
		
	}
	else {
		//render game mode
		//clear screen
		g_theRenderer->BindShader(g_theRenderer->m_diffuseShader);
		Rgba8 darkOrange = Rgba8(255, 0, 255);
		g_theRenderer->ClearScreen(darkOrange);
		
		g_theMatch->m_board->Render();


		
		//DebugRender();

	}
	//g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(48, 24));
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->BindTexture(nullptr,1);
	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theDevConsole->Render(AABB2(g_ScreenCamera->GetOrthoBottomLeft(), g_ScreenCamera->GetOrthoTopRight()), g_theRenderer);
	DebugRenderScreen(*g_ScreenCamera);
	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void Game::RenderDrafting()
{
    g_theRenderer->BeginCamera(*g_ScreenCamera);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->SetSampleMode(SamplerMode::POINT_CLAMP);
	g_theRenderer->BindTexture(&g_font->GetTexture());
	std::vector<Vertex_PCU> title;
	g_font->AddVertsForText2D(title, Vec2(WORLD_SIZE_X/2-120,170), 20.f, "Chess Sliver");
	g_font->AddVertsForText2D(title, Vec2(10,140), 10.f, "Black");
	g_font->AddVertsForText2D(title, Vec2(340, 140), 10.f, "White");
	std::string instruction;
	if (m_whiteTurn) {
		instruction="White's Turn to pick.";
	}
	else {
		instruction = "Black's turn to pick.";
	}
	instruction +="Press numbers to pick a upgrade";
	g_font->AddVertsForText2D(title, Vec2(WORLD_SIZE_X / 2 - 120, 160.f), 4.f, instruction);
	g_font->AddVertsForText2D(title, Vec2(WORLD_SIZE_X / 2 - 95, 130.f), 6.f, "Pawn: Auto Promote on Take Down");
	g_font->AddVertsForText2D(title, Vec2(WORLD_SIZE_X / 2 - 100, 110.f), 6.f, "Rook: Unstoppable Vertical Movement");
	g_font->AddVertsForText2D(title, Vec2(WORLD_SIZE_X / 2 - 100, 90.f), 6.f,  "Knight: Is Queen for First Move");
	g_font->AddVertsForText2D(title, Vec2(WORLD_SIZE_X / 2 - 50, 70.f), 6.f,  "Bishop: Bouncy");
	g_font->AddVertsForText2D(title, Vec2(WORLD_SIZE_X / 2 - 50, 50.f), 6.f,  "Queen: Polygamy");
	g_font->AddVertsForText2D(title, Vec2(WORLD_SIZE_X / 2 - 70, 30.f), 6.f,  "King: King of the Hill");
	g_font->AddVertsForText2D(title, Vec2(WORLD_SIZE_X / 2 - 120, 3.f), 6.f,  "Space to start, G to switch mode");
	
	g_theRenderer->DrawVertexArray(title);
	
	g_theRenderer->BindTexture(nullptr);
	std::vector<Vertex_PCU> circles;
	Rgba8 color;
	if (m_blackPlayer->m_upgrades[0]) 
		color = Rgba8::GREEN;
	else 
		color = Rgba8::GRAY;
	
	AddVertsForDisc(circles, Vec2(70.f,135.f), 6.f, color, Vec2());

	if (m_blackPlayer->m_upgrades[1])
		color = Rgba8::GREEN;
	else
		color = Rgba8::GRAY;
	AddVertsForDisc(circles, Vec2(70.f,115.f), 6.f, color, Vec2());

	if (m_blackPlayer->m_upgrades[2])
		color = Rgba8::GREEN;
	else
		color = Rgba8::GRAY;
	AddVertsForDisc(circles, Vec2(70.f, 95.f), 6.f, color, Vec2());

	if (m_blackPlayer->m_upgrades[3])
		color = Rgba8::GREEN;
	else
		color = Rgba8::GRAY;
	AddVertsForDisc(circles, Vec2(70.f, 75.f), 6.f, color, Vec2());

	if (m_blackPlayer->m_upgrades[4])
		color = Rgba8::GREEN;
	else
		color = Rgba8::GRAY;
	AddVertsForDisc(circles, Vec2(70.f, 55.f), 6.f, color, Vec2());

	if (m_blackPlayer->m_upgrades[5])
		color = Rgba8::GREEN;
	else
		color = Rgba8::GRAY;
	AddVertsForDisc(circles, Vec2(70.f, 35.f), 6.f, color, Vec2());


	if (m_whitePlayer->m_upgrades[0])
		color = Rgba8::GREEN;
	else
		color = Rgba8::GRAY;
	AddVertsForDisc(circles, Vec2(330.f,135.f), 6.f, color, Vec2());

	if (m_whitePlayer->m_upgrades[1])
		color = Rgba8::GREEN;
	else
		color = Rgba8::GRAY;
	AddVertsForDisc(circles, Vec2(330.f,115.f), 6.f, color, Vec2());

	if (m_whitePlayer->m_upgrades[2])
		color = Rgba8::GREEN;
	else
		color = Rgba8::GRAY;
	AddVertsForDisc(circles, Vec2(330.f, 95.f), 6.f, color, Vec2());

	if (m_whitePlayer->m_upgrades[3])
		color = Rgba8::GREEN;
	else
		color = Rgba8::GRAY;
	AddVertsForDisc(circles, Vec2(330.f, 75.f), 6.f, color, Vec2());

	if (m_whitePlayer->m_upgrades[4])
		color = Rgba8::GREEN;
	else
		color = Rgba8::GRAY;
	AddVertsForDisc(circles, Vec2(330.f, 55.f), 6.f, color, Vec2());

	if (m_whitePlayer->m_upgrades[5])
		color = Rgba8::GREEN;
	else
		color = Rgba8::GRAY;
	AddVertsForDisc(circles, Vec2(330.f, 35.f), 6.f, color, Vec2());
	g_theRenderer->DrawVertexArray(circles);

	g_theRenderer->EndCamera(*g_ScreenCamera);
}

void Game::DebugRender() {
	g_theRenderer->BindTexture(nullptr, 2);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetModelConstants(Mat44(), Rgba8::WHITE);
	g_theRenderer->DrawStaticModel(*m_woman);
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
	
	if(g_MatchPlayback!=MISSING_SOUND_ID)
	    g_theAudio->StopSound(g_MatchPlayback);
	g_MenuPlayback = g_theAudio->StartSound(g_Menu, true);
	
	if (g_theMatch) {
		delete g_theMatch;
		g_theMatch = nullptr;
	}
	m_whitePlayer = new ChessPlayer(Faction::WHITE);
	m_blackPlayer = new ChessPlayer(Faction::BLACK);
	DebugRenderSetHidden();
}
void Game::ExitAttractMode() {
	m_attractMode = false;
	
	g_theAudio->StopSound(g_MenuPlayback);
	g_MatchPlayback = g_theAudio->StartSound(g_Match, true);
	
	g_theMatch = new ChessMatch(this);
	if (m_drafting) {
		g_theMatch->m_whitePlayer = m_whitePlayer;
		g_theMatch->m_blackPlayer = m_blackPlayer;
	}
	m_drafting = true;
	g_theMatch->m_board->StartUp();
	
	DebugRenderSetVisible();
}
void Game::HandleKeyPress() {
	if (!(g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL)) {
		if (g_theInput->WasKeyJustReleased('G')) {
		    m_drafting = !m_drafting;
	    }
		if (m_drafting) {
			if (g_theInput->WasKeyJustReleased('1')) {
				if (m_whiteTurn) {
					m_whitePlayer->m_upgrades[0]=true;
				}
				else {
					m_blackPlayer->m_upgrades[0]=true;
				}
				m_whiteTurn=!m_whiteTurn;
			}

			if (g_theInput->WasKeyJustReleased('2')) {
				if (m_whiteTurn) {
					m_whitePlayer->m_upgrades[1] = true;
				}
				else {
					m_blackPlayer->m_upgrades[1] = true;
				}
				m_whiteTurn = !m_whiteTurn;
			}

			if (g_theInput->WasKeyJustReleased('3')) {
				if (m_whiteTurn) {
					m_whitePlayer->m_upgrades[2] = true;
				}
				else {
					m_blackPlayer->m_upgrades[2] = true;
				}
				m_whiteTurn = !m_whiteTurn;
			}

			if (g_theInput->WasKeyJustReleased('4')) {
				if (m_whiteTurn) {
					m_whitePlayer->m_upgrades[3] = true;
				}
				else {
					m_blackPlayer->m_upgrades[3] = true;
				}
				m_whiteTurn = !m_whiteTurn;
			}

			if (g_theInput->WasKeyJustReleased('5')) {
				if (m_whiteTurn) {
					m_whitePlayer->m_upgrades[4] = true;
				}
				else {
					m_blackPlayer->m_upgrades[4] = true;
				}
				m_whiteTurn = !m_whiteTurn;
			}

			if (g_theInput->WasKeyJustReleased('6')) {
				if (m_whiteTurn) {
					m_whitePlayer->m_upgrades[5] = true;
				}
				else {
					m_blackPlayer->m_upgrades[5] = true;
				}
				m_whiteTurn = !m_whiteTurn;
			}
		}
	}
	
}

void Game::LoadSound() {
	g_Vesus  = g_theAudio->CreateOrGetSound("Data/Sound/Start.mp3");
	g_Pawn   = g_theAudio->CreateOrGetSound("Data/Sound/Pawn1.mp3");
	g_Pawn2  = g_theAudio->CreateOrGetSound("Data/Sound/Pawn2.mp3");
	g_Rook   = g_theAudio->CreateOrGetSound("Data/Sound/Rook.mp3");
	g_Knight = g_theAudio->CreateOrGetSound("Data/Sound/Horse.mp3");
	g_Bishop = g_theAudio->CreateOrGetSound("Data/Sound/Bishop.mp3");
	g_Queen  = g_theAudio->CreateOrGetSound("Data/Sound/Queen.mp3");
	g_King   = g_theAudio->CreateOrGetSound("Data/Sound/King.mp3");
	g_Menu   = g_theAudio->CreateOrGetSound("Data/Sound/Menu.mp3");
	g_Match  = g_theAudio->CreateOrGetSound("Data/Sound/Match.mp3");
}



void Game::SendNetworkCommand(std::string const& commandLine)
{

	memcpy((void*)g_network->m_sendBuffer,(void*)commandLine.c_str(),commandLine.size());
	g_network->m_sendBuffer[commandLine.size()+1]='\0';
}

void Game::ReceiveNetworkCommand()
{

	if (g_network->m_revcvBufferSize > 0) {
		std::string remote = g_network->m_recvBuffer;
		g_theDevConsole->AddLine("RemoteCommand:");
		std::string command;
		for (int i = 0; i < g_network->m_revcvBufferSize - 1; i++) {
			command += g_network->m_recvBuffer[i];
		}
		command += " remote=true";
		g_theDevConsole->Execute(command, true);
		g_network->m_recvBuffer[0] = '\0';
	}
}

std::string Game::GetGameState()
{
	std::string state="";
	if (g_theMatch->m_gameState == GameState::BLACK_WON || g_theMatch->m_gameState == GameState::WHITE_WON) {
		state = "GameOver";
	}
	else if(g_theMatch->m_whiteTurn){
	    state = "Player1Turn";
	}
	else {
		state = "Player2Turn";
	}

	std::string player1 = "";
	player1 = g_theMatch->m_whitePlayer->m_name;
	std::string player2 = "";
	player2 = g_theMatch->m_blackPlayer->m_name;
	std::string move = "";
	move = std::to_string(g_theMatch->m_turns);
	std::string board ="";
	board = g_theMatch->m_board->GetBoardStateForValidation();
	std::string result = "CHESSVALIDATE STATE=" + state + " PLAYER1=" + player1 + " PLAYER2=" + player2 + " MOVE=" + move + " BOARD=" + board;
	return result;
}

Game::~Game() {
	DebugRenderSystemShutdown();

}


void Game::BeginFrame() {
	DebugRenderBeginFrame();
}
void Game::EndFrame() {
	DebugRenderEndFrame();
}

bool Game::HandleChessMove(EventArgs& args)
{
	if (!(g_theMatch->m_gameState == GameState::PLAYING)) {
		g_theDevConsole->AddLine("Game is already over");
		return true;
	}
	std::string strFrom = args.GetValue("FROM", "00");
	std::string strTo = args.GetValue("TO", "00");
	std::string strPromote = args.GetValue("PROMOTETO", "N/A");
	bool teleport = args.GetValue("TELEPORT", false);

	Position posFrom = ChessPiece::ParsePositionFromText(strFrom);
	Position posTo = ChessPiece::ParsePositionFromText(strTo);
	Type promotion = ChessBoard::ParseTypeString(strPromote);

	if (posFrom == Position::COUNT || posTo == Position::COUNT) {
		g_theDevConsole->AddLine("Invalid position, Try CHESSMOVE FROM=E2 TO=E4");
	}
	else {
		bool moved = g_theMatch->m_board->TryMovePiece(posFrom, posTo, promotion, teleport);
		if (!moved) {
			g_theDevConsole->AddLine("Chess Move command failed");
		}
	}

	if (!args.GetValue("remote", false)) {
		std::string command = "CHESSMOVE FROM=" + args.GetValue("FROM", "") + " TO=" + args.GetValue("TO", "");
		if (args.GetValue("PROMOTETO", "").compare("") != 0) {
		 command += " PROMOTETO=" + args.GetValue("PROMOTETO", "");
		}
		command	+= " TELEPORT=" + args.GetValue("TELEPORT", "false");
		g_game->SendNetworkCommand(command);
	}


	return true;
}

bool Game::HandlePrintBoardState(EventArgs& args)
{
	UNUSED(args);
	g_theMatch->m_board->PrintBoardState();
	return true;
}



bool Game::HandleChessServerInfo(EventArgs& args)
{
	if (g_network->m_ip.compare(args.GetValue("IP", "N/A"))!=0) {
		g_theDevConsole->AddLine("Warning: Wrong IP Address.");
	}

	if (g_network->m_port != (uint32_t)atoi(args.GetValue("PORT", "N/A").c_str())) {
		g_theDevConsole->AddLine("Warning: Wrong Port Address.");
	}
	g_theDevConsole->AddLine("Name: " + g_network->m_name + " IP: " + g_network->m_ip + " Port: " + std::to_string(g_network->m_port) + 
	" ServerStatus:" + std::to_string(g_network->m_networkStatus));
	return true;
}

bool Game::HandleChessListen(EventArgs& args)
{
	
	std::string ip = args.GetValue("IP", "127.0.0.1");

	std::string port = args.GetValue("PORT", "3100");

	g_network->StartServer(ip, port);
	return true;
}

bool Game::HandleChessConnect(EventArgs& args)
{


	std::string ip = args.GetValue("IP", "127.0.0.1");
	std::string port = args.GetValue("PORT", "3100");

	g_network->StartClient(ip, port);
	return true;

}

bool Game::HandleChessDisconnect(EventArgs& args)
{
	g_network->Disconnect();
	if (!args.GetValue("remote", false)) {
		g_game->SendNetworkCommand("CHESSDISCONNECT" +args.GetValue(" REASON=",""));
	}
	return true;
}

bool Game::HandleChessPlayerInfo(EventArgs& args)
{
	std::string playerName = args.GetValue("NAME", "Player");
	if (args.GetValue("remote", false)) {
		g_game->m_enemyName = playerName;
	}
	else {
		g_game->m_playerName = playerName;
	}
	return true;
}

bool Game::HandleChessBegin(EventArgs& args)
{
    std::string firstplayer = args.GetValue("FIRSTPLAYER", "Player");
	if(!g_game->m_attractMode)
	    g_game->EnterAttractMode();
	g_game->ExitAttractMode();
	if (firstplayer.compare(g_game->m_playerName) == 0) {
		g_theMatch->m_whitePlayer->m_name = g_game->m_playerName;
		g_theMatch->m_blackPlayer->m_name = g_game->m_enemyName;
	}
	else {
		g_theMatch->m_blackPlayer->m_name = g_game->m_playerName;
		g_theMatch->m_whitePlayer->m_name = g_game->m_enemyName;
	}
	if (!args.GetValue("remote", false)) {
		g_game->SendNetworkCommand("CHESSBEGIN FIRSTPLAYER="+firstplayer);
	}
	return true;
}

bool Game::HandleChessValidate(EventArgs& args)
{
	if (args.GetValue("remote", false)) {
	    g_game->SendNetworkCommand(g_game->GetGameState());
	}
    std::string fullstring;
	fullstring = "CHESSVALIDATE STATE=" + args.GetValue("STATE", "N/A") + " PLAYER1=" + args.GetValue("PLAYER1", "N/A") 
	+ " PLAYER2=" + args.GetValue("PLAYER2", "N/A") + " MOVE=" + args.GetValue("MOVE", "N/A") + " BOARD=" + args.GetValue("BOARD", "N/A");

	if (fullstring.compare(g_game->GetGameState()) != 0) {
		g_game->SendNetworkCommand("CHESSDISCONNECT REASON=InvalidGamestate");
		g_network->Disconnect();
	}
	return true;
}

bool Game::HandleChessResign(EventArgs& args)
{
	if (!args.GetValue("remote", false)) {
		g_game->SendNetworkCommand("CHESSRESIGN");
	}
	g_theMatch->m_gameState=GameState::BLACK_WON;
	return true;
}

bool Game::HandleChessOfferDraw(EventArgs& args)
{
	if (!args.GetValue("remote", false)) {
		g_game->SendNetworkCommand("CHESSOFFERDRAW");
	}
	g_game->m_offerDraw=true;
	return true;
}

bool Game::HandleChessAcceptDraw(EventArgs& args)
{
	if (g_game->m_offerDraw) {
		if (!args.GetValue("remote", false)) {
			g_game->SendNetworkCommand("CHESSACCEPTDRAW");
		}
		g_theMatch->m_gameState = GameState::BLACK_WON;
	}
	return true;
}

bool Game::HandleChessRejectDraw(EventArgs& args)
{
	if (g_game->m_offerDraw) {
		if (!args.GetValue("remote", false)) {
			g_game->SendNetworkCommand("CHESSREJECTDRAW");
		}
		g_game->m_offerDraw=false;
	}
	return true;
}

bool Game::HandleRemoteCommand(EventArgs& args)
{
	g_game->SendNetworkCommand(args.GetAsString());
    return true;
}
