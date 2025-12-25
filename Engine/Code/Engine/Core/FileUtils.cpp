#include "Engine/Core/FileUtils.hpp"
#include <stdio.h>
#include "ErrorWarningAssert.hpp"
#include <filesystem>
#include "Engine/Core/StringUtils.hpp"
int FileReadToBuffer(std::vector<uint8_t>& outBuffer, const std::string& filename) {
	//read the file
	FILE* file;
	fopen_s(&file, filename.c_str(),"rb");
	if (!file) {
		ERROR_AND_DIE("File not found");
	}

	//find the size
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	outBuffer.resize(size);
	fseek(file, 0, SEEK_SET);
	fread(outBuffer.data(),1,size,file);
	/*int c;
	while ((c = fgetc(file)) != EOF) {
		outBuffer.push_back(c);
	}*/
	
	fclose(file);
	return size;
}
int FileReadToString(std::string& outString, const std::string& filename) {
	std::vector<uint8_t> outBuffer;
	int result=FileReadToBuffer(outBuffer, filename);
	std::string str(outBuffer.begin(), outBuffer.end());
	outString = str;
	return result;
}

int FileWriteBuffer(const std::vector<uint8_t>* outBuffer, const std::string& filename)
{
	FILE* file;
	fopen_s(&file, filename.c_str(), "wb");
	if (!file) {
		ERROR_AND_DIE("File can't be opened");
	}
	fwrite(outBuffer->data(), sizeof(uint8_t), outBuffer->size(), file);
	fclose(file);
	return (int)outBuffer->size();
}

bool DoesFileExists(const std::string& filename)
{
	return std::filesystem::exists(filename);
}

bool DoFolderExists(const std::string& filename)
{
    return std::filesystem::is_directory(filename);
}

bool CreateFolder(const std::string& filename)
{
	if (!std::filesystem::create_directory(filename)) {
		ERROR_RECOVERABLE(Stringf("Could not create folder: \"%s\"", filename.c_str()));
		return false;
	}
	return true;
}
