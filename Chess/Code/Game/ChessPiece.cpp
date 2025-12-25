#include "ChessPiece.hpp"
#include "ChessBoard.hpp"
#include "GameCommon.hpp"
#include "ChessMatch.hpp"
#include "Engine/Core/VertexUtils.hpp"

ChessPiece::ChessPiece(Position pos, ChessPieceDefinition def, Faction faction, ChessBoard* board) :m_position(pos),m_prevPosition(pos), m_def(def), m_faction(faction), m_board(board)
{
    std::vector<Vertex_PCU> verts;
	AddVertsForSphere3D(verts,CellSize*.3f);
	if (m_faction == Faction::WHITE) {
		m_color = Rgba8(0,200,0);
	}
	else {
		m_color = Rgba8(200,0,0);
		m_orientation = EulerAngles(180.f, 0.f, 0.f);
	}
	if (faction == Faction::WHITE) {
		m_player=g_theMatch->m_whitePlayer;
	}
	else {
		m_player = g_theMatch->m_blackPlayer;
	}

}

void ChessPiece::Render()
{
	g_theRenderer->BindTexture(m_def.m_texture, 0);
	g_theRenderer->BindTexture(m_def.m_normalMap, 1);
	g_theRenderer->BindTexture(m_def.m_SGE,2);
	g_theRenderer->SetSampleMode(SamplerMode::BILINEAR_WRAP, 0);
	g_theRenderer->SetSampleMode(SamplerMode::BILINEAR_WRAP, 1);
	g_theRenderer->SetSampleMode(SamplerMode::BILINEAR_WRAP, 2);


	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->SetDepthMode(DepthMode::READ_WRITE_LESS_EQUAL);
	//g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetModelConstants(GetModelToWorld());


	if (m_faction == Faction::WHITE) {
		g_theRenderer->SetModelConstants(GetModelToWorld(),Rgba8(220,220,220));
		g_theRenderer->SetBuffers(m_def.m_vertexBuffer, m_def.m_indexBuffer);
		g_theRenderer->DrawIndexedVertexBuffer(m_def.m_vertexBuffer, m_def.m_indexBuffer);
	}
	else {
		g_theRenderer->SetModelConstants(GetModelToWorld(), Rgba8(50,50,50));
		g_theRenderer->SetBuffers(m_def.m_vertexBuffer2, m_def.m_indexBuffer2);
		g_theRenderer->DrawIndexedVertexBuffer(m_def.m_vertexBuffer2, m_def.m_indexBuffer2);
	}
	

	g_theRenderer->SetBuffers();

}

