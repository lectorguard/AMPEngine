#pragma once
#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>
#include "GLES3/gl3platform.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../AssetLoader/GeneralAssetLoader.h"
#include "../AssetLoader/AnimLoaderFromGLTF.h"

namespace GLHelper {

	/// Buffer Information
	///
	/// Simple struct storing buffer information
	struct BufferInfo {
		BufferInfo(int dataSize) : size(dataSize) {};
		int index = 0;
		int size = 0;
		unsigned int buffer = GL_NONE;
	};

	/// GL Error Check
	static void ErrorCheck() {
		GLenum error = glGetError();
		assert(error == GL_NONE);
	}

	/// Sets the content of a buffer
	/// 
	/// @param data The data you want to fill in your buffer
	/// @param counter The number of data elements
	/// @param type The draw type of your buffer
	template<typename T>
	static void SetBufferData(const T* data, size_t count, GLenum type = GL_STATIC_DRAW) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(T) * count, data, type);
		ErrorCheck();
	}

	/// Creates and binds a buffer
	/// 
	/// @param buffer The reference to your buffer (becomes set inside the function)
	/// @param target The type of the buffer
	static inline void CreateBuffer(GLuint& buffer, GLenum target = GL_ARRAY_BUFFER) {
		if (buffer == GL_NONE)
			glGenBuffers(1, &buffer);
		glBindBuffer(target, buffer);
		ErrorCheck();
	}

	/// Creates, binds and sets the buffer data
	/// 
	/// @param buffer The reference to your buffer (becomes set inside the function). This is actually a return value
	/// @param count The number of data elements
	/// @param data The data you want to fill the buffer with
	/// @param target The type of the buffer
	template<typename T>
	static void SetBuffer(GLuint& buffer, size_t count, const T* data, GLenum target = GL_ARRAY_BUFFER) {
		CreateBuffer(buffer, target);
		if (count > 0) {
			glBufferData(target, sizeof(T) * count, data, GL_STATIC_DRAW);
			ErrorCheck();
		}
	}

	/// Creates an uniform integer for your shader program
	/// 
	/// @note This is also used to set a Texture for your shader program
	/// @param ShaderProgram The shader program where you want to set your uniform int
	/// @param Name The name of your uniform int inside the shader program
	/// @return The reference you need to set the uniform int (e.g. with glUniform1iv())
	static unsigned int CreateUniformInt(GLuint& ShaderProgram, const char* Name) {
		return glGetUniformLocation(ShaderProgram, Name);
	}

	/// Sets a texture from a TexAttributes pointer
	/// 
	/// @return The texture id you need to bind the texture
	/// @param textureBuffer The buffer becomes generated and set by this function. Just pass a unsigned int, which becomes your texture buffer
	/// @param textAttr Contains all necessary texture information. For more info TexAttributes
	/// @param ShaderProgram The Shader in which you want to use this texture
	/// @param textureName The name of the texture inside the ShaderProgram
	static int SetTexture(GLuint& textureBuffer, TexAttributes* texAttr,GLuint& ShaderProgram,const char* textureName) {
		glGenTextures(1, &textureBuffer);
		glBindTexture(GL_TEXTURE_2D, textureBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, texAttr->numberOfComponents == 4 ? GL_RGBA : GL_RGB, texAttr->width, texAttr->height, 0, texAttr->numberOfComponents == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texAttr->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		ErrorCheck();
		return CreateUniformInt(ShaderProgram, textureName);
	}

	/// Sets a texture for an animation by an texture animation pointer
	/// 
	/// @return The texture id you need to bind the texture
	/// @param textureBuffer The buffer becomes generated and set by this function. Just pass a unsigned int, which becomes your texture buffer
	/// @param textAttr Contains all necessary texture animation information. For more info Texture
	/// @param ShaderProgram The Shader in which you want to use this texture
	/// @param textureName The name of the texture inside the ShaderProgram
	static int SetAnimTexture(GLuint& textureBuffer, Texture* texAttr, GLuint& ShaderProgram, const char* textureName) {
		glGenTextures(1, &textureBuffer);
		glBindTexture(GL_TEXTURE_2D, textureBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, texAttr->components == 4 ? GL_RGBA : GL_RGB, texAttr->width, texAttr->height, 0, texAttr->components == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texAttr->data.data());
 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//TO DO: Use GLTF infos in gl parameter
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,texAttr->MagFilter);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texAttr->MinFilter);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, texAttr->wrapS);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, texAttr->wrapT);
		//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, texAttr->wrapR);
		ErrorCheck();
		return CreateUniformInt(ShaderProgram, textureName);
	}

	/// Sets a uniform Matrix for the Shader program
	/// 
	/// @note You can call this function directly inside your draw function
	/// @param name The name inside the shader program of your uniform
	/// @param shaderProgram The Shader in which you want to use the uniform variable
	/// @param matrix This is the actual uniform value, you pass to the shader program
	static void SetUniformMatrix(unsigned int shaderProgram, const char* name, glm::mat4 matrix)
	{
		unsigned int loc = glGetUniformLocation(shaderProgram, name);
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
		ErrorCheck();
	}

	/// Sets a uniform Matrix Array for the Shader program
	/// 
	/// @note You can call this function directly inside your draw function
	/// @param name The name inside the shader program of your uniform
	/// @param shaderProgram The Shader in which you want to use the uniform variable
	/// @param length The number of matrices you pass to the ShaderProgramm
	/// @param data This is the actual uniform value, you pass to the shader program. It contains all Matrices in a row as float vector
	static void SetUniformMatrixArray(unsigned int shaderProgram, const char* name, int length, std::vector<float> data) {
		unsigned int loc = glGetUniformLocation(shaderProgram, name);
		glUniformMatrix4fv(loc, length, GL_FALSE, data.data());
		ErrorCheck();
	}

	/// Sets a uniform 2D Matrix for the Shader program
	/// 
	/// @note you can call this function directly inside your draw function
	/// @param name The name inside the shader program of your uniform
	/// @param shaderProgram The Shader in which you want to use the uniform variable
	/// @param matrix This is the actual uniform value, you pass to the shader program
	static void SetUniform2DMatrix(unsigned int shaderProgram, const char* name, glm::mat2 matrix) {
		unsigned int loc = glGetUniformLocation(shaderProgram, name);
		glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
		ErrorCheck();
	}

	/// Sets a uniform 3D Vector for the Shader program
	/// 
	/// @note You can call this function directly inside your draw function
	/// @param name The name inside the shader program of your uniform
	/// @param ShaderProgram The Shader in which you want to use the uniform variable
	/// @param vector This is the actual uniform value, you pass to the shader program
	static void SetUniformVector3(unsigned int ShaderProgram, const char* name, glm::vec3 vector) {
		unsigned int loc = glGetUniformLocation(ShaderProgram, name);
		glUniform3fv(loc, 1, glm::value_ptr(vector));
		ErrorCheck();
	}

	/// Sets a uniform 2D Vector for the Shader program
	/// 
	/// @note You can call this function directly inside your draw function
	/// @param name The name inside the shader program of your uniform
	/// @param ShaderProgram The Shader in which you want to use the uniform variable
	/// @param vector This is the actual uniform value, you pass to the shader program
	static void SetUniformVector2(unsigned int ShaderProgram, const char* name, glm::vec2 vector) {
		unsigned int loc = glGetUniformLocation(ShaderProgram, name);
		glUniform2fv(loc, 1, glm::value_ptr(vector));
		ErrorCheck();
	}

	/// Sets a uniform float for the Shader program
	/// 
	/// @note You can call this function directly inside your draw function
	/// @param name The name inside the shader program of your uniform
	/// @param ShaderProgram The Shader in which you want to use the uniform variable
	/// @param data This is the actual uniform value, you pass to the shader program
	static void SetUniformFloat(unsigned int ShaderProgram, const char* name, float data) {
		unsigned int loc = glGetUniformLocation(ShaderProgram, name);
		glUniform1fv(loc, 1, &data);
		ErrorCheck();
	}

	/// Sets a uniform int for the Shader program
	/// 
	/// @note You can call this function directly inside your draw function
	/// @param name The name inside the shader program of your uniform
	/// @param ShaderProgram The Shader in which you want to use the uniform variable
	/// @param data This is the actual uniform value, you pass to the shader program
	static void SetUniformInt(unsigned int ShaderProgram, const char* name, int data) {
		unsigned int loc = glGetUniformLocation(ShaderProgram, name);
		glUniform1iv(loc, 1, &data);
		ErrorCheck();
	}

	/// Activates and binds a Texture
	/// 
	/// @note You can call this function directly inside your draw function
	/// @note The texture ID must be already set, when calling this function. You can do this by calling glUniform1i(textureID, textureNumber) before this function
	/// @param texture This is the actual texture buffer, you want to bind
	/// @param textureNumber This is the texture number, when using just one texture the number is 0
	/// @param type The type of the texture, in most cases you want to use GL_TEXTURE_2D
	static void ActivateAndBindTexture(unsigned int texture, int textureNumber = 0,GLenum type = GL_TEXTURE_2D)
	{
		glActiveTexture(GL_TEXTURE0 + textureNumber);
		glBindTexture(type, texture);
		ErrorCheck();
	}


	/// Binds and activates the texture buffer and the texture id as a uniform
	/// 
	/// @note You can call this function directly inside your draw function
	/// @param texture This is the actual texture buffer, you want to bind
	/// @param textureID This is the texture ID you need to create the uniform and which becomes set here
	/// @param textureNumber This is the texture number, when using just one texture the number is 0
	/// @param type The type of the texture, in most cases you want to use GL_TEXTURE_2D
	static void BindTexture(unsigned int texture, unsigned int textureID, int textureNumber = 0, GLenum type = GL_TEXTURE_2D) {
		glUniform1i(textureID, textureNumber);
		ActivateAndBindTexture(texture, textureNumber, type);
		ErrorCheck();
	}

	/// Binds an array buffer
	/// 
	/// @note You can call this function directly inside your draw function
	/// @note This function can be used to set the vertices, uvs, and other per Vertex information
	/// @note You should use this function, when you have specified earlier your buffer data
	/// @param index This is the index which is used in the shader program to use the data in your buffer. Use ascending numbers for each function call like 1,2,3, ...
	/// @param size This is the size of one element inside the buffer. When having a vec3 buffer pass (int)3, when having a 4x4 matrix buffer use (int)16
	/// @param buffer This is the buffer where your data for your shader is stored
	/// @param type This is the type of your data in the buffer. Default is float btw. GL_FLOAT 
	static void BindArrayBuffer(unsigned int index, int size, unsigned int buffer,GLenum type = GL_FLOAT)
	{
		glEnableVertexAttribArray(index);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glVertexAttribPointer(index, size, type, GL_FALSE, 0, (void*)0);
		ErrorCheck();
	}

	/// Sets the content of an array buffer and binds it
	/// 
	/// @note You can call this function directly inside your draw function
	/// @note This function can be used to set the vertices, uvs, and other per Vertex information
	/// @note You should use this function, when you want to specify the buffer content inside the draw call. (Drawing dynamic content, like text, procedural meshes, ...)
	/// @param index This is the index which is used in the shader program to use the data in your buffer. Use ascending numbers for each function call like 1,2,3, ...
	/// @param size This is the size of one element inside the buffer. When having a vec3 buffer pass (int)3, when having a 4x4 matrix buffer use (int)16
	/// @param buffer This is the buffer where your data for your shader is stored
	/// @param dataType This is the type of your data in the buffer. Default is float btw. GL_FLOAT 
	/// @param count The number of data elements, you want to pass to the buffer
	/// @param data The actual data you pass to the buffer
	/// @param usageType The draw type of your buffer, in most cases you want to draw dynamic (GL_DYNAMIC_DRAW) 
	template<typename T>
	static void BindAndSetArrayBuffer(unsigned int index, int size, unsigned int buffer,int count, const T* data,GLenum usageType, GLenum dataType = GL_FLOAT)
	{
		glEnableVertexAttribArray(index);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		SetBufferData(data, count, usageType);
		glVertexAttribPointer(index, size, dataType, GL_FALSE, 0, (void*)0);
		ErrorCheck();
	}
}
