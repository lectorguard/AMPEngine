#pragma once
#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>
#include "GLES3/gl3platform.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "Component.h"
#include "../AssetLoader/GeneralAssetLoader.h"
#include "Text.h"
#include "Static/Camera.h"
#include "Graphics/Shader.h"
#include "AMP_Engine.h"
#include "functional"
#include "Graphics/GLHelper.h"

namespace amp
{

	/// This is an AMP default component
	/// 
	/// This component renders 3D meshes from 
	/// Call the functions from this component inside the Actor class
	class Mesh : public Component
	{
	public:
		Mesh(){};
		Mesh(Mesh&&) = delete;

		Mesh(const Mesh&) = delete;

		virtual ~Mesh();

		/// Setting up a mesh from an obj file
		/// 
		/// @param MeshAttributesPath The mesh file path to your .obj 
		/// @param TexturePath The texture file path to your .png or .jpg or ...
		/// @note Here you can find an example for calling these function and additional information abour the asset folder
		/// @see Animation::SetUp()
		void Mesh_SetUpGL(TEXT MeshAttributesPath,TEXT TexturePath);

		/// You can display your mesh with a custom shader, as an inspiration you can look at the default shader (assets/Shader/MeshShader.glsl)
		///
		/// @note Should be called **before** Mesh_SetUpGL()
		void SetShaderPath(TEXT path) { ShaderPath = path; };

		/// (De-)Activates alpha blending
		void SetBlend(bool blend) { isBlend = blend; };

		/// You can set here a custom draw callback which extends the default draw callback
		///
		/// @param cb The callback function signature : myExtendDraw(unsigned int ShaderProgram)
		void SetDrawCallback(std::function<void(unsigned int)> cb) { drawCallback = cb; };

		/// Resets the custom draw callback btw. stops the callback
		void RemoveDrawCallback() { drawCallback = nullptr; };

	protected:
		 void update() override;

	private:
		MeshAttributes* LoadMesh(TEXT MeshAttributesPath);
		void SetAttributes(MeshAttributes* meshAttr);
		void LoadShader(TEXT ShaderProgramPath);
		void LoadTexture(TEXT TexturePath);
		void draw();
		
		GLuint indexBuffer = GL_NONE;
		GLuint positionBuffer = GL_NONE;
		GLuint texCoordBuffer = GL_NONE;
		GLuint normalBuffer = GL_NONE;
		GLuint texture  = GL_NONE;
		GLuint TexID = GL_NONE;

		unsigned int ShaderProgram = GL_NONE;
		TEXT ShaderPath = TEXT("Shader/MeshShader.glsl");
		TEXT MeshAttributesPath;
		GLsizei NumIndices = 0;
		bool isDrawEnabled = true;
		bool isBlend = false;

		std::function<void(unsigned int)> drawCallback = nullptr;

		friend class DynamicMesh;
		friend class DynamicConcaveMesh;
		friend class CubeMap;
	};
}

