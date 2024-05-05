#pragma once

#include "Actor.h"
#include "Text.h"
#include "AMP_Engine.h"
#include "Event/EventSystem.h"
#include "Components/TouchGesture.h"
#include "../glm/gtc/constants.hpp"
#include "Static/Pixel.h"
#include "Sound/SoundPool.h"
#include "Components/Image.h"

class MenuUI : public amp::Actor
{

public:
	amp::Image* image = nullptr;
	amp::TouchGesture* touch = nullptr;

	MenuUI() {
		image = AddComponent<amp::Image>(TEXT("MenuImages"));
		touch = AddComponent<amp::TouchGesture>(TEXT("TouchForIcon"));
	};

	amp::SimpleButton onlineBut;
	amp::SimpleButton tutorialBut;

	virtual ~MenuUI() {};

	void Update() override;

	void AfterWindowStartup() override;

	virtual void BeforeSystemShutdown() override;

	void OnOnlineGame(bool isFirst);

	void OnTutorialGame(bool isFirst);

private:
	int buttonSound = -1;
};




