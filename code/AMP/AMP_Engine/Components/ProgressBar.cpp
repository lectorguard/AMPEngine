#include "pch.h"
#include "ProgressBar.h"

amp::ProgressBar::~ProgressBar()
{
	glDeleteBuffers(1, &VertexBuffer);
	glDeleteBuffers(1, &UVBuffer);
	glDeleteBuffers(1, &texLocBuffer);
	if (progressTex != GL_NONE)glDeleteTextures(1, &progressTex);
	if (frameTex != GL_NONE)glDeleteTextures(1, &frameTex);
	GLHelper::ErrorCheck();

}


void amp::ProgressBar::SetUp(TEXT progressBarPath, TEXT framePath)
{
	if (!engine)throw "Engine Pointer not set for Image Component";
	ShaderAttributes* shaderAttr = nullptr;
	if (auto result = engine->currentScene.AssetAttribMap.tryFind(shaderPath.getHash())) {
		shaderAttr = (ShaderAttributes*)*result;
	}
	else
	{
		shaderAttr = engine->ampShader.getShaderProgramFromPath(shaderPath.getCharArray());
		engine->currentScene.AssetAttribMap.add(shaderPath.getHash(), shaderAttr);
	}
	this->ShaderProgram = shaderAttr->ShaderProgram;

	progressTexID =	SetTexture(progressBarPath, progressTex, "progressTex", true);
	if (framePath.getTextLength() > 0) {
		frameTexID = SetTexture(framePath, frameTex, "frameTex", false);
	}
	GLHelper::CreateBuffer(VertexBuffer);
	GLHelper::CreateBuffer(UVBuffer);
	GLHelper::CreateBuffer(texLocBuffer);
	GLHelper::ErrorCheck();
}

void amp::ProgressBar::SetLimits(float upper, float lower)
{
	if (upper > 1.0f || upper < 0.0f || lower > 1.0f || lower < 0.0f || lower >= upper)throw "Invalid input";
	upperLimit = upper;
	lowerLimit = lower;
	deltaLimit = upper - lower;
}

void amp::ProgressBar::CreateProgressBar(glm::vec2 location, int pixelSizeX, int pixelSizeY, float progress)
{
	if (isProgressBar || isFrame)throw "Delete old Progress Bar before Creating new Progress Bar";
	this->location = location;
	this->BoxSizeX = pixelSizeX;
	this->BoxSizeY = pixelSizeY;
	this->startBoxY = (location.y - (pixelSizeY / 2.0f));
	this->boxYLength = pixelSizeY;
	setProgressWithLimits(progress);
	auto progressVertices = CalculateVertices(pixelSizeX, pixelSizeY, location);
	AppendVerticesAndUVs(progressVertices);
	isProgressBar = true;
	if (frameTex != GL_NONE)isFrame = true;
}

void amp::ProgressBar::AppendVerticesAndUVs(std::vector<float>& progressVertices)
{
	this->vertices.insert(vertices.end(), progressVertices.begin(), progressVertices.end());
	this->uvs.insert(uvs.end(), ConstUVs.begin(), ConstUVs.end());
}

void amp::ProgressBar::UpdateProgress(float newProgress)
{
	setProgressWithLimits(newProgress);
}

void amp::ProgressBar::RemoveProgressBar()
{
	if (!isProgressBar)throw "Progress Bar already deleted";
	isProgressBar = false;
	isFrame = false;
	uvs.clear();
	vertices.clear();
}

unsigned int amp::ProgressBar::SetTexture(TEXT& path, unsigned int& texBuffer, const char* name,bool isBar)
{
	TexAttributes* texAttr = nullptr;
	if (auto result = engine->currentScene.AssetAttribMap.tryFind(path.getHash())) {
		texAttr = (TexAttributes*)*result;
	}
	else {
		texAttr = engine->assetLoader.loadTextureFromImage(path.getCharArray(), true);
		engine->currentScene.AssetAttribMap.add(path.getHash(), texAttr);
	}
	if(isBar)resolutionProgressBarY = texAttr->height;
	return GLHelper::SetTexture(texBuffer, texAttr, ShaderProgram, name);
}

std::vector<float> amp::ProgressBar::CalculateVertices(int pixelSizeX, int pixelSizeY, glm::vec2& location)
{
	int HalfX = pixelSizeX / 2.0f;
	int HalfY = pixelSizeY / 2.0f;
	float HalfWidth = engine->ampContext.getWidth() / 2.0f;
	float HalfHeight = engine->ampContext.getHeight() / 2.0f;
	std::vector<float> vertices = {
		(location.x - HalfX - HalfWidth) / HalfWidth, (location.y + HalfY - HalfHeight) / HalfHeight, //top left
		(location.x - HalfX - HalfWidth) / HalfWidth, (location.y - HalfY - HalfHeight) / HalfHeight, //bot left
		(location.x + HalfX - HalfWidth) / HalfWidth, (location.y + HalfY - HalfHeight) / HalfHeight, //top right
		(location.x + HalfX - HalfWidth) / HalfWidth, (location.y - HalfY - HalfHeight) / HalfHeight, //bot right
		(location.x + HalfX - HalfWidth) / HalfWidth, (location.y + HalfY - HalfHeight) / HalfHeight, //top right
		(location.x - HalfX - HalfWidth) / HalfWidth, (location.y - HalfY - HalfHeight) / HalfHeight  //bot left
	};
	return vertices;
}

void amp::ProgressBar::update()
{
	if (isProgressBar)draw();
}

void amp::ProgressBar::draw()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glUseProgram(ShaderProgram);
	GLHelper::BindTexture(progressTex, progressTexID, 0);
	if(isFrame)GLHelper::BindTexture(frameTex, frameTexID, 1);
	GLHelper::SetUniformFloat(ShaderProgram, "progress", progress);
	GLHelper::SetUniformVector2(ShaderProgram, "yRange", glm::vec2(startBoxY,boxYLength));
	GLHelper::SetUniformInt(ShaderProgram, "isFrame", isFrame);
	GLHelper::BindAndSetArrayBuffer(0, 2, VertexBuffer, vertices.size(), vertices.data(), GL_STATIC_DRAW);
	GLHelper::BindAndSetArrayBuffer(1, 2, UVBuffer, uvs.size(), uvs.data(), GL_STATIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glDisable(GL_BLEND);
	GLHelper::ErrorCheck();
}

void amp::ProgressBar::setProgressWithLimits(float newProgress)
{
	if (newProgress > 1.0f)newProgress = 1.0;
	if (newProgress < 0.0f)newProgress = 0.0f;
	newProgress *= deltaLimit;
	newProgress += lowerLimit;
	this->progress = newProgress;
}

float amp::ProgressBar::getProgressWithLimits()
{
	assert(progress <= upperLimit && progress >= lowerLimit);
	auto newProgress = this->progress;
	newProgress -= lowerLimit;
	newProgress /= deltaLimit;
	return newProgress;
}