bool ChessPiece::Move(Position pos, Type promoteTo, bool teleport) {
	//promotion checks
	//Case:Non pawn trying to promote trying
	if (promoteTo != Type::COUNT && m_def.m_type != Type::PAWN) {
		g_theDevConsole->AddLine("Only Pawn may promote");
		return false;
	}
	//Case: wrong promotion type
	if (promoteTo == Type::KING || promoteTo == Type::PAWN) {
		g_theDevConsole->AddLine("Invalid Promotion type");
		return false;
	}

	
	
	if (m_def.m_type == Type::PAWN) {
	    int y = GetIntVec2FromPosition(pos).y;
		//Case: try to promote without reaching final line
		if ((y == 0 || y == 7)&&promoteTo==Type::COUNT) {
			g_theDevConsole->AddLine("Attempt to move pawn to final line without promoting");
			g_theDevConsole->AddLine("Try add PROMOTETO=QUEEN after the command");
			return false;
		}
		//Case reached final line but didn't promote
		if (y != 0 && y != 7 && promoteTo != Type::COUNT) {
			g_theDevConsole->AddLine("Pawn may only promote after reaching the last lane");
			return false;
		}
	}

	//start to move
	bool moved=false;
	ChessPiece* targetPiece = g_theMatch->m_board->m_piecesOnBoard[(int)pos];
	if (teleport) {
	    moved = !((targetPiece)&&(targetPiece->m_faction==m_faction));
	}
	else{
		moved = CheckValidMove(pos);
    }

	if (moved) {
	//Capturing
		bool banditPawn = m_player->m_upgrades[(int)Upgrade::PAWN_BANDIT];
		Type targetType = Type::COUNT;
		if (targetPiece) {
			
			if (banditPawn&&m_def.m_type==Type::PAWN) {
				targetType = targetPiece->m_def.m_type;
			}
			targetPiece->Captured();
		}
		//promoting
		if (promoteTo != Type::COUNT) {
			g_theMatch->m_board->m_piecesOnBoard[(int)pos] = new ChessPiece(pos, ChessPieceDefinition::g_chessDefinitions[(int)promoteTo], m_faction, m_board);
			g_theMatch->m_board->m_piecesOnBoard[(int)pos]->m_totalMove = m_totalMove;
			g_theMatch->m_board->m_piecesOnBoard[(int)m_position] = nullptr;
		    delete this;
		}
		else if(targetType!=Type::COUNT){
		//Promote on capture
			g_theMatch->m_board->m_piecesOnBoard[(int)pos] = new ChessPiece(pos, ChessPieceDefinition::g_chessDefinitions[(int)targetType], m_faction, m_board);
			g_theMatch->m_board->m_piecesOnBoard[(int)pos]->m_totalMove = m_totalMove;
			g_theMatch->m_board->m_piecesOnBoard[(int)m_position] = nullptr;
			delete this;
		}
		else{
		     //perform movment
			IntVec2 targetPosition = GetIntVec2FromPosition(pos);
			bool bouncing=false;
			IntVec2 direction;
			if (m_def.m_type == Type::BISHOP && m_player->m_upgrades[(int)Upgrade::BISHOP_BOUNCY]&&
			(targetPosition.x == 0 || targetPosition.x == 7)!=(targetPosition.y == 0 || targetPosition.y == 7)) {
			    m_bishopBouncing=true;
				m_bishopBouncePosition = pos;
				bouncing = true;
				if (targetPosition.x > GetIntVec2FromPosition(m_position).x) {
					direction.x=1;
				}
				else {
					direction.x = -1;
				}

				if (targetPosition.y > GetIntVec2FromPosition(m_position).y) {
					direction.y = 1;
				}
				else {
					direction.y = -1;
				}

				if (targetPosition.x == 0 || targetPosition.x == 7) {
					direction.x=-direction.x;
				}

				if (targetPosition.y == 0 || targetPosition.y == 7) {
					direction.y = -direction.y;
				}

				while (bouncing) {


					ChessPiece* blockingPiece = m_board->m_piecesOnBoard[(int)GetPositionFromIntVec2(targetPosition + direction)];
					if (blockingPiece) {
						bouncing = false;
						if (blockingPiece->m_faction != m_faction) {
							targetPosition = targetPosition + direction;
						}
						break;
					}
					targetPosition = targetPosition + direction;
					if ((targetPosition.x == 0 || targetPosition.x == 7) || (targetPosition.y == 0 || targetPosition.y == 7))
						bouncing = false;
				}
				pos = GetPositionFromIntVec2(targetPosition);
			}

			g_theMatch->m_board->m_piecesOnBoard[(int)pos] = g_theMatch->m_board->m_piecesOnBoard[(int)m_position];
			g_theMatch->m_board->m_piecesOnBoard[(int)m_position] = nullptr;
			m_prevPosition = m_position;
			m_position = pos;
			m_totalMove++;
			m_lastMoveSecond = (float)g_gameClock->GetTotalSeconds();
			m_lastMovedTurn = g_theMatch->m_turns;

			//After movement
		//Spawn new queen
			if (moved && m_totalMove == 1 && m_player->m_upgrades[(int)Upgrade::QUEEN_POLYGAMY] && m_def.m_type == Type::QUEEN) {
				g_theMatch->m_board->m_piecesOnBoard[(int)m_prevPosition] = new ChessPiece(m_prevPosition, ChessPieceDefinition::g_chessDefinitions[(int)Type::QUEEN], m_faction, m_board);
				g_theMatch->m_board->m_piecesOnBoard[(int)m_prevPosition]->m_totalMove = 1;
			}
		}

		
		
	}
	//play sound
	if (moved) {
	    if(m_def.m_type == Type::BISHOP)
		    g_theAudio->StartSound(Game::g_Bishop);
		if (m_def.m_type == Type::KING)
			g_theAudio->StartSound(Game::g_King);
		if (m_def.m_type == Type::KNIGHT)
			g_theAudio->StartSound(Game::g_Knight);
		if (m_def.m_type == Type::ROOK)
			g_theAudio->StartSound(Game::g_Rook);
		if (m_def.m_type == Type::QUEEN)
			g_theAudio->StartSound(Game::g_Queen);
		if (m_def.m_type == Type::PAWN) {
			if (m_totalMove == 1) {
				g_theAudio->StartSound(Game::g_Pawn2);
			}
			else {
				g_theAudio->StartSound(Game::g_Pawn);
			}
		}
		    
			
	}

	return moved;
}

