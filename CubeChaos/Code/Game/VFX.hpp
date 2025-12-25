#pragma once
#include "Engine/Core/Timer.hpp"
#include "GameCommon.hpp"
class VFX {
public:
    std::vector<Vertex_PCU> m_verts;
    Timer* m_timer;
    bool m_destroyed=false;
    Texture* m_texture;
    void Update();
    void Render();
};