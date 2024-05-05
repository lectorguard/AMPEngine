#include "pch.h"
#include "TextRenderer.h"


#define FIRSTLETTER_IN_TEXTURE 32 // SPACE 

void amp::TextRenderer::setUpText(TEXT FontTexturePath /*= TEXT("Font/default.bmp")*/, const int CharactersPerLine /*= 32*/, const int NumberOfLines /*= 8*/)
{
	if (!engine)throw "engine not initialised for Text Render Component";
	this->CharactersPerLine = CharactersPerLine;
	this->NumberOfLines = NumberOfLines;
	ShaderAttributes* shaderAttr = nullptr;
	if (auto result = engine->currentScene.AssetAttribMap.tryFind(shaderPath.getHash())) {
		shaderAttr = (ShaderAttributes*)*result;
	}
	else
	{
		shaderAttr = engine->ampShader.getShaderProgramFromPath(shaderPath.getCharArray());
		engine->currentScene.AssetAttribMap.add(shaderPath.getHash(), shaderAttr);
	}


	TexAttributes* imgAttrib = nullptr;
	if (auto result = engine->currentScene.AssetAttribMap.tryFind(FontTexturePath.getHash())) {
		imgAttrib = (TexAttributes*)*result;
	}
	else
	{
		imgAttrib = engine->assetLoader.loadTextureFromImage(FontTexturePath.getCharArray());
		engine->currentScene.AssetAttribMap.add(FontTexturePath.getHash(), imgAttrib);
	}

	this->ShaderProgram = shaderAttr->ShaderProgram;
	
	HalfWidthHeight = glm::vec2(engine->ampContext.getWidth() / 2.0f, engine->ampContext.getHeight() / 2.0f);
	
	GLHelper::CreateBuffer(VertexBuffer);
	GLHelper::CreateBuffer(UVBuffer);
	TextureID = GLHelper::SetTexture(Texture,imgAttrib, this->ShaderProgram, "FontTexSampler");
	GLHelper::ErrorCheck();
}

void amp::TextRenderer::renderText(std::string text, int locX, int locY, int fontsize,float duration)
{
	TEXT curr(text);
	if(duration > 0.0f)DurationRemoveEvent(duration, curr);
	TextRenderInformation info(curr);
	info.locX = locX;
	info.locY = locY;
	info.fontsize = fontsize;
	textInfos.push_back(info);
}

void amp::TextRenderer::renderText(TextRenderInformation info, float duration)
{
	if (duration > 0.0f)DurationRemoveEvent(duration, info.text);
	textInfos.push_back(info);
}

void amp::TextRenderer::renderText(std::string text, float duration)
{
	TEXT curr(text);
	if (duration > 0.0f)DurationRemoveEvent(duration, curr);
	TextRenderInformation info(curr);
	textInfos.push_back(info);
}



void amp::TextRenderer::DurationRemoveEvent(float duration, TEXT& curr)
{
	TimeEvent eraseEvent;
	eraseEvent.init<amp::TextRenderer,size_t>(duration, false, &amp::TextRenderer::eraseElementByHash,this, curr.getHash());
	engine->addTimerFunction(eraseEvent);
}

void amp::TextRenderer::eraseLast()
{
	textInfos.pop_back();
}

void amp::TextRenderer::updateTextSettings(TextRenderInformation info)
{
	int index = -1;
	for (int i = 0; i < textInfos.size(); ++i) {
		if (textInfos[i].text == info.text) {
			index = i;
			break;
		}
	}
	assert(index >= 0);
	textInfos[index] = info;
}

void amp::TextRenderer::updateTextColType(TextRenderInformation& info, ColType type)
{
	if (info.coltype != type) {
		info.coltype = type;
		updateTextSettings(info);
	}
}

void amp::TextRenderer::deleteText(TextRenderInformation info)
{
	eraseElementByHash(info.text.getHash());
}

void amp::TextRenderer::deleteTextSequence(std::vector<TextRenderInformation> info)
{
	auto firstItr = std::find_if(textInfos.begin(), textInfos.end(), [&](auto A) {return info[0].text.getHash() == A.text.getHash(); });
	//if crash size of texInfos is probably too small
	if(firstItr != textInfos.end())textInfos.erase(firstItr, firstItr + info.size());
}

void amp::TextRenderer::eraseElementByHash(size_t hash)
{
	auto CurrItr = textInfos.begin();
	auto endItr = textInfos.end();
	textInfos.erase(std::remove_if(CurrItr, endItr, [hash](TextRenderInformation k) {return k.text.getHash() == hash; }),endItr);
}

void amp::TextRenderer::update()
{
	draw();
}


