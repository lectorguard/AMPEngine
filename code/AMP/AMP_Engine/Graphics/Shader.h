#pragma once
#include "android/asset_manager.h"
#include "GLES3/gl31.h"
#include "../AssetLoader/FilesManager.h"
#include "../AssetLoader/Attributes.h"
#include "string"

#define  IS_USED_AT_RUNTIME true

/// This struct contains the Shader Program reference and is returned when loading a shader
///
/// @note The ShaderProgram stays alive as long the scene is alive
struct ShaderAttributes : public amp::Attributes
{
	ShaderAttributes():Attributes(IS_USED_AT_RUNTIME){};
	~ShaderAttributes() override {
		if(!isSelfOrganized)glDeleteProgram(ShaderProgram);
	};
	/// The ready to use ShaderProgram
	unsigned int ShaderProgram;

	/// (De-)Activates the auto deletion process of the shader program when your scene is unloaded
	///
	/// @attention If set to true, the shader program does **not** become deleted when unloading scene => Memory Leak
	/// @attention If set to true, make sure to delete the shader program when it is not used anymore
	bool isSelfOrganized = false;
};

namespace amp
{
	/// This is the AMP Shader class
	/// 
	/// You can use this class to load ShaderPrograms
	class Shader
	{
	public:
		Shader() {};
		~Shader() {};
		Shader(const Shader&) = delete;
		Shader(Shader&&) = delete;

		/// Loads a shader program from a specific path 
		/// 
		/// @return The reference to a loaded and a ready to use shader program inside the ShaderAttributes struct
		/// @param path This is the path were the application can find your shader program
		/// @note Your shader programs can be stored under slnPath/Android/GamePackaging/assets/...
		/// @note You just need to specify the path from the assets folder e.g "Shaders/yourAwesomeShader.glsl" inside your GamePackaging assets folder
		ShaderAttributes* getShaderProgramFromPath(const char* path);
	private:
		AAssetManager* manager;
		unsigned int CreateProgram(unsigned int vertexShader, unsigned int fragmentShader);
		unsigned int CreateShader(const char* Source, GLenum Type);
		void setAssetManager(AAssetManager* manager);
		friend class AMP_Engine;
	};
}

