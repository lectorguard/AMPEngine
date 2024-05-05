#include "pch.h"
#include "CubeMap.h"

amp::CubeMap::~CubeMap()
{
}


void amp::CubeMap::createCubeMap(TEXT forward, TEXT backward, TEXT left, TEXT right, TEXT top, TEXT bottom)
{
	CreateCubemap();
	TEXT paths[6] = {right,left,top,bottom,forward,backward};
	SetCubeMapTextures(paths);
	SetCubeMapParams();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	GLHelper::SetBuffer(cube->positionBuffer, numberOfVertices, skyboxVertices);
	cube->isDrawEnabled = false;
	cube->LoadShader(shaderPath);
	texID = GLHelper::CreateUniformInt(cube->ShaderProgram, "cubeMap");
	GLHelper::ErrorCheck();
}

void amp::CubeMap::SetCubeMapParams()
{
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void amp::CubeMap::CreateCubemap()
{
	glGenTextures(1, &cubeMapBuffer);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapBuffer);
}

void amp::CubeMap::SetCubeMapTextures(TEXT* paths)
{
	for (int i = 0; i < 6; i++)
	{
		TexAttributes* texAttr = nullptr;
		if (auto result = engine->currentScene.AssetAttribMap.tryFind(paths[i].getHash())) {
			texAttr = (TexAttributes*)*result;
		}
		else {
			texAttr = engine->assetLoader.loadTextureFromImage(paths[i].getCharArray());
			engine->currentScene.AssetAttribMap.add(paths[i].getHash(), texAttr);
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			texAttr->numberOfComponents == 4 ? GL_RGBA : GL_RGB,
			texAttr->width, texAttr->height,
			0, texAttr->numberOfComponents == 4 ? GL_RGBA : GL_RGB,
			GL_UNSIGNED_BYTE, texAttr->data);
	}
}

void amp::CubeMap::drawAfterScene()
{
	if (!cube)throw "Component was not initialised";
	glDepthFunc(GL_LEQUAL);
	glUseProgram(cube->ShaderProgram);
	GLHelper::SetUniformMatrix(cube->ShaderProgram, "view", glm::mat4(glm::mat3(Camera::getView())));
	GLHelper::SetUniformMatrix(cube->ShaderProgram, "projection", Camera::getProjection());

	GLHelper::BindArrayBuffer(0, 3, cube->positionBuffer);
	GLHelper::BindTexture(cubeMapBuffer,texID, 0, GLenum(GL_TEXTURE_CUBE_MAP));
	
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDisableVertexAttribArray(0);
	glDepthFunc(GL_LESS);
	GLHelper::ErrorCheck();
}

void amp::CubeMap::BeforeSystemShutdown()
{
	glDeleteTextures(1, &cubeMapBuffer);
}

void amp::CubeMap::OnActivate()
{
	//engine->postSceneUpdate.push_back([this]() {drawAfterScene(); });
	postUpdate = engine->events.subscribeEvent(engine->postSceneUpdate, &CubeMap::drawAfterScene,this);
}

void amp::CubeMap::OnDeactivate()
{
	engine->events.unsubscribeEvent(engine->postSceneUpdate, postUpdate);
	/*throw std::logic_error("The method or operation is not implemented.");*/
}

