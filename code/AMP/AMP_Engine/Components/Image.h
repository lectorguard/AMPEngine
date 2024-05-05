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
#include "Graphics/Shader.h"
#include "AMP_Engine.h"
#include "Graphics/GLHelper.h"
#include <array>
#include <utility>

namespace amp
{
	/// This is a helper struct inside AMP
	/// 
	/// This struct contains all information to render a 2D image
	struct ImageInfo {

		ImageInfo() {};

		/// ImageInfo constructor
		///
		/// @param loc The 2d location in pixels on the screen, origin is bottom left
		/// @param SizeX This is the x-size of your image in pixel
		/// @param SizeY This is the y-size of your image in pixel
		/// @param index This is the index of the ImagePool you create when setting up the Image component (index in the order of the listed images)
		ImageInfo(glm::vec2 loc, int SizeX, int SizeY, int index)
			:location(loc),BoxSizeX(SizeX),BoxSizeY(SizeY),ImageIndex(index){}

		void update(glm::vec2 loc, int SizeX,int SizeY){
			location = loc;
			BoxSizeX = SizeX;
			BoxSizeY = SizeY;
		}
		glm::vec2 location = glm::vec2(0,0);
		int BoxSizeX = 0;
		int BoxSizeY = 0;
		int ImageIndex = 0;
	};

	/// This is a helper struct inside AMP
	/// 
	/// This struct contains all information to render a 2D Slider
	struct SliderInfo {
		ImageInfo box;
		ImageInfo slider;
		int minX = 0;
		int maxX = 0;
		int distance = 0;
	};

	/// This is a helper struct inside AMP for Image
	/// 
	/// This struct contains all the information of all rendered Images
	struct ImageRenderInfo{

		ImageRenderInfo(int imageIndex) : index(imageIndex) {};
		int ImageRenderCounter = 0;
		int index = 0;
		std::vector<ImageInfo> images;
		std::vector<float> vertices;
		std::vector<float> uvs;
	};
	
	/// This is an AMP default component
	/// 
	/// With this component you can render 2D Images
	/// Call the functionds from this component inside the Actor class
	class Image : public Component
	{
		using imageID_t = std::pair<int,int>;
	public:
		Image() {};
		Image(Image&&) = delete;
	
		Image(const Image&) = delete;
	
		virtual ~Image();

		/// Loads and sets up all images you can use in this component
		/// 
		/// @note you should call this method inside the Actor::AfterWindowStartUp() method
		/// @param imagePath This is a image pool where you specify all images you want to display with this certain component
		/// @note Supported formats are .jpg, .png and other established formats
		/// @attention When using transparent images the sequenz is very important
		/// @attention Make sure to list the images from front to back, otherwise images can disappear
		/// @note The images become rendered in inverse order
		/// @see Animation::SetUp() method, how to use directory paths
		void SetUp(std::vector<TEXT> imagePath);

		/// You can display your image with a custom shader, as an inspiration you can look at the default shader (assets/Shader/ImageShader.glsl)
		void SetShaderPath(TEXT path) { shaderPath = path; };

		/// With this function you can display your images on screen
		/// 
		/// @param index The index of the image inside the image pool
		/// @param location Sets the center the image on the screen in pixels (Origin is bottom left)
		/// @param pixelSizeX The x-size of the image in pixels 
		/// @param pixelSizeY The y-size of the image in pixels
		/// @return Save the returned ImageInfo, in order to update or to remove the image. The ImageInfo contains also information about the image
		/// @attention The set location is an ID to delete the image again so make sure not to delete the location
		/// @attention You should use the engine layout system called PIX to set sizes and locations which look great on all devices 
		/// @see PIX
		ImageInfo RenderImage(int index, glm::vec2 location, int pixelSizeX, int pixelSizeY);
	