bool ChessPiece::CheckValidMove(Position pos) {
    
	

	IntVec2 currentPosition = GetIntVec2FromPosition(m_position);
	IntVec2 targetPosition = GetIntVec2FromPosition(pos);

	if (m_def.m_type == Type::PAWN) {
	    return CheckPawnValidMove(currentPosition, targetPosition);
	}
	if (m_def.m_type == Type::ROOK) {
		return CheckRookValidMove(currentPosition, targetPosition);
	}
	if (m_def.m_type == Type::KNIGHT) {
		return CheckKnightValidMove(currentPosition, targetPosition);
	}
	if (m_def.m_type == Type::BISHOP) {
		return CheckBishopValidMove(currentPosition, targetPosition);
	}
	if (m_def.m_type == Type::QUEEN) {
	    bool moved = CheckQueenValidMove(currentPosition, targetPosition);
		return moved;
	}
	if (m_def.m_type == Type::KING) {
	    bool moved = CheckKingValidMove(currentPosition, targetPosition);
		//Check king of the hill
		if (moved && (m_player->m_upgrades[(int)Upgrade::KING_OF_THE_HILL])) {
			if (targetPosition == IntVec2(3, 3) || targetPosition == IntVec2(3, 4) || targetPosition == IntVec2(4, 3) || targetPosition == IntVec2(4, 4)) {
				if (m_faction == Faction::WHITE) {
					g_theMatch->m_gameState = GameState::WHITE_WON;
				}
				else {
					g_theMatch->m_gameState = GameState::BLACK_WON;
				}
			}
		}
		return moved;
	}
	return false;
}

bool ChessPiece::CheckPawnValidMove(IntVec2 from, IntVec2 to)
{
 
	if (m_faction == Faction::WHITE) {
	//Case normal white move
		if (to.y ==from.y+1) {
		//Case: forward
			if (to.x == from.x) {
			    Position pos = GetPositionFromIntVec2(to);
				if (m_board->m_piecesOnBoard[(int)pos]) {
					g_theDevConsole->AddLine("Another piece at destination");
					g_theDevConsole->AddLine("Pawn may not capture forward");
					return false;
				}
				else {
					return true;
				}
			}
			//Case: Capturing
			else if (abs(to.x-from.x)==1) {
				Position pos = GetPositionFromIntVec2(to);
				if (m_board->m_piecesOnBoard[(int)pos] && m_board->m_piecesOnBoard[(int)pos]->m_faction != m_faction) {
				    return true;
				}
				//en passant
				Position side = GetPositionFromIntVec2(IntVec2(to.x,from.y));
				ChessPiece* sidePiece = m_board->m_piecesOnBoard[(int) side];
				if (sidePiece && sidePiece->m_def.m_type == Type::PAWN && sidePiece->m_faction != m_faction) {
					if (sidePiece->m_lastMovedTurn == g_theMatch->m_turns - 1 && sidePiece->m_totalMove == 1) {
						if (from.x == 3 || from.x == 4) {
							sidePiece->Captured();
							return true;
						}
					}
				}
				g_theDevConsole->AddLine("Pawn don't have piece to capture");
				return false;
			}
			else {
				g_theDevConsole->AddLine("Pawn can't move like that");
				return false;
			}
		}
		//Case: moving forward by 1 but out of range side ways
	    else {
	    	//initial move
	    	if (abs(to.y - from.y) == 2 && to.x == from.x && m_totalMove == 0) {
	    		return true;
	    	}
	    	g_theDevConsole->AddLine("Pawn can't move like that");
	    	return false;
	    }
	}
	//Case: black
	else {
		if (to.y == from.y - 1) {
			if (to.x == from.x) {
				Position pos = GetPositionFromIntVec2(to);
				if (m_board->m_piecesOnBoard[(int)pos]) {
					g_theDevConsole->AddLine("Another piece at destination");
					return false;
				}
				else {
					return true;
				}
			}
			else if (abs(to.x - from.x) == 1) {
				Position pos = GetPositionFromIntVec2(to);
				if (m_board->m_piecesOnBoard[(int)pos] && m_board->m_piecesOnBoard[(int)pos]->m_faction != m_faction) {
					return true;
				}
				//en passant
				Position side = GetPositionFromIntVec2(IntVec2(to.x, from.y));
				ChessPiece* sidePiece = m_board->m_piecesOnBoard[(int)side];
				if (sidePiece && sidePiece->m_def.m_type == Type::PAWN && sidePiece->m_faction != m_faction) {
					if (sidePiece->m_lastMovedTurn == g_theMatch->m_turns - 1 && sidePiece->m_totalMove == 1) {
						if (from.x == 3 || from.x == 4) {
							sidePiece->Captured();
							return true;
						}
					}
				}
				g_theDevConsole->AddLine("Pawn don't have piece to capture");
				return false;
			}
			else {
				g_theDevConsole->AddLine("Pawn can't move like that");
				return false;
			}
		}
		else {
			//initial move
			if (abs(to.y - from.y) == 2 && to.x == from.x && m_totalMove == 0) {
				return true;
			}
			g_theDevConsole->AddLine("Pawn can't move like that");
			return false;
		}
	}
}

