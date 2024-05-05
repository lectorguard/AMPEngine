#pragma once

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtx/string_cast.hpp"
#include "Component.h"
#include "AMP_Engine.h"
#include <array>
#include <set>
#include "functional"
#include "Components/Image.h"
#include "Components/TextRenderer.h"
#include "Event/EventSystem.h"
#include "Text.h"
#include "list"
#include "algorithm"

namespace amp
{

	/// This is a helper struct inside AMP for TouchGesture
	/// 
	/// This struct contains all the information about a pressable button
	struct Button {

		Button() {};
		/// Creates a button with the size of an Image
		/// 
		/// @param info Turns an image info into a button
		Button(ImageInfo info) : location(info.location), rectX(info.BoxSizeX),rectY(info.BoxSizeY)
		{
			SetID(info.location);
		}

		/// Creates a button with the size of an slider
		/// 
		/// @param info Turns a slider info into a button
		Button(SliderInfo info) : location(info.box.location),rectX(info.box.BoxSizeX),rectY(info.box.BoxSizeY)
		{
			SetID(info.box.location);
		}

		/// Creates a button by user input (rectangle)
		/// 
		/// @param sizeX The new x-size of the button in pixels 
		/// @param sizeY The new y-size of the button in pixels
		/// @param loc The new center of your button on screen in pixels (Origin is bottom left) 
		/// @attention You should use the engine layout system called PIX to set sizes and locations which look great on all devices 
		/// @see PIX
		Button(glm::vec2 loc, int sizeX, int sizeY) : location(loc),rectX(sizeX),rectY(sizeY)
		{
			SetID(loc);
		}

		/// Creates a button with the size of a rendered text
		/// 
		/// @param renderInfo Turns a text render info into a button
		Button(TextRenderInformation renderInfo) {

			int numLetters = renderInfo.text.getString().length();
			glm::vec2 DownLeft = glm::vec2(renderInfo.locX, renderInfo.locY);
			glm::vec2 TopRight = glm::vec2(DownLeft.x + numLetters * renderInfo.fontsize, DownLeft.y + renderInfo.fontsize);
			location = GetCenter(DownLeft,TopRight);
			rectX = numLetters * renderInfo.fontsize;
			rectY = renderInfo.fontsize;
			SetID(location);
		}

		/// The id of a button is specified by its location
		void SetID(glm::vec2 location) {
			id = TEXT(glm::to_string(location)).getHash();
		}

		/// Calculates the center of a Button
		glm::vec2 GetCenter(glm::vec2 botLeft, glm::vec2 topRight)
		{
			return botLeft + (topRight - botLeft) * 0.5f;
		}

		glm::vec2 location;
		int rectX = 0;
		int rectY = 0;
		int id = 0;
		int minX = 0;
		int maxX = 0;
		int minY = 0;
		int maxY = 0;
		std::function<void()> buttonTouchCallback = nullptr;
	};

	/// This is a helper struct inside AMP for TouchGesture
	/// 
	/// This struct contains all the information about a simple pressable Button
	/// The difference between a normal Button is that this button has an additional start pressed and end pressed callback
	struct SimpleButton : public Button{

		SimpleButton() {};
		/// Creates Button from Image
		SimpleButton(ImageInfo i) :Button(i) {};
		/// Creates Button from slider
		SimpleButton(SliderInfo s) : Button(s) {};
		/// Creates Button from text renderer
		SimpleButton(TextRenderInformation i) : Button(i) {};

		/// Creates a button by user input (rectangle)
		/// 
		/// @param sizeX The new x-size of the button in pixels 
		/// @param sizeY The new y-size of the button in pixels
		/// @param loc The new center of your button on screen in pixels (Origin is bottom left) 
		/// @attention You should use the engine layout system called PIX to set sizes and locations which look great on all devices 
		/// @see PIX
		SimpleButton(glm::vec2 loc, int sizeX, int sizeY) :Button(loc, sizeX, sizeY){};
		using list_itr = std::list<SimpleButton>::iterator;
		std::function<void(bool)> simpleButtonTouchCallback = nullptr;
		std::function<void()> touchUpdateCallback = nullptr;
		list_itr itr;
	};

