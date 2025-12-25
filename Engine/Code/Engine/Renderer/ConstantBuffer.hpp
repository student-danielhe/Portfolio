#pragma once
struct ID3D11Buffer;
class ConstantBuffer
{
	friend class Renderer;
public:
	ConstantBuffer(unsigned int size);
	ConstantBuffer(const ConstantBuffer& copy) = delete;
	virtual ~ConstantBuffer();

private:
	ID3D11Buffer* m_buffer = nullptr;
	unsigned int m_size = 0;
};