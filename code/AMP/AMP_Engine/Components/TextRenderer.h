#pragma once
#include <GLES3/gl31.h>
#include <GLES3/gl3ext.h>
#include "GLES3/gl3platform.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "Component.h"
#include "../AssetLoader/GeneralAssetLoader.h"
#include "AMP_Engine.h"
#include "Text.h"
#include "vector"
#include "cstdio"
#include "cassert"
#include "Graphics/Shader.h"
#include "Graphics/GLHelper.h"




namespace amp
{
	/// This is an AMP default helper enum for the TextRenderer
	///
	/// Specifies if text is pressed or not
	enum ColType {
		Default = 0,Pressed = 1
	};

	/// This is a helper struct inside AMP for the TextRenderer
	/// 
	/// This struct contains all the information about rendering text
	struct TextRenderInformation {
		TEXT text;
		int locX = 0;
		int locY = 0;
		int fontsize = 60;
		ColType coltype = ColType::Default;
		TextRenderInformation() {};

		/// Just simple text with default settings
		TextRenderInformation(TEXT text){
			this->text = text;
		};

		/// Constructor for TextRenderer helper struct
		/// 
		/// @param rendText This is the text you want to render
		/// @param x This is the x screen position of the left-bottom start letter in pixels
		/// @param y This is the y screen position of the left-bottom start letter in pixels
		/// @param size This is the size of your text (btw. letters) in pixels
		/// @param type Specifies if the text is pressed or not (These are 2 different Colors)
		/// @attention You should use the engine layout system called PIX to set sizes and locations which look great on all devices 
		/// @see PIX
		TextRenderInformation(TEXT rendText,int x,int y, int size, ColType type = ColType::Default):
		text(rendText),locX(x),locY(y),fontsize(size),coltype(type)
		{
		};

		/// Specifies the color type btw. pressed type of your text
		///
		///@param type ColType
		void SetColType(ColType type) {
			coltype = type;
		}

	};
	
	/// This is an AMP default component
	/// 
	/// This component allows you to render text
	/// Call the functions from this component inside the Actor class
	class TextRenderer : public Component
	{
	public:
		TextRenderer() {};
		TextRenderer(const TextRenderer&) = delete;
		TextRenderer(TextRenderer&&) = delete;
		~TextRenderer() override;

		/// Sets up your Font
		/// 
		/// @param FontTexturePath This is the path to your texture atlas. You can create your own atlas with your own font using this free software: http://www.codehead.co.uk/cbfg/ 
		/// @param FontTexturePath An example how to use SetUp methods can you find here : Animation::SetUp()
		/// @param CharactersPerLine This values specifies how many characters per line you have in your texture atlas
		/// @param NumberOfLines This values specifies how many lines you have in your texture atlas
		/// @attention When you want to use the default shader for your custom atlases, make sure your texture atlas has **red** background, and **blue** letters
		void setUpText(TEXT FontTexturePath = TEXT("Font/default.bmp"), const int CharactersPerLine = 32, const int NumberOfLines = 8);

		/// Starts text rendering
		/// 
		/// @param text Specifies the text you want to render
		/// @param locX This is the x screen position of the left-bottom start letter in pixels
		/// @param locY This is the y screen position of the left-bottom start letter in pixels
		/// @param fontsize This is the size of your text (btw. letters) in pixels
		/// @param duration How long the text will be displayed in seconds. 0.0f means forever
		/// @attention You should use the engine layout system called PIX to set sizes and locations which look great on all devices 
		/// @see PIX
		void renderText(std::string text, int locX, int locY, int fontsize, float duration = 0.0f);

		/// Starts text rendering
		/// 
		/// @param info Contains all text information to render your text
		/// @param duration How long the text will be displayed in seconds. 0.0f means forever
		void renderText(TextRenderInformation info, float duration = 0.0f);

