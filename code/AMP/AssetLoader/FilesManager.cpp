
#include "FilesManager.h"


char* amp::FilesManager::readAssetFile(AAssetManager* manager,const char* path, int mode /*= AASSET_MODE_BUFFER*/)
{
	if (manager == nullptr)throw "No AssetManager, Initialization Error";
 	AAsset* file = AAssetManager_open(manager, path, mode);
	if (file == nullptr)throw "No valid file path";
 	size_t fileLength = AAsset_getLength(file);
	char* fileContent = (char*)malloc(sizeof(char) * (fileLength+1));
 	AAsset_read(file, fileContent, fileLength);
 	fileContent[fileLength] = '\0';
 	AAsset_close(file);
	return fileContent;
}



void amp::FilesManager::readShaderFile(AAssetManager* manager, const char* path, std::string& vertexFile, std::string& fragmentFile)
{
	char* fileArr = readAssetFile(manager,path);
	const std::string fileStr(fileArr);
	std::istringstream ss(fileStr);
	std::stringstream typeArray[2];
	int Type = 0;
	for (std::string singleLine;std::getline(ss, singleLine);) {
		if (singleLine.find("#VERTEX") != std::string::npos) {
			Type = 0;
			continue;
		}
		else if (singleLine.find("#FRAGMENT") != std::string::npos) {
			Type = 1;
			continue;
		}
		if (Type == -1)throw std::runtime_error(std::string("Shader files have to start with #VERTEX") + path);
		typeArray[(int)Type] << singleLine << '\n';
	}
	vertexFile = typeArray[0].str();
	fragmentFile = typeArray[1].str();
	free(fileArr);
	fileArr = nullptr;
}

FILE* amp::FilesManager::readImageFile(AAssetManager* manager, const char* path)
{
	if (manager == nullptr)throw "No AssetManager Initialization Error";
	AAsset* asset = AAssetManager_open(manager, path, 0);
	if (!asset)throw "Could not open asset";
	return funopen(asset, android_read, android_write, android_seek, android_close);
}