	/// This is a helper struct inside AMP for TouchGesture
	/// 
	/// This struct contains all the information about a MultiButton
	/// Creates one big button for multiple elements, so when pressing the MultiButton, the button calculates which element inside the button you want to touch
	struct MultiButton
	{
		MultiButton(){};
		
		///Creates a MultiButton for multiple rendered texts
		///
		/// @param renderInfo The first element must be the highest text rendered on screen, the last element must be the lowest
		/// @note The renderInfo must be one block of texts rendered behind each other
		MultiButton(std::vector<TextRenderInformation> renderInfo) {
			if (renderInfo.size() == 0) return;
			else if (renderInfo.size() == 1) {
				button = Button(renderInfo[0]);
			}
			else {
				glm::vec2 downLeft = glm::vec2(renderInfo.back().locX, renderInfo.back().locY);
				int topNumLetters = renderInfo[0].text.getTextLength();
				glm::vec2 topRight = glm::vec2(renderInfo[0].locX + topNumLetters * renderInfo[0].fontsize, renderInfo[0].locY + renderInfo[0].fontsize);
				auto loc = button.GetCenter(downLeft,topRight);
				int SizeY = renderInfo[0].locY + renderInfo[0].fontsize - renderInfo.back().locY;
				int maxX = 0;
				for (auto& info : renderInfo) {
					YValues.push_back(std::pair(info.locY + info.fontsize, info.locY));
					int x = info.text.getTextLength() * info.fontsize;
					if (x > maxX)maxX = x;
				}
				button = Button(loc, maxX,SizeY);
				renderInformation = renderInfo;
			}
		}
		Button button;
		std::vector<std::pair<int, int>> YValues;
		std::vector<TextRenderInformation> renderInformation;

		/// @return The text information at the specified vector index
		TextRenderInformation At(int index) {
			return renderInformation[index];
		}

		/// @return the length of the texts vector
		int getLength() {
			return renderInformation.size();
		}

		/// This function sets a certain text element at index as pressed or default
		///
		/// @param rend This is the TextRenderer Component where the specified text is rendered
		/// @param index The element of the textRenderInformation vector you want to update
		/// @param type The new ColType for the text
		void SetPressed(TextRenderer* rend, int index, ColType type) {
			if (index != lastPressed && lastPressed >= 0)
			{
				auto& last = renderInformation[lastPressed];
				if (last.coltype != ColType::Default) {
					last.coltype = ColType::Default;
					rend->updateTextSettings(last);
				}
			}
			if (index >= 0) {
				auto& toUpdate = renderInformation[index];
				if (toUpdate.coltype != type) {
					toUpdate.coltype = type;
					rend->updateTextSettings(toUpdate);
				}
			}
			lastPressed = index;
		}

	private:
		int lastPressed = -1;

	};

	/// This is an AMP default component
	/// 
	/// This component allows you to interpret touch gestures
	/// Call the functions from this component inside the Actor class
	class TouchGesture : public Component
	{
		using buttonVec_t = std::list<SimpleButton>;
		using list_itr = buttonVec_t::iterator;
	public:
		TouchGesture(const TouchGesture&) = delete;
		TouchGesture(TouchGesture&&) = delete;
		TouchGesture() {
		};
		virtual ~TouchGesture(){};

		/// Sets up the touch gesture
		void SetUp();

		/// Creates a new Button
		/// 
		/// @param button Contains all information about size, location, ... of the button
		/// @param cb While the user is pressing the button you receive a callback, you can set the frequency with the SetButtonTimeOut() method
		void AddButton(Button button, std::function<void()> cb);

		/// Creates a new MultiButton
		/// 
		/// @param button Contains all information about the vector of the rendered text, their position, ... More Info : MultiButton
		/// @param cb While the user is pressing the button you receive a callback, you can set the frequency with the SetButtonTimeOut() method
		void AddButton(MultiButton button, std::function<void()> cb);

