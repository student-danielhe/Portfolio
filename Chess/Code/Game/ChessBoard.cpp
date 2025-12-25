#include "ChessBoard.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "ChessMatch.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Network/NetworkSystem.hpp"

void ChessBoard::StartUp()
{
	m_whiteCamera = new Camera();
	m_blackCamera = new Camera();
	m_whiteCamera->SetPositionAndOrientation(Vec3(4.f, -3.f, 6.f), EulerAngles(90.f, 45.f, 0.f));
	m_blackCamera->SetPositionAndOrientation(Vec3(4.f, 11.f, 6.f), EulerAngles(270.f, 45.f, 0.f));

	Mat44 CamToRender(Vec4(0.f, -1.f, 0.f, 0.f),
		Vec4(0.f, 0.f, 1.f, 0.f),
		Vec4(1.f, 0.f, 0.f, 0.f),
		Vec4(0.f, 0.f, 0.f, 1.f));
	CamToRender.Transpose();

	m_whiteCamera->SetPerspectiveView(2.f, 60.f, .1f, 100.f);
	m_whiteCamera->SetCameraToRenderTransform(CamToRender);

	m_blackCamera->SetPerspectiveView(2.f, 60.f, .1f, 100.f);
	m_blackCamera->SetCameraToRenderTransform(CamToRender);

	CreateBuffers();

	std::string filename = "Data/Shaders/Diffuse.hlsl";
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open file " << filename << std::endl;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	m_diffuseShader = g_theRenderer->CreateShader("Diffuse", buffer.str().c_str(), VertexType::VERTEX_PCUTBN);


	g_theRenderer->SetBuffers(m_vertexBuffer, m_indexBuffer);

	m_piecesOnBoard.resize((int)Position::COUNT);

	InitializePieces();

	InitializeGeometry();


	PrintBoardState();

	m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Bricks_d.png");
	m_normalMap = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Bricks_n.png");
	m_SGE = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Bricks_sge.png");
	g_theAudio->StartSound(Game::g_Vesus);
}

void ChessBoard::Render() {
		//render board
	if (m_owner->m_freeCamera) {
		g_thePlayerCamera->SetPerspectiveView(2.f, 60.f, .1f, 100.f);
		Mat44 CamToRender(Vec4(0.f, -1.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 1.f, 0.f),
			Vec4(1.f, 0.f, 0.f, 0.f),
			Vec4(0.f, 0.f, 0.f, 1.f));
		CamToRender.Transpose();
		g_thePlayerCamera->SetCameraToRenderTransform(CamToRender);
		g_theRenderer->BeginCamera(*g_thePlayerCamera);
	}
	else {
		if (m_owner->m_whiteTurn) {
		    g_theRenderer->BeginCamera(*m_whiteCamera);
		}
		else {
		    g_theRenderer->BeginCamera(*m_blackCamera);
		}
	}
   g_theRenderer->BindTexture(m_texture,0);
   g_theRenderer->BindTexture(m_normalMap,1);
   g_theRenderer->BindTexture(m_SGE, 2);
   g_theRenderer->SetSampleMode(SamplerMode::BILINEAR_WRAP, 0);
   g_theRenderer->SetSampleMode(SamplerMode::BILINEAR_WRAP, 1);
   g_theRenderer->SetSampleMode(SamplerMode::BILINEAR_WRAP, 2);
   g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
   g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
   g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
   g_theRenderer->BindShader(m_diffuseShader);
   g_theRenderer->SetModelConstants();


  /*g_theRenderer->DrawVertexArray(m_vertexes, m_indexes);*/
  g_theRenderer->SetBuffers(m_vertexBuffer,m_indexBuffer);

  g_theRenderer->DrawIndexedVertexBuffer(m_vertexBuffer,m_indexBuffer);

  g_theRenderer->SetBuffers();

	for (int i = 0; i < (int)Position::COUNT; i++) {
		if (m_piecesOnBoard[i]) {
			m_piecesOnBoard[i]->Render();
		}
	}

	

	if (m_owner->m_freeCamera) {
	    RaycastResult3D result; 
		IntVec2 cell = RayCastVSBoard(g_player->GetPosition(), g_player->GetFwdVector(), 10.f, result);
		if (result.m_didImpact) {
			m_targetedCell = cell;
		}
		RenderHighLighted(m_targetedCell);
		g_theRenderer->EndCamera(*g_thePlayerCamera);
	}
	else {
		if (m_owner->m_whiteTurn) {
			g_theRenderer->EndCamera(*m_whiteCamera);
		}
		else {
			g_theRenderer->EndCamera(*m_blackCamera);
		}
	}
}

