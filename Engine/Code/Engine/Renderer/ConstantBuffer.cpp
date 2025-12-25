#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
//D11---------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN	
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

ConstantBuffer::ConstantBuffer(unsigned int size):
m_size(size)
{

}
ConstantBuffer::~ConstantBuffer() {
	DX_SAFE_RELEASE(m_buffer);
}