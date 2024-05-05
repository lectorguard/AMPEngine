#pragma once

#include "Extern/tiny_obj_loader.h"
#include "FilesManager.h"
#include "android/asset_manager.h"
#include "sstream"
#include "istream"
#include "vector"
#include "Extern/stb_image.h"
#include "stdlib.h"
#include "Attributes.h"
#include <string>



/// This is an asset struct containing all information of an mesh
///
/// @see Attributes for additional information
struct MeshAttributes : public amp::Attributes{
	std::vector<float> positions;
	std::vector<unsigned int> indices;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<float> originVertices;
	std::vector<int> originIndices;
	int originIndicesPerFace = 0;

	MeshAttributes() {};
	virtual ~MeshAttributes()override {
	};
};

/// This is an asset struct containing all information of a texture
///
/// @see Attributes for additional information
struct TexAttributes : public amp::Attributes{
	int width;
	int height;
	int numberOfComponents;
	unsigned char* data = nullptr;

	TexAttributes() {};

	virtual ~TexAttributes()override {
		free(data);
		data = nullptr;
	};

};

namespace amp
{
	/// This is the amp GeneralAssetLoader, you can load here general files, objs and textures 
	/// 
	/// You can call the functions from the engines 
	class GeneralAssetLoader
	{
	public:
		GeneralAssetLoader() {};
		GeneralAssetLoader(const GeneralAssetLoader&) = delete;
		GeneralAssetLoader(GeneralAssetLoader&&) = delete;

		/// Loads a wavefront format mesh
		/// 
		/// @attention Just files of type .obj can be loaded
		/// @attention When calling this function outside of the engine code, the returned MeshAttributes ptr **must** be deleted when it is not used anymore !!!
		/// @param path This is the file path of the .obj file, the path is starting after the assets folder (e.g: "Mesh/yourAwesomeMesh.obj")
		/// @return The MeshAttributes object contains the necessary information of the mesh
		MeshAttributes* loadOBJMesh(const char* path);

		
		/// Loads a wavefront format mesh
		/// 
		/// @attention Supported file formats JPEG, PNG, TGA, BMP, RLE, PSD, GIF, HDR, PIC, PNM
		/// @attention When calling this function outside of the engine code, the returned MeshAttributes ptr **must** be deleted when it is not used anymore !!!
		/// @param path This is the file path of the .obj file, the path is starting after the assets folder (e.g: "Mesh/yourAwesomeMesh.obj")
		/// @param flip If true, the texture becomes flipped by the x axis
		/// @return The MeshAttributes object contains the necessary information of the mesh
		TexAttributes* loadTextureFromImage(const char* path, bool flip = false);

		/// Loads a file of any format
		/// 
		/// @attention Make sure to close the file again, when you are finished using it
		/// @param path This is the file path starting after the assets folder (e.g: "Files/yourAwesomeFile.niceFileFormat")
		/// @return The file at the specified path
		FILE* getRawFile(const char* path);

		/// Loads just the vertices and indices by reading in the .obj format by hand
		/// 
		/// @attention Just files of type .obj can be loaded
		/// @attention When calling this function outside of the engine code, the returned MeshAttributes ptr **must** be deleted when it is not used anymore !!!
		/// @param path This is the file path of the .obj file, the path is starting after the assets folder (e.g: "Mesh/yourAwesomeMesh.obj")
		/// @param meshAttr Contains the vertices and indices as they are inside the obj file
		/// @note This method is used to set custom collision shapes of rigid-bodies
		void readOBJVerticesAndIndices(const char* path, MeshAttributes* meshAttr);

		/// Splits a string by a delimeter
		/// 
		/// @param str This is the input string
		/// @param delim This is the splitting delimeter
		/// @param out This is the output vector of the result strings
		void split(std::string const& str, const char* delim, std::vector<std::string>& out);
	
	private:
		AAssetManager* manager = nullptr;
		void setAssetManager(AAssetManager* manager) { this->manager = manager; };
		MeshAttributes* readOBJ(std::istringstream& OBJStream, const char* path);
		friend class AMP_Engine;
	};
}