void ChessBoard::Update() {
    HandleKeyPressed();
}

bool ChessBoard::TryMovePiece(Position piecePos, Position destination, Type promoteTo, bool teleport) {
    if (g_theMatch->m_whiteTurn&&m_piecesOnBoard[(int)piecePos]) {
		if(m_piecesOnBoard[(int)piecePos]->m_faction==Faction::WHITE){
		    
				bool moved = m_piecesOnBoard[(int)piecePos]->Move(destination, promoteTo, teleport);
				if (moved) {
					g_theMatch->m_whiteTurn = !g_theMatch->m_whiteTurn;
					g_theMatch->m_turns++;
					g_theMatch->m_board->PrintBoardState();
				}
				return moved;
			
		}
		else {
			g_theDevConsole->AddLine("This is white's turn");
		}
		
	}
	else if (!g_theMatch->m_whiteTurn&&m_piecesOnBoard[(int)piecePos]) {
		if (m_piecesOnBoard[(int)piecePos]->m_faction == Faction::BLACK) {
			bool moved = m_piecesOnBoard[(int)piecePos]->Move(destination, promoteTo, teleport);
			if (moved) {
				g_theMatch->m_whiteTurn = !g_theMatch->m_whiteTurn;
				g_theMatch->m_turns++;
				g_theMatch->m_board->PrintBoardState();
			}
			return moved;
		}
		else {
			g_theDevConsole->AddLine("This is black's turn");
		}
	}
	g_theDevConsole->AddLine("No piece at the position");
    return false;
}

ChessBoard::ChessBoard(ChessMatch* owner):m_owner(owner) 
{
	
}

ChessBoard::~ChessBoard()
{
	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;
	delete m_indexBuffer;
	m_indexBuffer = nullptr;
    g_theRenderer->SetBuffers();
	delete m_whiteCamera;
	m_whiteCamera=nullptr;
	delete m_blackCamera;
	m_blackCamera=nullptr;
}