bool ChessPiece::CheckRookValidMove(IntVec2 from, IntVec2 to)
{
    bool unstoppable = m_player->m_upgrades[(int)Upgrade::ROOK_UNSTOPPABLE_FORCE];
	std::vector<ChessPiece*> stompped;
	if (to.x == from.x || to.y == from.y) {
	    IntVec2 direction(to.x-from.x, to.y-from.y);
		IntVec2 checkPos = from;
		while (checkPos != to) {
			if (checkPos.x > to.x) {
			  checkPos.x--;
			}
			else if (checkPos.x < to.x) {
			    checkPos.x++;
			}
			else if (checkPos.y > to.y) {
			    checkPos.y--;
			}
			else if (checkPos.y < to.y) {
			    checkPos.y++;
			}
			if (checkPos == to) {
				ChessPiece* targetPiece = m_board->m_piecesOnBoard[(int)GetPositionFromIntVec2(to)];
				if (targetPiece) {
					if (targetPiece->m_faction != m_faction){
						for (ChessPiece* piece : stompped) {
							piece->Captured();
						}
						return true;
					}
					else {
						g_theDevConsole->AddLine("Another ally at destination");
						return false;
					}
				}
				else {
					for (ChessPiece* piece : stompped) {
						piece->Captured();
					}
					return true;
				}
			}
			else {
				Position pos = GetPositionFromIntVec2(checkPos);
				ChessPiece* blockingPiece = m_board->m_piecesOnBoard[(int)pos];
				if (blockingPiece) {
					if (blockingPiece->m_faction != m_faction && unstoppable && from.x == to.x) {
						stompped.push_back(blockingPiece);
					}
					else {
						g_theDevConsole->AddLine("Rook blocked");
						return false;
					}
					
				}
			}
		}
	}
	g_theDevConsole->AddLine("Rook only move cardinal direction");
	return false;
}


bool ChessPiece::CheckKnightValidMove(IntVec2 from, IntVec2 to)
{
	if (m_player->m_upgrades[(int)Upgrade::KNIGHT_PRINCESSKNIGHT] && m_totalMove == 0) {
		return CheckQueenValidMove(from, to);
	}

	if (to.x != from.x && to.y != from.y) {
		if (abs(to.x - from.x) + abs(to.y - from.y) == 3) {
			ChessPiece* targetPiece = m_board->m_piecesOnBoard[(int)GetPositionFromIntVec2(to)];
			if (targetPiece) {
				if (targetPiece->m_faction != m_faction)
					return true;
				else {
					g_theDevConsole->AddLine("Another ally at destination");
					return false;
				}
			}
			else {
				return true;
			}
		}
	}
	g_theDevConsole->AddLine("Knight only jump in L");
	return false;
}