		/// Creates a new SimpleButton
		/// 
		/// @param button Contains all information about size, location, ... of the SimpleButton
		/// @param StartEndCallback This is a callback which is called on the start and on the end of the button press gesture
		/// @param StartEndCallback The function could look like : ```OnStartEndButtonPress(bool isStart)``` 
		/// @param TouchUpdateCallback While the user is pressing the button you can receive a callback, you can set the frequency with the SetButtonTimeOut() method
		/// @return This SimpleButton must be used to remove or update the button, using the parameter button does **not** work
		SimpleButton AddButton(SimpleButton button, std::function<void(bool)> StartEndCallback, std::function<void()> TouchUpdateCallback = nullptr);

		/// Removes a simple button, button have to exist
		///
		/// @param button The button received from the AddButton(SimpleButton, std::function<void(bool)>, std::function<void()>) method
		void RemoveSimpleButton(SimpleButton button);

		/// Removes a simple button if the button exist
		///
		/// @param button The button received from the AddButton(SimpleButton, std::function<void(bool)>, std::function<void()>) method
		/// @note You can call this function with an invalid button without throwing an exception
		void TryRemoveSimpleButton(SimpleButton button);

		/// Removes all currently active buttons
		void RemoveAllSimpleButtons();

		/// @return The last touch x location in pixels
		/// @note The coordinate origin is bottom left of the screen
		int GetX() { return touchX; };

		/// @return The last touch y location in pixels
		/// @note The coordinate origin is bottom left of the screen
		int GetY() { return engine->ampContext.getHeight() - touchY; };

		/// @return The last touch location as vector in pixels
		/// @note The coordinate origin is bottom left of the screen
		glm::vec2 GetTouch() { return glm::vec2(GetX(), GetY()); };

		/// Removes a Button
		/// 
		/// @param button Use the button you specified when adding the button with AddButton(Button, std::function<void()>)
		void RemoveButton(Button button);

		/// Removes a MultiButton
		/// 
		/// @param button Use the button you specified when adding the MultiButton with AddButton(MultiButton, std::function<void()>)
		void RemoveButton(MultiButton button);

		/// Sets the frequency of the touch callback
		///
		/// @param timeInSec Frequency in seconds
		void SetButtonTimeOut(float timeInSec) { buttonTimeOutInSec = timeInSec; };

		/// @return The frequency of the touch callback
		float GetButtonTimeOut() { return buttonTimeOutInSec; };

		/// @return The currently touched text index of Text Render vector
		int GetTouchedIndex(MultiButton textInfo);

		std::list<SimpleButton> simpleButtonList;
	protected:
		void update() override{};


		virtual void OnActivate() override;


		virtual void OnDeactivate() override;

	private:
		template<typename ITR, typename Container>
		bool IsItrValid(ITR&& CompareItr, Container&& container)
		{
			auto currentItr = container.begin();
			auto end = container.end();
			for (; currentItr != end; ++currentItr) {
				if (currentItr == CompareItr)return true;
			}
			throw "Invalid Itr, probably you have erased one element multiple times";
			return false;
		};

		int touchX = 0;
		int touchY = 0;
		float buttonTimeOutInSec = 0.1f;
		bool isButtonTimeOut = false;
		void ResetButtonTimeOut();
		void ToAndroidCoords(Button& button);
		void initOnTouch();
		void OnStartTouch(int32_t x, int32_t y);
		void OnTouch(int32_t x, int32_t y);
		void OnEndTouch(int32_t x, int32_t y);
		void CheckButtons(int32_t x, int32_t y);

		void SetTouch(int32_t x, int32_t y);

		bool isPressed(Button& b);

		void CheckSimpleButton(int32_t x, int32_t y);

		void ResetCurrentSimpleButton();

		void SetTimeOut();
		std::vector<Button> buttonVec;
		
		list_itr lastSimpleButton;
		bool isSimpleButtonNull = true;

		callbacksIter startTouch;
		callbacksIter touchUpdate;
		callbacksIter endTouch;
	};
}

