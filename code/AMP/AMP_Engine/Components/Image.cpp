#include "pch.h"
#include "Image.h"

amp::Image::~Image()
{
	glDeleteBuffers(1, &VertexBuffer);
	glDeleteBuffers(1, &UVBuffer);
	for(auto& tex : Textures)glDeleteTextures(1, &tex);
	info.clear();
	GLHelper::ErrorCheck();
}

void amp::Image::SetUp(std::vector<TEXT> imagePath)
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
	int index = 0;
	for (auto& path : imagePath){
		SetTexture(path);
		info.push_back(ImageRenderInfo(index++));
	}
	GLHelper::CreateBuffer(UVBuffer);
	GLHelper::CreateBuffer(VertexBuffer);
	GLHelper::ErrorCheck();
}

void amp::Image::SetTexture(TEXT& path)
{
	TexAttributes* texAttr;
	if (auto result = engine->currentScene.AssetAttribMap.tryFind(path.getHash())) {
		texAttr = (TexAttributes*)*result;
	}
	else {
		texAttr = engine->assetLoader.loadTextureFromImage(path.getCharArray(),true);
		engine->currentScene.AssetAttribMap.add(path.getHash(), texAttr);
	}
	Textures.push_back(GL_NONE);
	TexIDs.push_back(GL_NONE);
	TexIDs.back() = GLHelper::SetTexture(Textures.back(), texAttr, ShaderProgram, "TextureSampler");
}

amp::ImageInfo amp::Image::RenderImage(int index, glm::vec2 location, int pixelSizeX, int pixelSizeY)
{
	if (index < 0 || index >= info.size() || pixelSizeX < 0 || pixelSizeY < 0)throw "invalid attributes";
	info[index].ImageRenderCounter++;
	ImageInfo img(location, pixelSizeX, pixelSizeY, index);
	info[index].images.push_back(img);
	auto vertices = CalculateVertices(pixelSizeX, pixelSizeY, location);

	auto& ImVer = info[index].vertices;
	ImVer.insert(ImVer.end(), vertices.begin(), vertices.end());

	auto& ImUVs = info[index].uvs;
	ImUVs.insert(ImUVs.end(), ConstUVs.begin(), ConstUVs.end());
	return img;
}

std::vector<float> amp::Image::CalculateVertices(int pixelSizeX, int pixelSizeY, glm::vec2& location)
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

amp::SliderInfo amp::Image::RenderHorizontalSlider(int indexSlider, int indexBox, glm::vec2 location, int pixelSizeX, int pixelSizeY, float percent /*= .80f*/)
{
	SliderInfo sliderI;
	sliderI.box = RenderImage(indexBox, location, pixelSizeX, pixelSizeY);
	sliderI.slider = RenderImage(indexSlider, location, pixelSizeY * percent, pixelSizeY * percent);
	int Margin = percent * pixelSizeY;
	sliderI.minX = location.x - (pixelSizeX / 2.0f) + Margin;
	sliderI.maxX = location.x + (pixelSizeX / 2.0f) - Margin;
	sliderI.distance = sliderI.maxX - sliderI.minX;
	return sliderI;
}

amp::ImageInfo amp::Image::updateImage(ImageInfo image, glm::vec2 newLocation, int sizeX, int sizeY)
{
	assert(UVBuffer);
	int TargetIndex = FindTargetImageIndex(image.ImageIndex,image.location);
	imageID_t id(image.ImageIndex, TargetIndex);
	UpdateImageInfo(id, sizeX, sizeY, newLocation);
	info[id.first].images[id.second].update(newLocation, sizeX, sizeY);
	return info[id.first].images[id.second];
}

float amp::Image::updateHorizontalSlider(SliderInfo slider, int touchLocationX)
{
	int ImgArrPtr = slider.slider.ImageIndex;
	int TargetIndex = FindTargetImageIndex(ImgArrPtr, slider.slider.location);
	if (touchLocationX > slider.maxX)touchLocationX = slider.maxX;
	if (touchLocationX < slider.minX)touchLocationX = slider.minX;
	glm::vec2 newLocation = glm::vec2(touchLocationX, slider.slider.location.y);
	auto id = imageID_t(ImgArrPtr, TargetIndex);
	UpdateImageInfo(id, slider.slider.BoxSizeX, slider.slider.BoxSizeY, newLocation);
	return (touchLocationX - slider.minX) / (float)(slider.distance);
}

