#include "VFX.hpp"
void VFX::Update()
{
    if (m_timer->HasPeriodElapsed()) {
        m_destroyed = true;
    }
}

void VFX::Render()
{
    g_theRenderer->BindTexture(m_texture);
    g_theRenderer->DrawVertexArray(m_verts);
}

