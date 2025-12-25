#include"ChessMatch.hpp"
ChessMatch* g_theMatch = nullptr; 
ChessMatch::ChessMatch(Game* owner) :m_owner(owner)
{
	ChessPieceDefinition::InitializeDefinitions();
    m_board = new ChessBoard(this);
	m_whitePlayer = new ChessPlayer(Faction::WHITE);
	m_blackPlayer = new ChessPlayer(Faction::BLACK);
}

ChessMatch::~ChessMatch()
{
    delete m_board;
    m_board = nullptr;
	for (ChessPieceDefinition def : ChessPieceDefinition::g_chessDefinitions) {
		delete def.m_indexBuffer;
		def.m_indexBuffer=nullptr;
		delete def.m_indexBuffer2;
		def.m_indexBuffer2 = nullptr;

		delete def.m_vertexBuffer;
		def.m_vertexBuffer =nullptr;
        delete def.m_vertexBuffer2;
		def.m_vertexBuffer2=nullptr;
	}
	ChessPieceDefinition::g_chessDefinitions.clear();
	delete m_blackPlayer;
	m_blackPlayer=nullptr;
	delete m_whitePlayer;
	m_whitePlayer=nullptr;
}

void ChessMatch::Update(float deltaSeconds)
{
   UNUSED(deltaSeconds);
   m_board->Update();
   HandleKeyPressed();
   if (m_gameState == GameState::BLACK_WON || m_gameState == GameState::WHITE_WON) {
	   g_game->EnterAttractMode();
   }
}

void ChessMatch::GameOver(Faction looser) {
	if (looser == Faction::WHITE) {
		m_gameState = GameState::BLACK_WON;
	}
	else {
		m_gameState = GameState::WHITE_WON;
	}
}

void ChessMatch::HandleKeyPressed()
{
	if (g_theInput->WasKeyJustReleased(KEYCODE_F4)) {
		m_freeCamera=!m_freeCamera;
	}
}