void ChessBoard::InitializePieces() {
	for (int i = 0; i < (int)Position::COUNT; i++) {
		if (m_piecesOnBoard[i]) {
            delete m_piecesOnBoard[i];
            m_piecesOnBoard[i]=nullptr;
		}
	}

    //Non Pawns
    m_piecesOnBoard[(int)Position::A1] = new ChessPiece(Position::A1, ChessPieceDefinition::g_chessDefinitions[(int)Type::ROOK], Faction::WHITE, this);
    m_piecesOnBoard[(int)Position::B1] = new ChessPiece(Position::B1, ChessPieceDefinition::g_chessDefinitions[(int)Type::KNIGHT], Faction::WHITE, this);
    m_piecesOnBoard[(int)Position::C1] = new ChessPiece(Position::C1, ChessPieceDefinition::g_chessDefinitions[(int)Type::BISHOP], Faction::WHITE, this);
    m_piecesOnBoard[(int)Position::D1] = new ChessPiece(Position::D1, ChessPieceDefinition::g_chessDefinitions[(int)Type::KING],   Faction::WHITE, this);
    m_piecesOnBoard[(int)Position::E1] = new ChessPiece(Position::E1, ChessPieceDefinition::g_chessDefinitions[(int)Type::QUEEN],  Faction::WHITE, this);
    m_piecesOnBoard[(int)Position::F1] = new ChessPiece(Position::F1, ChessPieceDefinition::g_chessDefinitions[(int)Type::BISHOP], Faction::WHITE, this);
    m_piecesOnBoard[(int)Position::G1] = new ChessPiece(Position::G1, ChessPieceDefinition::g_chessDefinitions[(int)Type::KNIGHT], Faction::WHITE, this);
    m_piecesOnBoard[(int)Position::H1] = new ChessPiece(Position::H1, ChessPieceDefinition::g_chessDefinitions[(int)Type::ROOK],   Faction::WHITE, this);

	m_piecesOnBoard[(int)Position::A8] = new ChessPiece(Position::A8, ChessPieceDefinition::g_chessDefinitions[(int)Type::ROOK],   Faction::BLACK, this);
	m_piecesOnBoard[(int)Position::B8] = new ChessPiece(Position::B8, ChessPieceDefinition::g_chessDefinitions[(int)Type::KNIGHT], Faction::BLACK, this);
	m_piecesOnBoard[(int)Position::C8] = new ChessPiece(Position::C8, ChessPieceDefinition::g_chessDefinitions[(int)Type::BISHOP], Faction::BLACK, this);
	m_piecesOnBoard[(int)Position::D8] = new ChessPiece(Position::D8, ChessPieceDefinition::g_chessDefinitions[(int)Type::KING],   Faction::BLACK, this);
	m_piecesOnBoard[(int)Position::E8] = new ChessPiece(Position::E8, ChessPieceDefinition::g_chessDefinitions[(int)Type::QUEEN],  Faction::BLACK, this);
	m_piecesOnBoard[(int)Position::F8] = new ChessPiece(Position::F8, ChessPieceDefinition::g_chessDefinitions[(int)Type::BISHOP], Faction::BLACK, this);
	m_piecesOnBoard[(int)Position::G8] = new ChessPiece(Position::G8, ChessPieceDefinition::g_chessDefinitions[(int)Type::KNIGHT], Faction::BLACK, this);
	m_piecesOnBoard[(int)Position::H8] = new ChessPiece(Position::H8, ChessPieceDefinition::g_chessDefinitions[(int)Type::ROOK],   Faction::BLACK, this);
    
    //Pawns
    for (int i = (int)Position::A2; i <= (int)Position::H2; i++) {
        m_piecesOnBoard[i] = new ChessPiece((Position)i, ChessPieceDefinition::g_chessDefinitions[(int)Type::PAWN], Faction::WHITE, this);
    }
	
    for (int i = (int)Position::A7; i <= (int)Position::H7; i++) {
		m_piecesOnBoard[i] = new ChessPiece((Position)i, ChessPieceDefinition::g_chessDefinitions[(int)Type::PAWN], Faction::BLACK, this);
	}
}

void ChessBoard::InitializeGeometry()
{

    for (int i = 0; i < (int)Position::COUNT; i++) {
		Position pos = (Position)i;
		float xPos = ((int)pos % 8) * CellSize;
		float yPos = (((int)pos - ((int)pos % 8)) / 8) * CellSize;
		Rgba8 color =Rgba8::WHITE;
		if ((((int)pos % 8) % 2 == 1) xor ((((int)pos - ((int)pos % 8)) / 8) % 2 == 1)) {
		    color = Rgba8::BLACK;
		}

        AddVertsForAABB3D(m_vertexes,m_indexes, AABB3(Vec3(xPos,yPos,-.1f), Vec3(xPos+CellSize, yPos+CellSize, 0.f)), color);

		
    }

	g_theRenderer->CopyIndexedVertexCPUTBNToBuffers(m_vertexes,m_indexes,m_vertexBuffer, m_indexBuffer);
}



void ChessBoard::CreateBuffers()
{
	m_vertexBuffer = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_indexBuffer = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));
}

