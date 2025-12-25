#pragma once
#include <vector>
#include <string>
int FileReadToBuffer(std::vector<uint8_t>& outBuffer, const std::string& filename);
int FileReadToString(std::string& outString, const std::string& filename);

int FileWriteBuffer(const std::vector<uint8_t>* outBuffer, const std::string& filename);
bool DoesFileExists(const std::string& filename);

bool DoFolderExists(const std::string& filename);
bool CreateFolder(const std::string& filename);