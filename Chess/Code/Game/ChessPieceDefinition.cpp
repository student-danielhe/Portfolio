#include "ChessPieceDefinition.hpp"
#include "Engine/Core/VertexUtils.hpp"
std::vector<ChessPieceDefinition>ChessPieceDefinition::g_chessDefinitions = std::vector<ChessPieceDefinition>((int)Type::COUNT);

void ChessPieceDefinition::InitializeDefinitions()
{
    g_chessDefinitions = std::vector<ChessPieceDefinition>((int)Type::COUNT);
    for (int i = 0; i < (int)Type::COUNT; i++) {
        g_chessDefinitions[i].m_type = (Type)i;
        g_chessDefinitions[i].Initialize();
    }
}

void ChessPieceDefinition::ClearDefinition()
{
    for (ChessPieceDefinition def : g_chessDefinitions) {
		delete def.m_vertexBuffer;
		def.m_vertexBuffer = nullptr;
		delete def.m_vertexBuffer2;
		def.m_vertexBuffer2 = nullptr;
		delete def.m_indexBuffer;
		def.m_indexBuffer = nullptr;
		delete def.m_indexBuffer2;
		def.m_indexBuffer2 = nullptr;

    }

}

void ChessPieceDefinition::Initialize()
{
    
    std::vector<Vertex_PCUTBN> commonVerts;
    std::vector<unsigned int> commonIndexes;
    if (m_type == Type::PAWN) {
        AddVertsForZCylinder3D(commonVerts, commonIndexes, Vec3(0.f, 0.f, .1f), .9f, .3f);
        AddVertsForSphere3D(commonVerts,commonIndexes,Vec3(0.f,0.f,1.3f),.3f);
    }

    if (m_type == Type::ROOK) {
        AddVertsForAABB3D(commonVerts,commonIndexes,AABB3(Vec3( - .3f, -.3f, .1f), Vec3(.3f,.3f, 1.f)));
        AddVertsForAABB3D(commonVerts, commonIndexes, AABB3(Vec3(-.4f, -.4f, 1.f), Vec3(.4f, .4f, 1.4f)));
    }
    
    if (m_type == Type::KNIGHT) {
        AddVertsForAABB3D(commonVerts, commonIndexes, AABB3(Vec3(-.3f, -.3f, .1f), Vec3(.3f, .3f, 1.f)));
        AddVertsForAABB3D(commonVerts, commonIndexes, AABB3(Vec3(-.2f, .3f, .7f), Vec3(.2f, .5f, .9f)));
    }

    if (m_type == Type::BISHOP) {
        AddVertsForZCylinder3D(commonVerts, commonIndexes, Vec3(0.f, 0.f, .1f), .9f, .3f);
        AddVertsForSphere3D(commonVerts, commonIndexes, Vec3(0.f, 0.f, 1.3f), .3f);
        AddVertsForSphere3D(commonVerts, commonIndexes, Vec3(0.f, 0.f, 1.6f), .1f);
    }

    if (m_type == Type::QUEEN) {
        AddVertsForZCylinder3D(commonVerts, commonIndexes, Vec3(0.f, 0.f, .1f), .9f, .3f);
		AddVertsForSphere3D(commonVerts, commonIndexes, Vec3(0.f, 0.f, 1.3f), .3f);
		AddVertsForSphere3D(commonVerts, commonIndexes, Vec3(0.f, 0.f, 1.6f), .1f);
        AddVertsForSphere3D(commonVerts, commonIndexes, Vec3(0.f, 0.f, 1.75f), .1f);
    }

    if (m_type == Type::KING) {
        AddVertsForZCylinder3D(commonVerts, commonIndexes, Vec3(0.f, 0.f, .1f), .9f, .3f);
        AddVertsForAABB3D(commonVerts, commonIndexes, AABB3(Vec3(-.4f, -.4f, 1.f), Vec3(.4f, .4f, 1.4f)));
        AddVertsForAABB3D(commonVerts, commonIndexes, AABB3(Vec3(-.2f, -.2f, 1.4f), Vec3(.2f, .2f, 1.6f)));
    }


    m_verts = commonVerts;
    m_verts2 = commonVerts;
    m_indexes = commonIndexes;
    m_indexes2 = commonIndexes;

	AddVertsForZCylinder3D(m_verts, m_indexes, Vec3(), .1f, .4f);
	AddVertsForAABB3D(m_verts2, m_indexes2, AABB3(Vec3(-.4f, -.4f, 0.f), Vec3(.4f, .4f, .1f)));
	
    m_indexBuffer = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));
    m_indexBuffer2 = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));
    m_vertexBuffer = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
    m_vertexBuffer2 = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
    g_theRenderer->CopyIndexedVertexCPUTBNToBuffers(m_verts,m_indexes,m_vertexBuffer,m_indexBuffer);
    g_theRenderer->CopyIndexedVertexCPUTBNToBuffers(m_verts2, m_indexes2, m_vertexBuffer2, m_indexBuffer2);

    m_texture = nullptr;
    m_normalMap = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Cobblestone_n.png");
    m_SGE = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Cobblestone_sge.png");

}