void ChessBoard::PrintBoardState()
{
	std::string result = "";

	result += " ABCDEFGH";
	g_theDevConsole->AddLine(result);

	result = "";
	for (int y = 7; y >= 0; y--) {

		result.append(std::to_string(y + 1));
		for (int x = 0; x < 8; x++) {

			ChessPiece* piece = m_piecesOnBoard[x + y * 8];

			if (!piece) {
				result += ".";
			}
			else {
				Type type = piece->m_def.m_type;
				if (piece->m_faction == Faction::BLACK) {
					if (type == Type::PAWN) {
						result += "P";
					}
					if (type == Type::BISHOP) {
						result += "B";
					}
					if (type == Type::KING) {
						result += "K";
					}
					if (type == Type::KNIGHT) {
						result += "N";
					}
					if (type == Type::QUEEN) {
						result += "Q";
					}
					if (type == Type::ROOK) {
						result += "R";
					}
				}
				else {
					if (type == Type::PAWN) {
						result += "p";
					}
					if (type == Type::BISHOP) {
						result += "b";
					}
					if (type == Type::KING) {
						result += "k";
					}
					if (type == Type::KNIGHT) {
						result += "n";
					}
					if (type == Type::QUEEN) {
						result += "q";
					}
					if (type == Type::ROOK) {
						result += "r";
					}
				}
				
			}
		}
		result += std::to_string(y + 1);
		
		g_theDevConsole->AddLine(result);

		result = "";
	}
	result = " ABCDEFGH";
	g_theDevConsole->AddLine(result);
}

std::string ChessBoard::GetBoardStateForValidation()
{
	std::string result = "";


	for (int y = 7; y >= 0; y--) {
		for (int x = 0; x < 8; x++) {

			ChessPiece* piece = m_piecesOnBoard[x + y * 8];

			if (!piece) {
				result += ".";
			}
			else {
				Type type = piece->m_def.m_type;
				if (piece->m_faction == Faction::BLACK) {
					if (type == Type::PAWN) {
						result += "P";
					}
					if (type == Type::BISHOP) {
						result += "B";
					}
					if (type == Type::KING) {
						result += "K";
					}
					if (type == Type::KNIGHT) {
						result += "N";
					}
					if (type == Type::QUEEN) {
						result += "Q";
					}
					if (type == Type::ROOK) {
						result += "R";
					}
				}
				else {
					if (type == Type::PAWN) {
						result += "p";
					}
					if (type == Type::BISHOP) {
						result += "b";
					}
					if (type == Type::KING) {
						result += "k";
					}
					if (type == Type::KNIGHT) {
						result += "n";
					}
					if (type == Type::QUEEN) {
						result += "q";
					}
					if (type == Type::ROOK) {
						result += "r";
					}
				}

			}
		}

	}
	return result;
}

Type ChessBoard::ParseTypeString(std::string str)
{
	if (str.compare("PAWN") == 0) {
		return Type::PAWN;
	}
	else if (str.compare("ROOK") == 0) {
		return Type::ROOK;
	}
	else if (str.compare("KNIGHT") == 0) {
		return Type::KNIGHT;
	}
	else if (str.compare("BISHOP") == 0) {
		return Type::BISHOP;
	}
	else if (str.compare("QUEEN") == 0) {
		return Type::QUEEN;
	}
	else if (str.compare("KING") == 0) {
		return Type::KING;
	}

	return Type::COUNT;
}