void amp::TextRenderer::draw()
{
	assert(ShaderProgram && "SetUpText has to be called before rendering starts (loop)");
	if (textInfos.size() == 0)return;
	if (isAnim)ColorChangingTextAnimation();
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;
	for (auto& info : textInfos) {
		SetUVsAndVerticesForText(info, positions, uvs);
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glUseProgram(ShaderProgram);
	GLHelper::SetUniformVector3(ShaderProgram, "DefaultColor", DefaultColor);
	GLHelper::SetUniformVector3(ShaderProgram, "PressedColor", PressedColor);
	GLHelper::BindTexture(Texture,TextureID);
	GLHelper::BindAndSetArrayBuffer(0, 3, VertexBuffer, positions.size(), positions.data(), GL_DYNAMIC_DRAW);
	GLHelper::BindAndSetArrayBuffer(1, 2, UVBuffer, uvs.size(), uvs.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, positions.size());
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glDisable(GL_BLEND);
	GLHelper::ErrorCheck();
}

void amp::TextRenderer::SetUVsAndVerticesForText(TextRenderInformation& info, std::vector<glm::vec3>& positions, std::vector<glm::vec2>& uvs)
{
	int row = 0;
	int column = 0;
	int length = info.text.getTextLength();
	for (unsigned int i = 0; i < length; i++) {
		char character = info.text[i];
		if (character == '\n') {
			++row;
			column = 0;
			continue;
		}
		SetLetterVertices(info, row, column, positions);
		character -= FIRSTLETTER_IN_TEXTURE;
		SetLetterUVs(character, uvs);
		++column;
	}
}

void amp::TextRenderer::SetLetterUVs(char character, std::vector<glm::vec2>& uvs)
{
	//Top Left position
	float sbl_X = 1 / (float)CharactersPerLine; // size between letters Adoption they are cube shaped
	float sbl_Y = 1 / (float)NumberOfLines;
	float uv_x = (character % CharactersPerLine) / (float)CharactersPerLine;
	float uv_y = (character / CharactersPerLine) / (float)NumberOfLines;

	glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
	glm::vec2 uv_up_right = glm::vec2(uv_x + sbl_X, uv_y);
	glm::vec2 uv_down_right = glm::vec2(uv_x + sbl_X, uv_y + sbl_Y);
	glm::vec2 uv_down_left = glm::vec2(uv_x, uv_y + sbl_Y);

	// origin is bottom left from the texture (gl UVs)
	uvs.push_back(uv_up_left);
	uvs.push_back(uv_down_left);
	uvs.push_back(uv_up_right);

	uvs.push_back(uv_down_right);
	uvs.push_back(uv_up_right);
	uvs.push_back(uv_down_left);
}

void amp::TextRenderer::SetLetterVertices(TextRenderInformation& info, int row, int column, std::vector<glm::vec3>& positions)
{
	int currLocY = info.locY - row * info.fontsize;
	glm::vec2 vertex_up_left = (glm::vec2(info.locX + column * info.fontsize, currLocY + info.fontsize) - HalfWidthHeight) / HalfWidthHeight;
	glm::vec2 vertex_up_right = (glm::vec2(info.locX + column * info.fontsize + info.fontsize, currLocY + info.fontsize) - HalfWidthHeight) / HalfWidthHeight;
	glm::vec2 vertex_down_right = (glm::vec2(info.locX + column * info.fontsize + info.fontsize, currLocY) - HalfWidthHeight) / HalfWidthHeight;
	glm::vec2 vertex_down_left = (glm::vec2(info.locX + column * info.fontsize, currLocY) - HalfWidthHeight) / HalfWidthHeight;

	positions.push_back(glm::vec3(vertex_up_left   ,info.coltype));
	positions.push_back(glm::vec3(vertex_down_left ,info.coltype));
	positions.push_back(glm::vec3(vertex_up_right  ,info.coltype));
	positions.push_back(glm::vec3(vertex_down_right,info.coltype));
	positions.push_back(glm::vec3(vertex_up_right  ,info.coltype));
	positions.push_back(glm::vec3(vertex_down_left ,info.coltype));
}


void amp::TextRenderer::ColorChangingTextAnimation()
{
	accumulator += engine->frameTime;
	float r = glm::sin(accumulator) * glm::sin(accumulator);
	if (r < 0)r = 0;
	float g = glm::sin(accumulator + glm::pi<float>() / 3.0) * glm::sin(accumulator + glm::pi<float>() / 3.0);
	if (g < 0)g = 0;
	float b = glm::sin(accumulator + glm::pi<float>() * 2 / 3) * glm::sin(accumulator + glm::pi<float>() * 2 / 3);
	if (b < 0)b = 0;
	DefaultColor = glm::vec3(r, g, b);
}

amp::TextRenderer::~TextRenderer() {
	glDeleteBuffers(1, &VertexBuffer);
	glDeleteBuffers(1, &UVBuffer);
	glDeleteTextures(1, &Texture);
	textInfos.clear();
	GLHelper::ErrorCheck();
}