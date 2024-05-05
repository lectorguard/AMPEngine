#pragma once
#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>
#include "GLES3/gl3platform.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "string"
#include "GLHelper.h"
#include "functional"


namespace amp
{
	/// The frame buffer shader allows you to set a shading type
	///
	/// @attention The default frame buffer shader just supports the **default** shading type
	/// @note You can use your own shader as well, and use also 
	enum ShadingType {
		DEFAULT			= 0,
		CUSTOM			= 1
	};
	
	class AMP_Engine;

	/// This is the AMP frame buffer
	/// 
	/// This class allows the creation of frame buffers and shading of these buffers (post processing)
	class FrameBuffer
	{
	public:
		FrameBuffer() {};
		~FrameBuffer();

		FrameBuffer(const FrameBuffer& f) = delete;
		FrameBuffer(FrameBuffer&& f) = delete;

		/// You can change the default shader for the frame buffer 
		///
		/// By changing the shader you can create your own post processing shader
		/// @attention You should call this function inside your games AfterWindowStartup function 
		/// @note After the shader is loaded, changing the shader has no effect anymore
		/// @note The shader is loaded after the AfterWindowStartup method of the game
		void setShaderPath(std::string path);

		/// Changes the current shading type
		/// 
		/// You can change the shading type everywhere in your game as long you have a valid engine reference
		/// @note The default shading type is ShadingType::DEFAULT
		void setShadingType(ShadingType type);

		/// @return The current shading type
		ShadingType getShadingType();

		/// When your custom shader needs additional uniforms or buffers you can set them in this callback function
		/// 
		/// @param drawCb The function is called in each frame and has the following signature: myCustomPostProcessDrawCallBack(unsigned int ShaderProgram)
		void setDrawCallback(std::function<void(unsigned int)> drawCb) {
			drawCallback = drawCb;
		};

		/// Resets the draw callback, stops the callback from being called
		void resetDrawCallback() {
			drawCallback = nullptr;
		}

	private:
		void enable(AMP_Engine* engine);
		void SetFragmentShaderFastest();
		void CreateFrameRenderBuffer();
		void SetRenderBuffer();
		void CreateRenderBuffer();
		void CreateFrameBuffer();
		void CreateFrameBufferTexture();
		void GetShader();
		void clear(bool isLowestLayer = false);
		void drawFrameBufferContent();
		void EnableDepthTesting();
		void DisableBlend();
		void BindFrameBuffer();
		void EnableBlend();
		void DisableDepthTesting();



		std::string shaderPath = "Shader/DefaultFrameBufferShader.glsl";
		ShadingType shadingType = ShadingType::DEFAULT;
		AMP_Engine* engine = nullptr;

		unsigned int mainFramebuffer = GL_NONE;
		unsigned int texColorBuffer = GL_NONE;
		unsigned int rbo = GL_NONE;
		unsigned int vertexBuffer = GL_NONE;
		unsigned int uvBuffer = GL_NONE;
		unsigned int shaderProgram = GL_NONE;
		unsigned int TextureID;

		glm::vec2 verticesArray[6] = {
			glm::vec2(-1, 1),  //top-left
			glm::vec2(-1,-1),  //bottom-left
			glm::vec2( 1, 1),  //top-right

			glm::vec2( 1,-1),  //bottom-right
			glm::vec2( 1, 1),  //top-right
			glm::vec2(-1,-1)   //bottom-left
		};

		glm::vec2 uvsArray[6] = {
			glm::vec2(0,1), //top-left
			glm::vec2(0,0),	//bottom-left
			glm::vec2(1,1),	//top-right

			glm::vec2(1,0),	//bottom-right
			glm::vec2(1,1),	//top-right
			glm::vec2(0,0)	//bottom-left
		};
		std::function<void(unsigned int)> drawCallback = nullptr;
		friend class AMP_Engine;
	};
}

