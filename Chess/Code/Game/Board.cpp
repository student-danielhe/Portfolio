#include "Board.hpp"
#include "Engine/Core/VertexUtils.hpp"

void Board::GameOver(Faction looser) {
    if (looser == Faction::WHITE) {
        m_gameState = GameState::BLACK_WON;
    }
    else {
        m_gameState = GameState::WHITE_WON;
    }
}

void Board::Render() {
		//render board
    for (int i = 0; i < m_cells.size(); i++) {
        m_cells[i]->Render();
    }

	for (int i = 0; i < (int)Position::COUNT; i++) {
		if (m_whitePieces[i]) {
			m_whitePieces[i]->Render();
		}
	}
    
}

void Board::Update() {
    DeleteCapturedPieces();
}

bool Board::TryMovePiece(Position piecePos, Position destination) {
    if (m_whitePieces[(int)piecePos]) {
		
		if (m_whitePieces[(int)destination]) {

			//dev console log: blocked
			return false;
		}
		bool moved = m_whitePieces[(int)piecePos]->Move(destination);
		if (moved && m_blackPieces[(int)destination]) {
			m_blackPieces[(int)destination]->Captured();
		}
		return moved;
		
	}
	else if (m_blackPieces[(int)piecePos]) {
		if (m_blackPieces[(int)destination]) {

			//dev console log: blocked
			return false;
		}
		bool moved = m_blackPieces[(int)piecePos]->Move(destination);
		if (moved&& m_whitePieces[(int)destination]) {
			m_whitePieces[(int)destination]->Captured();
		}
		return moved;
	}
    //Dev console log: no piece at position
    return false;
}

Board::Board(Game* owner):m_owner(owner) 
{
    m_whitePieces.resize((int)Position::COUNT);
    m_blackPieces.resize((int)Position::COUNT);
    InitializePieces();
    InitializeProp();
    SubscribeEventCallbackFunction("QUIT", Board::ChessMove);
}

void Board::InitializePieces() {
	for (int i = 0; i < (int)Position::COUNT; i++) {
		if (m_whitePieces[i]) {
            delete m_whitePieces[i];
            m_whitePieces[i]=nullptr;
		}
	}
	for (int i = 0; i < (int)Position::COUNT; i++) {
		if (m_blackPieces[i]) {
			delete m_blackPieces[i];
			m_blackPieces[i] = nullptr;
		}
	}
    //Non Pawns
    m_whitePieces[(int)Position::A1] = new ChessPiece(Position::A1, ChessPieceDefinition::g_chessDefinitions[(int)Type::ROOK], Faction::WHITE, this);
    m_whitePieces[(int)Position::B1] = new ChessPiece(Position::B1, ChessPieceDefinition::g_chessDefinitions[(int)Type::KNIGHT], Faction::WHITE, this);
    m_whitePieces[(int)Position::C1] = new ChessPiece(Position::C1, ChessPieceDefinition::g_chessDefinitions[(int)Type::BISHOP], Faction::WHITE, this);
    m_whitePieces[(int)Position::D1] = new ChessPiece(Position::D1, ChessPieceDefinition::g_chessDefinitions[(int)Type::KING],   Faction::WHITE, this);
    m_whitePieces[(int)Position::E1] = new ChessPiece(Position::E1, ChessPieceDefinition::g_chessDefinitions[(int)Type::QUEEN],  Faction::WHITE, this);
    m_whitePieces[(int)Position::F1] = new ChessPiece(Position::F1, ChessPieceDefinition::g_chessDefinitions[(int)Type::BISHOP], Faction::WHITE, this);
    m_whitePieces[(int)Position::G1] = new ChessPiece(Position::G1, ChessPieceDefinition::g_chessDefinitions[(int)Type::KNIGHT], Faction::WHITE, this);
    m_whitePieces[(int)Position::H1] = new ChessPiece(Position::H1, ChessPieceDefinition::g_chessDefinitions[(int)Type::ROOK],   Faction::WHITE, this);

	m_blackPieces[(int)Position::A8] = new ChessPiece(Position::A8, ChessPieceDefinition::g_chessDefinitions[(int)Type::ROOK],   Faction::BLACK, this);
	m_blackPieces[(int)Position::B8] = new ChessPiece(Position::B8, ChessPieceDefinition::g_chessDefinitions[(int)Type::KNIGHT], Faction::BLACK, this);
	m_blackPieces[(int)Position::C8] = new ChessPiece(Position::C8, ChessPieceDefinition::g_chessDefinitions[(int)Type::BISHOP], Faction::BLACK, this);
	m_blackPieces[(int)Position::D8] = new ChessPiece(Position::D8, ChessPieceDefinition::g_chessDefinitions[(int)Type::KING],   Faction::BLACK, this);
	m_blackPieces[(int)Position::E8] = new ChessPiece(Position::E8, ChessPieceDefinition::g_chessDefinitions[(int)Type::QUEEN],  Faction::BLACK, this);
	m_blackPieces[(int)Position::F8] = new ChessPiece(Position::F8, ChessPieceDefinition::g_chessDefinitions[(int)Type::BISHOP], Faction::BLACK, this);
	m_blackPieces[(int)Position::G8] = new ChessPiece(Position::G8, ChessPieceDefinition::g_chessDefinitions[(int)Type::KNIGHT], Faction::BLACK, this);
	m_blackPieces[(int)Position::H8] = new ChessPiece(Position::H8, ChessPieceDefinition::g_chessDefinitions[(int)Type::ROOK],   Faction::BLACK, this);
    
    //Pawns
    for (int i = (int)Position::A2; i <= (int)Position::H2; i++) {
        m_whitePieces[i] = new ChessPiece((Position)i, ChessPieceDefinition::g_chessDefinitions[(int)Type::PAWN], Faction::WHITE, this);
    }
	
    for (int i = (int)Position::A7; i <= (int)Position::H7; i++) {
		m_blackPieces[i] = new ChessPiece((Position)i, ChessPieceDefinition::g_chessDefinitions[(int)Type::PAWN], Faction::BLACK, this);
	}
}