		/// Rendering a horizontal slider
		///
		/// @note Sequence matters, select images from front to back 
		/// @param indexSlider The index of the image slider inside the image pool, this value must be smaller than the indexBox value
		/// @param indexBox The index of the image box inside the image pool, this value must be greater than the indexSlider value
		/// @param location The center of your slider on screen in pixels (Origin is bottom left)
		/// @param pixelSizeX The x-size of the slider in pixels 
		/// @param pixelSizeY The y-size of the slider in pixels
		/// @param percent The area of the box which is slidable. Example for 0.8: The first and the last 20 percent are not slidable.  
		/// @attention You should use the engine layout system called PIX to set sizes and locations which look great on all devices 
		/// @see PIX
		SliderInfo RenderHorizontalSlider(int indexSlider, int indexBox, glm::vec2 location, int pixelSizeX, int pixelSizeY, float percent = 0.8f);

		/// Updates a certain image in location and/or scale
		/// 
		/// @param image The image you received from RenderImage()
		/// @param newLocation The new center of your image on screen in pixels (Origin is bottom left) 
		/// @param sizeX The new x-size of the image in pixels
		/// @param sizeY The new y-size of the image in pixels
		/// @return The new ImageInfo
		/// @attention The new location does **not** replace the location, you need to delete the image. More info RenderImage()
		/// @attention You should use the engine layout system called PIX to set sizes and locations which look great on all devices 
		/// @see PIX
		ImageInfo updateImage(ImageInfo image, glm::vec2 newLocation, int sizeX, int sizeY);

		/// Updates the x-location of the slider
		/// 
		/// @param slider The slider you received from RenderHorizontalSlider()
		/// @param touchLocationX The new slider-x location, where the user has touched the screen 
		/// @return The slider "progress" in percent, where the start (mid position is 0.5) and the most left position is 0.0f and the most right is 1.0f
		float updateHorizontalSlider(SliderInfo slider,int touchLocationX);

		/// Stops certain image from being rendered
		/// 
		/// @param index This is the index of the image inside the image pool
		/// @param location This is the location you have set first when rendering the image
		void RemoveImage(int index, glm::vec2 location);

		/// Stops certain image from being rendered
		/// 
		/// @param imgInfo This is the image info, which was first returned when calling RenderImage() for the image you want to remove
		void RemoveImage(ImageInfo imgInfo);

		/// Reduces the size of the image a little bit to make the image look pressed
		/// 
		/// @param info The image info you received when calling RenderImage() for the image you want to have pressed
		void SetPressed(ImageInfo info);

		/// Calls SetUnpressed()
		void OnUntouch(int32_t x,int32_t y);

		/// Unpress the last touched image btw. sets the image size to normal again
		void SetUnpressed();

		/// Removes all rendered images
		void RemoveAllImages();

		/// High performance way of updating images
		/// 
		/// @param id The combination of index inside the image pool (first value) and the i-te rendered image of this type (second value)  
		/// @param id Overhead of searching the correct image you want to update is missing here
		/// @param SizeX The new x-size of the image in pixels 
		/// @param SizeY The new y-size of the image in pixels
		/// @param location The new center of your image on screen in pixels (Origin is bottom left) 
		/// @attention You should use the engine layout system called PIX to set sizes and locations which look great on all devices 
		/// @see PIX
		void UpdateImageInfo(imageID_t id, int SizeX, int SizeY, glm::vec2 location);

	protected:
		void SetTexture(TEXT& path);

		std::vector<float> CalculateVertices(int pixelSizeX, int pixelSizeY, glm::vec2& location);

		int FindTargetImageIndex(int index, glm::vec2 location);

		virtual void update() override;

		void draw(ImageRenderInfo info);

		virtual void OnActivate() override;

		virtual void OnDeactivate() override;

	private:
		TEXT shaderPath = TEXT("Shader/ImageShader.glsl"); 
		unsigned int ShaderProgram = GL_NONE;
		std::vector<unsigned int> TexIDs;
		std::vector<unsigned int> Textures;
		unsigned int VertexBuffer = GL_NONE;
		unsigned int UVBuffer = GL_NONE;

		callbacksIter Unpress;

		std::pair<int, int> isPressed = std::pair(-1, -1);
		float pressedPercent = 0.9f;

		std::vector<ImageRenderInfo> info;
		const std::vector<float> ConstUVs = {0,1,	//up left
											 0,0,	//down left
											 1,1,	//up right
											 1,0,	//down right
											 1,1,	//up right
											 0,0 };  //down left
	};
}