bool ChessPiece::CheckBishopValidMove(IntVec2 from, IntVec2 to)
{
	
	if (abs(to.x - from.x) == abs(to.y - from.y)) {
		IntVec2 direction(to.x - from.x, to.y - from.y);
		IntVec2 checkPos = from;
		while (checkPos != to) {
			if (checkPos.x > to.x) {
				checkPos.x--;
			}
			else if (checkPos.x < to.x) {
				checkPos.x++;
			}
			if (checkPos.y > to.y) {
				checkPos.y--;
			}
			else if (checkPos.y < to.y) {
				checkPos.y++;
			}
			if (checkPos == to) {
				ChessPiece* targetPiece = m_board->m_piecesOnBoard[(int)GetPositionFromIntVec2(to)];
				if (targetPiece) {
					if (targetPiece->m_faction != m_faction)
						return true;
					else {
						g_theDevConsole->AddLine("Another ally at destination");
						return false;
					}
				}
				else {
					return true;
				}
			}
			else {
				Position pos = GetPositionFromIntVec2(checkPos);
				if (m_board->m_piecesOnBoard[(int)pos]) {
					g_theDevConsole->AddLine("Bishop blocked");
					return false;
				}
			}
		}
	}
	g_theDevConsole->AddLine("Bishop only move diagonal direction");
	return false;
}

bool ChessPiece::CheckQueenValidMove(IntVec2 from, IntVec2 to)
{
	if (abs(to.x - from.x) == abs(to.y - from.y)) {
		IntVec2 direction(to.x - from.x, to.y - from.y);
		IntVec2 checkPos = from;
		while (checkPos != to) {
			if (checkPos.x > to.x) {
				checkPos.x--;
			}
			else if (checkPos.x < to.x) {
				checkPos.x++;
			}
			if (checkPos.y > to.y) {
				checkPos.y--;
			}
			else if (checkPos.y < to.y) {
				checkPos.y++;
			}
			if (checkPos == to) {
				ChessPiece* targetPiece = m_board->m_piecesOnBoard[(int)GetPositionFromIntVec2(to)];
				if (targetPiece) {
					if (targetPiece->m_faction != m_faction)
						return true;
					else {
						g_theDevConsole->AddLine("Another ally at destination");
						return false;
					}
				}
				else {
					return true;
				}
			}
			else {
				Position pos = GetPositionFromIntVec2(checkPos);
				if (m_board->m_piecesOnBoard[(int)pos]) {
					g_theDevConsole->AddLine("Queen blocked");
					return false;
				}
			}
		}
	}

	if (abs(to.x == from.x) || abs(to.y == from.y)) {
		IntVec2 direction(to.x - from.x, to.y - from.y);
		IntVec2 checkPos = from;
		while (checkPos != to) {
			if (checkPos.x > to.x) {
				checkPos.x--;
			}
			else if (checkPos.x < to.x) {
				checkPos.x++;
			}
			else if (checkPos.y > to.y) {
				checkPos.y--;
			}
			else if (checkPos.y < to.y) {
				checkPos.y++;
			}
			if (checkPos == to) {
				ChessPiece* targetPiece = m_board->m_piecesOnBoard[(int)GetPositionFromIntVec2(to)];
				if (targetPiece) {
					if (targetPiece->m_faction != m_faction)
						return true;
					else {
						g_theDevConsole->AddLine("Another ally at destination");
						return false;
					}
				}
				else {
					return true;
				}
			}
			else {
				Position pos = GetPositionFromIntVec2(checkPos);
				if (m_board->m_piecesOnBoard[(int)pos]) {
					g_theDevConsole->AddLine("Queen blocked");
					return false;
				}
			}
		}
	}
	g_theDevConsole->AddLine("Queen only move diagonal or cardinal direction");
	return false;
}

