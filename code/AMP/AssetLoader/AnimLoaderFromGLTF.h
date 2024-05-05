#pragma once
#include "Extern/tiny_gltf.h"
#include "android/asset_manager.h"
#include "vector"
#include "string"
#include "functional"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "Attributes.h"
#include "array"

/// This is the maximum number of joints you skeletal animation can have
///
/// You can change this value, but when you do so, change also the value inside the shader !!!
constexpr unsigned int MAX_NUMBER_JOINTS = 64u;

/// These are the different interpolation types for animations
///
/// @note At the moment just the linear interpolation type is supported
enum InterpType {
	STEP, LINEAR, CUBICSPLINE
};

/// These are different path types stored inside the gltf format
enum PathType {
	TRANSLATION,ROTATION,SCALE,WEIGHTS
};

/// Nodes become constructed from the gltf nodes, and are containing all gltf specific node information
///
/// The gltf format is build like a graph and contains many nodes
struct Node {
	std::vector<int> childs;
	int ID = -1;
	int parentID = -1;
	int meshID = -1;
	int skinID = -1;
	std::string name;
	glm::vec3 translate = glm::vec3(0.0f);
	glm::quat rotate = {};
	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 matrix = glm::identity<glm::mat4>();
};

/// Textures become constructed from the gltf textures, and are containing all gltf specific texture information
///
/// These are the texture information loaded from a gltf file. At the moment just one texture is supported
struct Texture {
	//Image
	int width;
	int height;
	int components;
	std::vector<unsigned char> data;
	//Texture
	int MinFilter;
	int MagFilter;
	int wrapR;
	int wrapS;
	int wrapT;
};

/// Skins become constructed from the gltf skins, and are containing all gltf specific skin information
///
/// The skin contains all bones and inverse bind matrices to calculate the vertex transformations
struct Skin
{
	std::vector<int> joints;
	int SkeletonID;
	std::vector<glm::mat4> inverseBindMatrices;
};

/// Channels become constructed from the gltf channels, and are containing all gltf specific channel information
///
/// The channel just defines the type of the data inside the gltf format
struct Channel {
	PathType pathType;
	int node_ID = -1;
	int Sampler_ID = -1;
};

/// Samplers become constructed from the gltf samplers, and are containing all gltf specific sampler information
/// 
/// The sampler contains the information about the different key-frames ant the timings
struct Sampler {
	std::vector<float> inputTime;
	std::vector<float> output;
	InterpType interpType;
};

/// Animations become constructed from the gltf animations, and are containing all gltf specific animation information
/// 
/// Contains all necessary information of one animation like key-frames, timings, transformations, ...
struct Anim {
	std::vector<Sampler> Samplers;
	std::vector<Channel> Channels;
	std::string name;
	float startTime = 10000000000.0f;
	float endTime = -1.0f;
};


/// Meshes become constructed from the gltf meshes, and are containing all gltf specific mesh information
/// 
/// Contains the mesh information and also the skeleton
struct MeshInfo {
	std::vector<unsigned int> indices;
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;
	std::vector<float> weights;
	std::vector<unsigned int> joints;
	std::array<glm::mat4,MAX_NUMBER_JOINTS> jointMatrices;
	glm::mat4 matrix = glm::mat4(1.0f);
};


/// AnimationAttrib is the AMP container in which animations become stored
/// 
/// Contains all information of one .glb file
struct AnimationAttrib : public amp::Attributes{
	Texture texture;
	Skin skin;
	MeshInfo mesh;
	std::vector<Node> Nodes;
	std::vector<Anim> Animations;
	float duration;
	std::string name;
	//Mesh
	
	int materialID;
	bool isIndexed = false;

	int AnimationID = 1;
	int StartNode;

};

namespace amp
{

	/// This is the AMP animation loader
	/// 
	/// This class is the loader for the animations
	/// The animations are player inside the Animation component
	class AnimLoaderFromGLTF
	{
	
	public:
		AnimLoaderFromGLTF();
		AnimLoaderFromGLTF(AnimLoaderFromGLTF&&) = delete;
		AnimLoaderFromGLTF(const AnimLoaderFromGLTF&) = delete;
	
		/// Loads a binary gltf animation (.glb)
		/// 
		/// @attention Just files of type .glb can be loaded
		/// @attention When calling this function outside the engine code, the returned AnimationAttrib ptr must be deleted when it is not used anymore !!!
		/// @note It is recommended to load the texture additional on your own using the MeshLoaderFromObj
		/// @param path This is the file path of the .glb file, the path is starting after the assets folder (e.g: "Animations/yourAwesomAnimation.glb")
		/// @param loadTex If set to true, a texture from the gltf file is loaded with the animation
		/// @return The AnimationAttrib object containing the necessary information to play the animation(s)
		AnimationAttrib* loadAnimation(const char* path, bool loadTex = false);

	private:
		void LoadNodes(tinygltf::Model& model, tinygltf::Node& gltf_Node, AnimationAttrib* anim, int ID, int parentID =-1);

		void loadGLB(tinygltf::Model& model, const char* path);

		void SetSkin(tinygltf::Model& model, AnimationAttrib* anim);

		void LoadTexture(tinygltf::Model& model, AnimationAttrib* anim);

		void SetAnimChannels(tinygltf::Model& model, AnimationAttrib* anim, int i);

		void SetAnimInputAndOutputValues(tinygltf::Model& model, AnimationAttrib* anim, int i);

		void SetAttributes(tinygltf::Model& model, AnimationAttrib* anim);

		void SetIndices(tinygltf::Model& model, AnimationAttrib* anim);

		//I is the Input type
		//T is the Target type
		template<typename I,typename T>
		void FillWithScalar(std::vector<T>& vectorToFill, int count, std::function<const unsigned char* (int)> getTargetAdress)
		{
			for (int i = 0; i <count; ++i)
			{
				vectorToFill.emplace_back(*(reinterpret_cast<const I*>(getTargetAdress(i))));
			}
		}

		template<typename I, typename T>
		void FillWithVectors(std::vector<T>& vectorToFill,int size, int count, std::function<const unsigned char* (int,int)> getTargetAdress,std::function<void()> extra = nullptr)
		{
			for (int i = 0; i < count; ++i)
			{
				for (int k = 0; k < size; ++k)
				{
					vectorToFill.emplace_back(*(reinterpret_cast<const I*>(getTargetAdress(i,k))));
				}
				if(extra)extra();
			}
		}

		PathType GetPathType(std::string type);
		int NumberOfElementsInType(std::string type, int accessorType);
		void SetByType(AnimationAttrib* ptr, std::string type, std::vector<float>& data);
		InterpType getInterpType(std::string type);
		AAssetManager* manager = nullptr;
		void setAssetManager(AAssetManager* manager);
		friend class AMP_Engine;
	};
}

