#pragma once
#include "Actor.h"
#include "AMP_Engine.h"
#include "Components/TextRenderer.h"
#include "Components/Image.h"
#include "Components/TouchGesture.h"
#include "Event/EventSystem.h"
#include <string>
#include "Text.h"
namespace amp
{
	
	enum Action {
		Translate = 0, Rotate = 1, Scale = 2 
	};
	
	enum Axis {
		X = 0, Y =1, Z = 2
	};
	
	
	class DebugAndTesting : public amp::Actor
	{
	public:
		 amp::TextRenderer* rend = nullptr;
		 amp::Image* img = nullptr;
		 amp::TouchGesture* gest = nullptr;
		 DebugAndTesting(amp::Type type) :Actor(type) {
			rend = AddComponent<amp::TextRenderer>(TEXT("Renderer"));
			img = AddComponent<amp::Image>(TEXT("img"));
			gest = AddComponent<amp::TouchGesture>(TEXT("gest"));
		 }
		 amp::ImageInfo info;
		 amp::Button b;
		 amp::MultiButton multiBut;
		 
	
		 virtual void OnActivate() override;
	
		 virtual void OnDeactivate() override;
	
		 void PostInit();
	
		 void DrawOutliner();
	
		 void UpdateOutlinerSelection(int actorIndex);
	
		 void AddOutlineComponent(amp::Component* comp, int outlineIndex, std::vector<amp::TextRenderInformation>& TextOutliner);
	
		 void AddOutlineActor(int i, int outlineIndex, std::vector<amp::TextRenderInformation>& TextOutliner);
	
		 std::vector<TEXT> names;
	private:
		
		amp::TextRenderInformation translate;
		amp::TextRenderInformation rotate;
		amp::TextRenderInformation scale;
		amp::TextRenderInformation Camera;
		amp::SliderInfo sensitivity;
		amp::SliderInfo step;
		amp::ImageInfo	bot;
		amp::ImageInfo	left;
		amp::ImageInfo	top;
		amp::ImageInfo	right;
		amp::ImageInfo	forw;
		amp::ImageInfo	backw;
		amp::ImageInfo  menu;
	
	
		Action currentAct = Action::Translate; 
	
		 void OnMultiButton();
	
		 void ReDrawOutliner();
	
		 void Forward();
	
		 void UpdateCameraTransform(Axis axis, float delta);
	
		 void UpdateCurrentMatrix(Axis axis, float delta);
	
		 void UpdateActorTransform(Axis axis, float delta);
	
		 void UpdateComponentTransform(Axis axis, float delta);
	
		 void SetCurrentMatrix();
	
		 void UpdateLocRotScale();
	
		 void UpdateSensitivity();
	
		 void UpdateStep();
	
		 void Backward();
		 void Left();
		 void Right();
		 void Up();
		 void Down();
	
		 void SetTranslate();
		 void SetRotate();
		 void SetScale();
	
		 std::string GetDecimals(float f, int decimals);
	
		 void Update() override;
	
		 void GetLocRotScaleAsString(glm::mat4 matrix, std::string& locS, std::string& rotS, std::string& scaleS);
	
		 void OnButton();
	
		 void AfterWindowStartup() override;
	
		 void SetCurrentActor(int actorIndex);
	
		 void OnCamera();
	
		 void OnMenu(bool isFirst);
		
		 void SetCurrentComponent(int actorIndex, int compIndex, int pressedIndex);
		 // World outline properties
		 int posX = 600;
		 int posXComp = 700;
		 int posY = 1600;
		 int size = 55;
		 int padding = 20;
		 bool isOutlinerDisplayed = true;
		 //
		 
		 float StepValue = 1.0f;
		 int currentActor = -1;;
		 int currentComponent = -1;
		 bool isCamera = true;
		 glm::mat4 currentMatrix = glm::mat4(1.0f);
	};
}

