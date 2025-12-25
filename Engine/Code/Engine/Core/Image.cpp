#include"Engine/Core/Image.hpp"
#define STB_IMAGE_IMPLEMENTATION // Exactly one .CPP (this Image.cpp) should #define this before #including stb_image.h
#include "ThirdParty/stb/stb_image.h"

Image::Image(const char* imageFilePath)
	:m_imageFilePath(imageFilePath)
{
	stbi_set_flip_vertically_on_load(true);
	unsigned char* texelData=stbi_load(imageFilePath, &m_dimesions.x, &m_dimesions.y, &m_bytesPerPixel, 0);
	int i = 0;
	int j = 0;
	m_texelRgba8Data.resize(m_dimesions.x * m_dimesions.y);
	while (j < m_dimesions.x*m_dimesions.y) {
		Rgba8 color(texelData[i], texelData[i + 1], texelData[i + 2], m_bytesPerPixel==3?255:texelData[i + 3]);
		m_texelRgba8Data[j] = color;
		i += m_bytesPerPixel;
		j++;
	}
}
Image::Image(IntVec2 size, Rgba8 color)
	:m_dimesions(size)
{
	m_texelRgba8Data.resize(size.x * size.y);
	for (Rgba8 c : m_texelRgba8Data) {
		c = color;
	}
}

IntVec2 Image::GetDimensions()const {
	return m_dimesions;
}
const std::string& Image::GetImageFilePath() const {
	return m_imageFilePath;
}
const void* Image::GetRawData()const {
	return reinterpret_cast<const void*>(m_texelRgba8Data.data());
}

int Image::GetBytePerPixel()const {
	return m_bytesPerPixel;
}

std::vector<Rgba8> Image::GetPixelData()const {
	return m_texelRgba8Data;
}