IntVec2 ChessBoard::RayCastVSBoard(Vec3 startPos, Vec3 fwdNormal, float maxDist, RaycastResult3D& out_result)
{
	if (fwdNormal.z < 0) {
	std::vector<RaycastResult3D> allhits;
	std::vector<IntVec2> allhitPositions;
		for (int i = 0; i < (int)Position::COUNT; i++) {
			Position pos = (Position)i;
			float xPos = ((int)pos % 8) * CellSize;
			float yPos = (((int)pos - ((int)pos % 8)) / 8) * CellSize;

			AABB3 cell = AABB3(Vec3(xPos, yPos, -.1f), Vec3(xPos + CellSize, yPos + CellSize, 0.f));
			RaycastResult3D result = RaycastVsAABB3D(startPos,fwdNormal,maxDist,cell);
			if (result.m_didImpact) {
				allhits.push_back(result);
				allhitPositions.push_back(IntVec2((int)xPos, (int)yPos));
			}
			if (m_piecesOnBoard[i]) {
				ChessPiece* piece = m_piecesOnBoard[i];
				RaycastResult3D pieceResult = piece->RayCastVSMe(startPos, fwdNormal, maxDist);
				if (pieceResult.m_didImpact) {
					allhits.push_back(pieceResult);
					allhitPositions.push_back(IntVec2((int)xPos, (int)yPos));
				}
			}
		}


		if (allhits.size() > 0) {
			float closestDist = FLT_MAX;
			IntVec2 result =IntVec2();
			for (int i = 0; i < allhits.size(); i++) {
				if (allhits[i].m_impactDist < closestDist) {
					out_result = allhits[i];
					closestDist = allhits[i].m_impactDist;
					result = allhitPositions[i];
				}
			}
			return result;
		}
		else 
			return IntVec2(-1, -1);
	}
	else {
		return IntVec2(-1, -1);
	}
	
}

void ChessBoard::RenderHighLighted(IntVec2 cell)
{
	if (cell.x >=0) {
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_NONE);
		std::vector<Vertex_PCU> verts;
		AddVertsForAABB3D(verts, AABB3(Vec3(6.5f-cell.x, 6.5f-cell.y, 0.f), Vec3(6.5f-cell.x + CellSize, 6.5f-cell.y + CellSize, .3f)), Rgba8::RED);
		if (m_piecesOnBoard[(int)cell.x + (int)cell.y * 8]) {
			AddVertsForZCylinder3D(verts, Vec3(7.f-cell.x, 7.f-cell.y, 0.f), 1.f, .3f, Rgba8::RED);
		}

		g_theRenderer->DrawVertexArray(verts);
		g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	}

	if (m_selectedPiece) {
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_NONE);
		std::vector<Vertex_PCU> verts;
		IntVec2 chessCell = ChessPiece::GetIntVec2FromPosition(m_selectedPiece->m_position);
		AddVertsForZCylinder3D(verts, Vec3(7.f - chessCell.x, 7.f - chessCell.y, 0.f), 1.f, .3f, Rgba8::GREEN);
		g_theRenderer->DrawVertexArray(verts);
		g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	}
}

void ChessBoard::HandleKeyPressed()
{
	if (g_theInput->WasKeyJustReleased(KEYCODE_LEFT_MOUSE)) {
	    RaycastResult3D result;
		IntVec2 pos = RayCastVSBoard(g_player->GetPosition(), g_player->GetFwdVector(), 10.f, result);
		if (pos.x >= 0 && pos.y >= 0 && pos.x < 8 && pos.y < 8) {
			if (m_selectedPiece) {
				bool moved = false;
				if (m_selectedPiece->m_def.m_type == Type::PAWN && (pos.y == 7 || pos.y == 0)) {
					moved = TryMovePiece(m_selectedPiece->m_position, (Position)(pos.x + pos.y * 8), Type::QUEEN, m_cheating);
				}
				else
					moved = TryMovePiece(m_selectedPiece->m_position, (Position)(pos.x + pos.y * 8), Type::COUNT, m_cheating);
				if (moved) {
					m_selectedPiece = nullptr;
				}
			}
			else {
				if (pos.x != -1 && m_piecesOnBoard[pos.x + pos.y * 8])
					m_selectedPiece = m_piecesOnBoard[pos.x + pos.y * 8];
			}
		}
		
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHT_MOUSE)) {
		m_selectedPiece = nullptr;
	}
	m_cheating=g_theInput->WasKeyJustPressed(KEYCODE_CONTROL);
}


