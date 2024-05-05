#pragma once

#include "Actor.h"
#include "Text.h"
#include "AMP_Engine.h"
#include "Event/EventSystem.h"
#include "Components/ProgressBar.h"
#include "Components/TouchGesture.h"
#include "../glm/gtc/constants.hpp"
#include "Tasks/IconTouch.h"
#include "Tasks/ProgressBars.h"
#include "Tasks/WinLoose.h"
#include "Tasks/DisplayInfos.h"
#include "Components/Image.h"

class ThrowingUI : public amp::Actor
{

public:

	amp::ProgressBar* one = nullptr;
	amp::ProgressBar* two = nullptr;
	amp::Image* image = nullptr;
	amp::TouchGesture* touch = nullptr;
	amp::Image* finishImages = nullptr;

	amp::TaskController* controller = nullptr;

	ThrowingUI() {
		one = AddComponent<amp::ProgressBar>(TEXT("PlayerOneBar"));
		two = AddComponent<amp::ProgressBar>(TEXT("PlayerTwoBar"));
		image = AddComponent<amp::Image>(TEXT("BeerIconImages"));
		touch = AddComponent<amp::TouchGesture>(TEXT("TouchForIcon"));
		finishImages = AddComponent<amp::Image>(TEXT("FinishImages"));
		controller = CreateTaskController();
	};

	virtual ~ThrowingUI() {};

	void Update() override;

	void AfterWindowStartup() override;

	virtual void BeforeSystemShutdown() override;
};