bool ChessPiece::CheckKingValidMove(IntVec2 from, IntVec2 to)
{
	ChessPiece* targetPiece = m_board->m_piecesOnBoard[(int)GetPositionFromIntVec2(to)];
	if (abs(to.x - from.x) <= 1 && abs(to.y - from.y) <= 1 && to != from) {
		
		if (targetPiece) {
			if (targetPiece->m_faction != m_faction)
				return true;
			else {
				g_theDevConsole->AddLine("Another ally at destination");
				return false;
			}
		}
		else {
			return true;
		}
		
	}

	//Castling
	if (m_totalMove == 0 && abs(to.x-from.x)==2 && to.y == from.y) {
	    int castleX=0;
		if(to.x>from.x)
		    castleX=7;
		IntVec2 castlePos (castleX, from.y);
		ChessPiece* targetCastle=m_board->m_piecesOnBoard[(int)GetPositionFromIntVec2(castlePos)];
		if (targetCastle && targetCastle->m_totalMove == 0) {
			//check block
			int x = castleX;
			while (x != from.x) {
				if (x > from.x) {
					x--;
				}
				else {
					x++;
				}
				if (x != from.x) {
					//check space is empty
					if (g_theMatch->m_board->m_piecesOnBoard[(int)GetPositionFromIntVec2(IntVec2(x, from.y))]) {
						g_theDevConsole->AddLine("Castling path blocked ");
						return false;
					}
				}
				else {
					if (castleX > from.x) {
						g_theMatch->m_board->m_piecesOnBoard[(int)GetPositionFromIntVec2(IntVec2(4, from.y))] = targetCastle;
						g_theMatch->m_board->m_piecesOnBoard[(int)targetCastle->m_position] = nullptr;
					    targetCastle->m_position = GetPositionFromIntVec2(IntVec2(4,from.y));
					}
					else {
						g_theMatch->m_board->m_piecesOnBoard[(int)GetPositionFromIntVec2(IntVec2(2, from.y))] = targetCastle;
						g_theMatch->m_board->m_piecesOnBoard[(int)targetCastle->m_position] = nullptr;
						targetCastle->m_position = GetPositionFromIntVec2(IntVec2(2, from.y));
					}
					targetCastle->m_totalMove++;
					targetCastle->m_lastMovedTurn = g_theMatch->m_turns;
					targetCastle->m_lastMoveSecond = (float)g_gameClock->GetTotalSeconds();
					return true;
				}
			}
		}
	}
	g_theDevConsole->AddLine("King only move to adjacent or diagonal cell");
	return false;
}

void ChessPiece::Captured()
{
	if (m_def.m_type == Type::KING) {
		g_theMatch->GameOver(m_faction);
	}
	g_theMatch->m_board->m_piecesOnBoard[(int)m_position] = nullptr;
	delete this;
}



Position ChessPiece::ParsePositionFromText(std::string str)
{
    if (str.compare("A1") == 0) { return Position::A1; }
	if (str.compare("B1") == 0) { return Position::B1; }
	if (str.compare("C1") == 0) { return Position::C1; }
	if (str.compare("D1") == 0) { return Position::D1; }
	if (str.compare("E1") == 0) { return Position::E1; }
	if (str.compare("F1") == 0) { return Position::F1; }
	if (str.compare("G1") == 0) { return Position::G1; }
	if (str.compare("H1") == 0) { return Position::H1; }

	if (str.compare("A2") == 0) { return Position::A2; }
	if (str.compare("B2") == 0) { return Position::B2; }
	if (str.compare("C2") == 0) { return Position::C2; }
	if (str.compare("D2") == 0) { return Position::D2; }
	if (str.compare("E2") == 0) { return Position::E2; }
	if (str.compare("F2") == 0) { return Position::F2; }
	if (str.compare("G2") == 0) { return Position::G2; }
	if (str.compare("H2") == 0) { return Position::H2; }

	if (str.compare("A3") == 0) { return Position::A3; }
	if (str.compare("B3") == 0) { return Position::B3; }
	if (str.compare("C3") == 0) { return Position::C3; }
	if (str.compare("D3") == 0) { return Position::D3; }
	if (str.compare("E3") == 0) { return Position::E3; }
	if (str.compare("F3") == 0) { return Position::F3; }
	if (str.compare("G3") == 0) { return Position::G3; }
	if (str.compare("H3") == 0) { return Position::H3; }

	if (str.compare("A4") == 0) { return Position::A4; }
	if (str.compare("B4") == 0) { return Position::B4; }
	if (str.compare("C4") == 0) { return Position::C4; }
	if (str.compare("D4") == 0) { return Position::D4; }
	if (str.compare("E4") == 0) { return Position::E4; }
	if (str.compare("F4") == 0) { return Position::F4; }
	if (str.compare("G4") == 0) { return Position::G4; }
	if (str.compare("H4") == 0) { return Position::H4; }

	if (str.compare("A5") == 0) { return Position::A5; }
	if (str.compare("B5") == 0) { return Position::B5; }
	if (str.compare("C5") == 0) { return Position::C5; }
	if (str.compare("D5") == 0) { return Position::D5; }
	if (str.compare("E5") == 0) { return Position::E5; }
	if (str.compare("F5") == 0) { return Position::F5; }
	if (str.compare("G5") == 0) { return Position::G5; }
	if (str.compare("H5") == 0) { return Position::H5; }

	if (str.compare("A6") == 0) { return Position::A6; }
	if (str.compare("B6") == 0) { return Position::B6; }
	if (str.compare("C6") == 0) { return Position::C6; }
	if (str.compare("D6") == 0) { return Position::D6; }
	if (str.compare("E6") == 0) { return Position::E6; }
	if (str.compare("F6") == 0) { return Position::F6; }
	if (str.compare("G6") == 0) { return Position::G6; }
	if (str.compare("H6") == 0) { return Position::H6; }

	if (str.compare("A7") == 0) { return Position::A7; }
	if (str.compare("B7") == 0) { return Position::B7; }
	if (str.compare("C7") == 0) { return Position::C7; }
	if (str.compare("D7") == 0) { return Position::D7; }
	if (str.compare("E7") == 0) { return Position::E7; }
	if (str.compare("F7") == 0) { return Position::F7; }
	if (str.compare("G7") == 0) { return Position::G7; }
	if (str.compare("H7") == 0) { return Position::H7; }

	if (str.compare("A8") == 0) { return Position::A8; }
	if (str.compare("B8") == 0) { return Position::B8; }
	if (str.compare("C8") == 0) { return Position::C8; }
	if (str.compare("D8") == 0) { return Position::D8; }
	if (str.compare("E8") == 0) { return Position::E8; }
	if (str.compare("F8") == 0) { return Position::F8; }
	if (str.compare("G8") == 0) { return Position::G8; }
	if (str.compare("H8") == 0) { return Position::H8; }

	return Position::COUNT;
}

