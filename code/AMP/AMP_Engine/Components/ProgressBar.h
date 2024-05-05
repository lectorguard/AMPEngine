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
#include <utility>

namespace amp
{
	/// This is an AMP default component
	/// 
	/// This component allows you to render a progress bar
	/// Call the functions from this component inside the Actor class
	class ProgressBar : public Component
	{
	public:
		ProgressBar() {};
		ProgressBar(ProgressBar&&) = delete;

		ProgressBar(const ProgressBar&) = delete;

		virtual ~ProgressBar();

		//Textures must have same size and should be power of 2
		///Loads and sets up the images for the progress bars
		///
		/// @note All texture sizes should be a power of 2 and when you specify both images they should have the same size
		/// @note The here specified progress bars are all vertical
		/// @param progressBarPath Specifies the path to progress bar texture (all established formats are allowed .png, .jpg, ...)
		/// @param framePath Specifies a frame for the progress bar, but behaves just like a normal image
		/// @note You can find an example how to use SetUp functions here : Animation::SetUp() 
		void SetUp(TEXT progressBarPath, TEXT framePath = TEXT(""));

		/// Specifies where you progress bar starts and where it ends
		/// 
		/// @note upper This is the upper limit in percent. More intuitive : At which smallest vertical percent of the image is my progress bar full
		/// @note lower This is the lower limit in percent. More intuitive : At which biggest vertical percent of the image is my progress bar empty
		void SetLimits(float upper, float lower);

		///@return The upper (first) and the lower (second) limit of the image
		std::pair<float, float> getLimits() { return std::make_pair(upperLimit, lowerLimit); }

		/// You can specify a custom shader for your progress bar
		///
		/// @note As an inspiration you can have a look at the default shader ("assets/Shader/ProgressBarShader.glsl")
		void SetShaderPath(TEXT path) { shaderPath = path; };

		/// Creates and renders progress bar
		/// 
		/// @param location Specifies the center of your progress bar on the screen in pixels (origin is bottom left)
		/// @param pixelSizeX The x-size of the progress bar in pixels 
		/// @param pixelSizeY The y-size of the progress bar in pixels
		/// @param progress This is the starting default progress of the progress bar
		/// @note You can create just one progress bar with this component
		/// @attention You should use the engine layout system called PIX to set sizes and locations which look great on all devices 
		/// @see PIX
		void CreateProgressBar(glm::vec2 location, int pixelSizeX, int pixelSizeY, float progress = 1.0f);

		/// @return the current progress of the bar
		float getProgress() { return getProgressWithLimits(); };

		/// Updates the progress of the bar in percent
		///
		/// @param newProgress In percent
		void UpdateProgress(float newProgress);

		/// Removes the progress bar btw. stops rendering the progress bar
		///
		/// @note throws error when progress bar is already removed
		void RemoveProgressBar();

	protected:
		void AppendVerticesAndUVs(std::vector<float>& progressVertices);

		unsigned int SetTexture(TEXT& path, unsigned int& texBuffer, const char* name, bool isBar);

		std::vector<float> CalculateVertices(int pixelSizeX, int pixelSizeY, glm::vec2& location);

		virtual void update() override;

		void draw();

		void setProgressWithLimits(float newProgress);

		float getProgressWithLimits();

	private:
		float upperLimit = 1.0f;
		float lowerLimit = 0.0f;
		float deltaLimit = 1.0f;

		glm::vec2 location = glm::vec2(0, 0);
		int BoxSizeX = 0;
		int BoxSizeY = 0;
		float progress = 1.0f;

		float startBoxY = 0.0f;
		float boxYLength = 0.0f;

		bool isProgressBar = false;
		bool isFrame = false;

		std::vector<float> vertices;
		std::vector<float> uvs;

		unsigned int progressTex = GL_NONE;
		unsigned int progressTexID = GL_NONE;
		unsigned int frameTex = GL_NONE;
		unsigned int frameTexID = GL_NONE;


		TEXT shaderPath = TEXT("Shader/ProgressBarShader.glsl");
		unsigned int ShaderProgram = GL_NONE;
		unsigned int VertexBuffer = GL_NONE;
		unsigned int UVBuffer = GL_NONE;
		unsigned int texLocBuffer = GL_NONE;

		float resolutionProgressBarY = 256.0f;
		const std::vector<float> ConstUVs = { 0,1,	//up left
											 0,0,	//down left
											 1,1,	//up right
											 1,0,	//down right
											 1,1,	//up right
											 0,0 };  //down left
	};
}

