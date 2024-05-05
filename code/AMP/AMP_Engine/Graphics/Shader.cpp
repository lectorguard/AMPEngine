
#include "Shader.h"

void amp::Shader::setAssetManager(AAssetManager* manager)
{
	this->manager = manager;
}

ShaderAttributes* amp::Shader::getShaderProgramFromPath(const char* path)
{
	if (manager == nullptr)throw "AssetManager for Shader is not set !!";
	std::string vertexString;
	std::string fragmentString;
	FilesManager::readShaderFile(manager, path, vertexString, fragmentString);
	const char* vertexSource = vertexString.c_str();
	const char* fragmentSource = fragmentString.c_str();
	unsigned int vertexShader = CreateShader(vertexSource, GL_VERTEX_SHADER);
	unsigned int fragmentShader = CreateShader(fragmentSource, GL_FRAGMENT_SHADER);
	unsigned int ShaderProgram = CreateProgram(vertexShader, fragmentShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	ShaderAttributes* attrib = new ShaderAttributes;
	attrib->ShaderProgram = ShaderProgram;
	return attrib;
}

unsigned int amp::Shader::CreateProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
	unsigned int ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, vertexShader);
	glAttachShader(ShaderProgram, fragmentShader);
	glLinkProgram(ShaderProgram);
	int success;
	char infoLog[512];
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ShaderProgram, 512, NULL, infoLog);
		throw std::runtime_error(std::string("ShaderProgram failed to create : ") + infoLog);
	}	
	return ShaderProgram;
}

//35632 = Fragment Shader
//35633 = Vertex Shader
unsigned int amp::Shader::CreateShader(const char* Source, GLenum Type)
{
	unsigned int ShaderID = glCreateShader(Type);
	glShaderSource(ShaderID, 1, &Source, NULL);
	glCompileShader(ShaderID);
	int  success;
	char infoLog[512];
	glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ShaderID, 512, NULL, infoLog);
		throw std::runtime_error(std::string("Shader failed to compile : ") + infoLog);
	}	
	return ShaderID;
}
