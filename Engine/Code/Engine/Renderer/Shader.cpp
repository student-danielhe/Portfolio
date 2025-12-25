#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Renderer.hpp"
//D11---------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN	
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")


Shader::Shader(const ShaderConfig& config):
m_config(config)
{
	
}
Shader::~Shader() {
	DX_SAFE_RELEASE(m_InputLayout);
	DX_SAFE_RELEASE(m_pixelShader);
	DX_SAFE_RELEASE(m_vertexShader);
}

const std::string& Shader:: GetName()const {
	return m_config.m_name;
}
const char* Shader::ParseShaderCodeFromFilePath(std::string path) {
	std::string filename = path;
	filename.append(".hlsl");
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open file " << filename << std::endl;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str().c_str();
}