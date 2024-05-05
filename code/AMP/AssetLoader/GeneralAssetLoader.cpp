#include "GeneralAssetLoader.h"




MeshAttributes* amp::GeneralAssetLoader::loadOBJMesh(const char* path)
{
	if (manager == nullptr)throw "Asset Manager inside GeneralAssetLoader is not set";
	char * textOBJ = amp::FilesManager::readAssetFile(manager,path);
	const std::string OBJString(textOBJ);
	std::istringstream OBJStream(OBJString);

	auto meshAttrib = readOBJ(OBJStream, path);
	
	//For collision creation
	readOBJVerticesAndIndices(path,meshAttrib);
	return meshAttrib;
}

MeshAttributes* amp::GeneralAssetLoader::readOBJ(std::istringstream& OBJStream, const char* path)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	{
		std::string errors;
		std::string warnings;
		//tinyobj::MaterialStreamReader mtlReader(MTLStream);
		//bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnings, &errors, &OBJStream, &mtlReader);
		bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnings, &errors, &OBJStream);
		if (!success) {
			throw std::runtime_error("Failed to load file (" + std::string(path) + "): " + errors + " " + warnings);
		}

		//Warning coming up when there is no MTL file, but mtl file is not necessary
// 		if (!warnings.empty()) {
// 			throw std::runtime_error("Warning happend when loading obj " + warnings);
// 		}
	}
	auto mesh = shapes[0].mesh;
	MeshAttributes* meshAttrib = new MeshAttributes();

	// https://github.com/syoyo/tinyobjloader/blob/master/tiny_obj_loader.h#L323
	const size_t TINYOBJ_NOT_USED = -1;

	for (size_t v = 0; v < mesh.indices.size(); v++) {
		// Note: This throws away any vertex reuse.
		// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-9-vbo-indexing/ 
		meshAttrib->indices.push_back((unsigned int)v);

		{
			int vIndex = mesh.indices[v].vertex_index;
			meshAttrib->positions.push_back(attrib.vertices[3 * vIndex + 0]);
			meshAttrib->positions.push_back(attrib.vertices[3 * vIndex + 1]);
			meshAttrib->positions.push_back(attrib.vertices[3 * vIndex + 2]);
		}


		{
			int nIndex = mesh.indices[v].normal_index;
			if (nIndex != TINYOBJ_NOT_USED) {
				meshAttrib->normals.push_back(attrib.normals[3 * nIndex + 0]);
				meshAttrib->normals.push_back(attrib.normals[3 * nIndex + 1]);
				meshAttrib->normals.push_back(attrib.normals[3 * nIndex + 2]);
			}
		}

		{
			int tIndex = mesh.indices[v].texcoord_index;
			if (tIndex != TINYOBJ_NOT_USED) {
				meshAttrib->texCoords.push_back(attrib.texcoords[2 * tIndex + 0]);
				meshAttrib->texCoords.push_back(1.0f - attrib.texcoords[2 * tIndex + 1]);
			}
		}
	}
	return meshAttrib;
}

TexAttributes* amp::GeneralAssetLoader::loadTextureFromImage(const char* path, bool flip /*= false*/)
{
	if (manager == nullptr)throw "Asset Manager not Initialized for Mesh Loader From Obj";
	FILE* file = FilesManager::readImageFile(manager, path);
	TexAttributes* retTexAttrib = new TexAttributes();
	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load_from_file(file, &retTexAttrib->width, &retTexAttrib->height, &retTexAttrib->numberOfComponents, 0);
	if (!data)throw "File could not be loaded";
	retTexAttrib->data = data;
	fclose(file);
	return retTexAttrib;
}

FILE* amp::GeneralAssetLoader::getRawFile(const char* path)
{
	return  FilesManager::readImageFile(manager, path);
}

void amp::GeneralAssetLoader::readOBJVerticesAndIndices(const char* path, MeshAttributes* meshAttr)
{
	if (manager == nullptr)throw "Asset Manager inside GeneralAssetLoader is not set";
	char* text = amp::FilesManager::readAssetFile(manager, path);
	const std::string fileStr(text);
	std::istringstream ss(fileStr);
	std::vector<float> vertices;
	std::vector<int> indices;
	int indicesPerRow = 0;
	int row = 0;
	for (std::string singleLine; std::getline(ss, singleLine);) {
		++row;
		if (row < 5)continue;
		if (singleLine.find("s") != std::string::npos)continue;
		if (singleLine.find("vt") != std::string::npos)continue;
		if (singleLine.find("vn") != std::string::npos)continue;
		if (singleLine.find("v") != std::string::npos) {
			std::vector<std::string> result;
			split(singleLine," ", result);
			result.erase(result.begin());
			for (auto& v : result) {
				std::string::size_type sz;
				vertices.push_back(std::stof(v, &sz));
			}
		}
		else if (singleLine.find("f") != std::string::npos) {
			std::vector<std::string> result;
			split(singleLine, " ", result);
			result.erase(result.begin());
			indicesPerRow = result.size();
			for (auto& v : result) {
				std::vector<std::string> elem;
				split(v, "/", elem);
				//expected first number is 0
				if (elem[0] == "\r" || elem[0] == "\n")continue;
				if(elem.size() > 0)indices.push_back(std::stoi(elem[0])-1);
			}
		}
	}
	meshAttr->originIndices = indices;
	meshAttr->originVertices = vertices;
	meshAttr->originIndicesPerFace = indicesPerRow;
}

void amp::GeneralAssetLoader::split(std::string const& str, const char* delim, std::vector<std::string>& out)
{
	char* token = strtok(const_cast<char*>(str.c_str()),delim);
	while(token != nullptr){
		out.push_back(std::string(token));
		token = strtok(nullptr, delim);
	}
}