void Board::InitializeProp()
{
    for (int i = 0; i < (int)Position::COUNT; i++) {
        std::vector<Vertex_PCU> verts;
        AddVertsForAABB3D(verts,AABB3(Vec3(),Vec3(CellSize,CellSize,CellSize*.1f)));
        Prop* prop =  new Prop(m_owner);
        prop->SetVerts(verts);
        Position pos = (Position) i;
		float xPos = ((int)pos % 8) * CellSize;
		float yPos = (((int)pos - ((int)pos % 8)) / 8) * CellSize;
		prop->SetPosition(Vec3(xPos-.5f, yPos-.5f, -.5f));
        if (((int)pos % 8) % 2 xor (((int)pos - ((int)pos % 8)) / 8) % 2) {
            prop->SetColor(Rgba8::WHITE);
        }
        else {
            prop->SetColor(Rgba8::BLACK);
        }
        m_cells.push_back(prop);
    }
}

void Board::DeleteCapturedPieces() {
	for (int i = 0; i < (int)Position::COUNT; i++) {
		if (m_whitePieces[i]&&m_whitePieces[i]->m_captured) {
			delete m_whitePieces[i];
			m_whitePieces[i] = nullptr;
		}
	}

	for (int i = 0; i < (int)Position::COUNT; i++) {
		if (m_blackPieces[i] && m_blackPieces[i]->m_captured) {
			delete m_blackPieces[i];
			m_blackPieces[i] = nullptr;
		}
	}
}

std::string Board::PrintBoardState()
{
    std::string result = "";

    for (int y = 8; y > 0; y--) {
        result.append(" ABCDEFGH");
        result.append(std::to_string(y));
        for (int x = 0; x < 8; x++) {
            ChessPiece* wpiece = m_whitePieces[x+y*8];
            ChessPiece* bpiece = m_blackPieces[x + y * 8];
            ChessPiece* piece = nullptr;
           
           if (wpiece) {
                piece = wpiece;
            }
            else if (bpiece) {
                piece = bpiece;
            }

            if (!piece) {
                result.append(".");
            }
            else {
                Type type = piece->m_def.m_type;
                if (type == Type::PAWN) {
                    result.append("P");
                }
                if (type == Type::BISHOP) {
                    result.append("B");
                }
                if (type == Type::KING) {
                    result.append("K");
                }
                if (type == Type::KNIGHT) {
                    result.append("N");
                }
                if (type == Type::QUEEN) {
                    result.append("Q");
                }
                if (type == Type::ROOK) {
                    result.append("R");
                }
            }
        }
        result.append(" ABCDEFGH");
    }
    return result;
}

bool Board::ChessMove(EventArgs& args)
{
    std::string strFrom = args.GetValue("1", "00");
    std::string strTo = args.GetValue("1", "00");
    strFrom.erase(0,5);
    strTo.erase(0,3);
    Position posFrom = ChessPiece::ParsePositionFromText(strFrom);
    Position posTo = ChessPiece::ParsePositionFromText(strTo);
    if (posFrom == Position::COUNT || posTo == Position::COUNT) {
        //print invalid position
    }
    else {
    //bool moved = g_theBoard->TryMovePiece(posFrom,posTo);
    }
    return true;
}