IntVec2 ChessPiece::GetIntVec2FromPosition(Position pos)
{
    int x = (int)pos%8;
	int y = ((int)pos-x)/8;
	return IntVec2(x, y);
}

Position ChessPiece::GetPositionFromIntVec2(IntVec2 pos)
{
	if (pos.x < 0 || pos.x >= 8 || pos.y < 0 || pos.y >= 8) {
		return Position::COUNT;
	}
    return (Position)(pos.x+pos.y*8);
}

Mat44 ChessPiece::GetModelToWorld() {
    Vec3 position = CalculatePosition();
	Mat44 modelToWorld = Mat44::MakeTranslation3D(position);
	modelToWorld.Append(m_orientation.GetAsMatrix_IFwd_JLeft_KUp());
	return modelToWorld;
}

Vec3 ChessPiece::CalculatePosition() 
{
    float timeSinceLastMove= (float)g_gameClock->GetTotalSeconds()- m_lastMoveSecond;
	if (timeSinceLastMove > 1.f||m_totalMove==0) {
		
		if (m_bishopBouncing) {
			m_prevPosition = m_bishopBouncePosition;
			m_lastMoveSecond = (float)g_gameClock->GetTotalSeconds();
			m_bishopBouncing=false;
		}
		int index = (int)m_position;
		float xPos = ((int)index % 8) * CellSize;
		float yPos = (((int)index - ((int)index % 8)) / 8) * CellSize;
		return Vec3(xPos + .5f, yPos + .5f, 0.f);
	}
	else {
		int index=0;
		if (m_bishopBouncing) {
			index = (int)m_bishopBouncePosition;
		}
		else {
			index = (int)m_position;
		}


		float xPos = ((int)index % 8) * CellSize;
		float yPos = (((int)index - ((int)index % 8)) / 8) * CellSize;
		
		int indexPrev = (int)m_prevPosition;
		float xPosPrev = ((int)indexPrev % 8) * CellSize;
		float yPosPrev = (((int)indexPrev - ((int)indexPrev % 8)) / 8) * CellSize;
        
		float xPosNow = xPosPrev+(xPos-xPosPrev)*timeSinceLastMove;
		float yPosNow = yPosPrev+(yPos-yPosPrev)*timeSinceLastMove;
		if (m_def.m_type == Type::KNIGHT) {
			float zPos = Lerp(0, 3, 0, timeSinceLastMove);
			return Vec3(xPosNow + .5f, yPosNow + .5f, zPos);
		}
		return Vec3(xPosNow + .5f, yPosNow + .5f, 0.f);
	}
    
}

RaycastResult3D ChessPiece::RayCastVSMe(Vec3 startPos, Vec3 fwdNormal, float maxDist)
{
	int index = (int)m_position;
	float xPos = ((int)index % 8) * CellSize +.5f;
	float yPos = (((int)index - ((int)index % 8)) / 8) * CellSize +.5f;
    RaycastResult3D result = RaycastVsZCylinder3D(startPos,fwdNormal,maxDist, Vec3(xPos,yPos,0.f), .5f, FloatRange(0.f, 1.f));
	return result;
}
