#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"

void ParseObjFromFile(std::string const& filename, std::vector<Vertex_PCUTBN>& out_vertexes, std::vector<unsigned int>& out_indexes, bool reverse = false);

std::vector<std::string> RemoveExtraLines(std::vector<std::string>& lines);

void ParseVertexArray(std::vector<std::string>const& lines, std::vector<Vertex_PCUTBN>& out_verts, std::vector<unsigned int>& out_index, bool reverse = false);

void AddVertsForFace(std::vector<std::string>const& faces, std::vector<Vec3>const& positions, std::vector<Vec2>const& UVs, std::vector<Vec3>const& normals,
std::vector<Vertex_PCUTBN>& out_verts, std::vector<unsigned int>& out_index, bool reverse = false);