void amp::Image::RemoveImage(int index, glm::vec2 location)
{
	int ImageIndex = FindTargetImageIndex(index, location);
	info[index].ImageRenderCounter--;
	auto& infoImages = info[index].images;
	infoImages.erase(infoImages.begin() + ImageIndex);
	auto& uvs = info[index].uvs;
	if(uvs.size() >= 12) uvs.erase(uvs.begin(),uvs.begin() + 12);
	auto& ver = info[index].vertices;
	if(ver.size() >= 12) ver.erase(ver.begin() + 12 * ImageIndex, ver.begin() + 12 * (ImageIndex+1));
}

int amp::Image::FindTargetImageIndex(int index, glm::vec2 location)
{
	int TargetIndex = -1;
	for (int i = 0; i < info[index].images.size(); ++i) {
		if (info[index].images[i].location == location) {
			TargetIndex = i;
			break;
		}
	}
	if (TargetIndex <= -1)throw "Invalid location";
	return TargetIndex;
}

void amp::Image::RemoveImage(ImageInfo imgInfo)
{
	RemoveImage(imgInfo.ImageIndex, imgInfo.location);
}

void amp::Image::UpdateImageInfo(imageID_t id, int SizeX, int SizeY, glm::vec2 location)
{
	auto vertices = CalculateVertices(SizeX, SizeY, location);
	auto& image = info[id.first];
	assert(id.second * 12 + 11 < image.vertices.size() && "Not enough image vertices (internal error)");
	for (int i = 0; i < 12; ++i)
	{
		image.vertices[i + id.second * 12] = vertices[i];
	}
}

void amp::Image::SetPressed(ImageInfo image)
{
	int TargetIndex = FindTargetImageIndex(image.ImageIndex, image.location);
	imageID_t id(image.ImageIndex, TargetIndex);
	if (id == isPressed)return;
	if (isPressed != imageID_t(-1, -1)) {
		auto& oldPressed = info[isPressed.first].images[isPressed.second];
		UpdateImageInfo(isPressed, oldPressed.BoxSizeX, oldPressed.BoxSizeY, oldPressed.location);
	}
	UpdateImageInfo(id, image.BoxSizeX * pressedPercent, image.BoxSizeY * pressedPercent, image.location);
	isPressed = id;
}

void amp::Image::OnUntouch(int32_t x,int32_t y)
{
	SetUnpressed();
}

void amp::Image::SetUnpressed()
{
	if (isPressed != imageID_t(-1, -1)) {
		auto& oldPressed = info[isPressed.first].images[isPressed.second];
		UpdateImageInfo(isPressed, oldPressed.BoxSizeX, oldPressed.BoxSizeY, oldPressed.location);
		isPressed = imageID_t(-1, -1);
	}
}

void amp::Image::RemoveAllImages()
{
	for(auto& elem : info){
		elem.ImageRenderCounter = 0;
		elem.images.clear();
		elem.uvs.clear();
		elem.vertices.clear();
	}
}

void amp::Image::update()
{
	for(int i = info.size()-1; i >= 0; --i){
		if (info[i].ImageRenderCounter > 0)draw(info[i]);
	}
}

void amp::Image::draw(ImageRenderInfo info)
{
	//Buffer recreation sometimes necessary, when Buffer becomes invalidated (set to 0)
	if(!UVBuffer)GLHelper::CreateBuffer(UVBuffer);
	if (!VertexBuffer)GLHelper::CreateBuffer(VertexBuffer);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glUseProgram(ShaderProgram);
	GLHelper::BindTexture(Textures[info.index], TexIDs[info.index]);
	GLHelper::BindAndSetArrayBuffer(0, 2, VertexBuffer, info.vertices.size(), info.vertices.data(), GL_DYNAMIC_DRAW);
	GLHelper::BindAndSetArrayBuffer(1, 2, UVBuffer, info.uvs.size(), info.uvs.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, info.vertices.size());
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	GLHelper::ErrorCheck();
}

void amp::Image::OnActivate()
{
	Unpress = engine->events.subscribeEvent<Image>(engine->endTouch, &Image::OnUntouch, this);
}

void amp::Image::OnDeactivate()
{
	engine->events.unsubscribeEvent(engine->endTouch, Unpress);
}
