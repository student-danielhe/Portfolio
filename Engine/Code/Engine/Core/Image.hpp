#pragma once
#include "Engine/Renderer/Renderer.hpp"
class Image {
	friend class Renderer;
public:
	Image() {};
	~Image() {};
	Image(const char* imageFilePath);
	Image(IntVec2 size, Rgba8 color);

	IntVec2 GetDimensions()const;
	const std::string& GetImageFilePath() const;
	const void* GetRawData()const;
	int GetBytePerPixel()const;
	std::vector<Rgba8> GetPixelData()const;

private:
	std::string m_imageFilePath;
	IntVec2 m_dimesions;
	std::vector<Rgba8> m_texelRgba8Data;
	int m_bytesPerPixel=4;
};