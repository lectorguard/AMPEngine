#pragma once
#include "Text.h"
#include "GLES3/gl31.h"
#include "../AssetLoader/GeneralAssetLoader.h"
#include "Actor.h"
#include "AMP_Engine.h"
#include "Components/Mesh.h"
#include "GLHelper.h"

namespace amp
{
	/// This is an AMP default actor
	/// 
	/// This actor allows you to easily create a skybox
	/// Call the functions from this component inside your Scene after creating the actor
	/// You should create this actor with type amp::Type::Static
	class CubeMap : public Actor
	{
	public:
		/// Constructor should be called with type amp::Type::Static
		CubeMap(amp::Type type):Actor(type){
			cube = AddComponent<Mesh>(TEXT("CubeMapComp"));
		};
		virtual ~CubeMap();
		CubeMap(const CubeMap&) = delete;
		CubeMap(CubeMap&&) = delete;

		/// Creates a skybox by 6 single images
		/// 
		/// @note An example for the method usage is here Animation::SetUp()
		/// @note You should call this function inside your Scene after creating the actor
		/// @note The names of the parameters describe the image position
		void createCubeMap(TEXT forward, TEXT backward, TEXT left, TEXT right, TEXT top, TEXT bottom);

	private:
		Mesh* cube = nullptr;
		void SetCubeMapParams();
		void CreateCubemap();
		void SetCubeMapTextures(TEXT* paths);
		void drawAfterScene();
		void BeforeSystemShutdown() override;
		virtual void OnActivate() override;
		virtual void OnDeactivate() override;

		TEXT boxObj = TEXT("3DModels/box.obj");
		TEXT boxMtl = TEXT("3DModels/box.mtl");
		TEXT shaderPath = TEXT("Shader/Skybox.glsl");

		unsigned int cubeMapBuffer = GL_NONE;
		unsigned int texID = GL_NONE;
		const int numberOfVertices = 108;

		callbacksIter postUpdate;
	
		float skyboxVertices[108] = {       
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
	};
}

