#pragma once
#include "GameCommon.hpp"
#include <vector>
#include "Engine/Renderer/Renderer.hpp"
enum class Type {
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING,
	COUNT
};
class ChessPieceDefinition {
public:
	static std::vector<ChessPieceDefinition> g_chessDefinitions;
    static void InitializeDefinitions();
	static void ClearDefinition();
	void Initialize();
	Type m_type;

	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;
	std::vector<Vertex_PCUTBN> m_verts;
	std::vector<unsigned int> m_indexes;

	VertexBuffer* m_vertexBuffer2 = nullptr;
	IndexBuffer* m_indexBuffer2 = nullptr;
	std::vector<Vertex_PCUTBN> m_verts2;
	std::vector<unsigned int> m_indexes2;

	Texture* m_texture = nullptr;
	Texture* m_normalMap = nullptr;
	Texture* m_SGE = nullptr;
};

