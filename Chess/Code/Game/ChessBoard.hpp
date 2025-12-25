#pragma once
#include "GameCommon.hpp"
#include <vector>
#include "ChessPiece.hpp"
#include "Game.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/Camera.hpp"
class ChessMatch;
class ChessBoard {
public:
    Camera* m_whiteCamera = nullptr;
    Camera* m_blackCamera = nullptr;

    std::vector<ChessPiece*> m_piecesOnBoard;
    ChessMatch* m_owner = nullptr;
    
    VertexBuffer* m_vertexBuffer=nullptr;
    IndexBuffer* m_indexBuffer = nullptr;
    std::vector<Vertex_PCUTBN> m_vertexes;
    std::vector<unsigned int> m_indexes;
    
    Shader* m_diffuseShader = nullptr;

    Texture* m_texture = nullptr;
    Texture* m_normalMap = nullptr;
    Texture* m_SGE = nullptr;

    IntVec2 m_targetedCell =IntVec2(-1, -1);
    ChessPiece* m_selectedPiece = nullptr;

    bool m_cheating=false;
    void StartUp();
    void Render();
    void Update();
    bool TryMovePiece(Position piecePos, Position destination, Type promoteTo=Type::COUNT, bool teleport = false);
    ChessBoard(ChessMatch* owner);
    ~ChessBoard();
    void InitializePieces();
    void InitializeGeometry();
    void CreateBuffers();
    void PrintBoardState();
    std::string GetBoardStateForValidation();
    static Type ParseTypeString(std::string str);

    IntVec2 RayCastVSBoard(Vec3 startPos, Vec3 fwdNormal, float maxDist, RaycastResult3D& out_result);
    void RenderHighLighted(IntVec2 cell);

    void HandleKeyPressed();
};