		/// Starts text rendering
		/// 
		/// @param text Renders text with the default render information. You can find them at TextRenderInformation
		/// @param duration How long the text will be displayed in seconds. 0.0f means forever
		void renderText(std::string text, float duration = 0.0f);

		/// @return The number of different text(s) which are rendered at the moment 
		int count() { return textInfos.size(); };

		/// You can specify a custom shader for your text renderer
		///
		/// @note As an inspiration you can have a look at the default shader ("assets/Shader/FontShader.glsl")
		void setShaderPath(TEXT path) { shaderPath = path; };

		/// Deletes the last element(text) in the render queue (This is the text which came last in) 
		void eraseLast();

		/// Updates a displayed text
		///
		/// @attention Make sure the string content stays the same
		/// @attention Otherwise delete the old text and render a new one
		void updateTextSettings(TextRenderInformation info);

		/// Updates the color type of a rendered text
		///
		/// @param info The TextRenderInformation of the rendered text
		/// @param type The new color type
		void updateTextColType(TextRenderInformation& info, ColType type);

		/// Removes a text from the render pipeline
		/// 
		/// @param info The currently rendered text
		void deleteText(TextRenderInformation info);

		/// You can delete multiple texts which where you called renderText() behind each other
		/// 
		/// @param info Make sure the first element, is also the first text where you called renderText()
		/// @attention You can just delete blocks of rendered text !!
		void deleteTextSequence(std::vector<TextRenderInformation> info);

		
		/// Deletes rendered text by hash
		///
		/// @param hash You can find the hash in TextRenderInformation::text.getHash()
		void eraseElementByHash(size_t hash);

		/// Sets the default Color of your text
		/// 
		/// @attention **Ranges: 0.0f - 1.0f**
		/// @param r This is the red value of your color 
		/// @param g This is the green value of your color 
		/// @param b This is the blue value of your color 
		void setDefaultColor(float r, float g, float b) {
			DefaultColor = glm::vec3(r, g, b);
		}

		/// Sets the pressed Color of your text
		/// 
		/// @attention **Ranges: 0.0f - 1.0f**
		/// @param r This is the red value of your color 
		/// @param g This is the green value of your color 
		/// @param b This is the blue value of your color 
		void setPressColor(float r, float g, float b) {
			PressedColor = glm::vec3(r, g, b);
		}

		/// Makes a Color animation for your text letters
		/// 
		/// @note When activating this feature, pressed and default colors are **ignored**
		/// @param IsAnim (De-)Activates the color animation. Default is false
		void setAnimation(bool IsAnim) {
			this->isAnim = IsAnim;
		}

	private:
		void update() override;

		void draw();

		void DurationRemoveEvent(float duration, TEXT& curr);

		void SetUVsAndVerticesForText(TextRenderInformation& info, std::vector<glm::vec3>& positions, std::vector<glm::vec2>& uvs);

		void SetLetterUVs(char character, std::vector<glm::vec2>& uvs);

		void SetLetterVertices(TextRenderInformation& info, int row, int column, std::vector<glm::vec3>& positions);

		void ColorChangingTextAnimation();

		unsigned int VBO = GL_NONE;
		unsigned int VertexBuffer = GL_NONE;
		unsigned int UVBuffer = GL_NONE;
		unsigned int ShaderProgram = GL_NONE;
		unsigned int TextureID = GL_NONE;
		unsigned int Texture = GL_NONE;

		TEXT shaderPath = TEXT("Shader/FontShader.glsl");
		glm::vec3 DefaultColor = glm::vec3(1.0,0.0,0.0);
		glm::vec3 PressedColor = glm::vec3(1.0, 1.0, 1.0);
		bool isAnim = false;
		int CharactersPerLine = 32;
		int NumberOfLines = 8;
		double accumulator = 0.0;
		glm::vec2 HalfWidthHeight = glm::vec2(0,0);
		std::vector<TextRenderInformation> textInfos;
	};
}

