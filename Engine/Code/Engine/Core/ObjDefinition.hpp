#pragma once
#include "XmlUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Math/Mat44.hpp"
class ObjDefinition {
public:
    std::string m_objFilePath;
    std::string m_diffuse;
    std::string m_normal;
    std::string m_sge;
    std::string m_shader;
    bool m_reverseTriangle = false;
    Mat44 m_modelToWorldOrigin;

public:
    ObjDefinition(std::string const& fileName);

};