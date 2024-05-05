#pragma once
#include "Scene.h"
#include "AMP_Engine.h"
#include "Actors/MenuUI.h"
#include "Actors/MatchMaker.h"


class Menu : public amp::Scene
{
public:
	MenuUI* menuActor = nullptr;
	MatchMaker* matchMakerActor = nullptr;
public:
	Menu(amp::AMP_Engine* engine) :amp::Scene(engine) {

	}

protected:
	void load() override
	{
		menuActor = CreateUIActor<MenuUI>(TEXT("MenuUI"));
		matchMakerActor = CreateUIActor<MatchMaker>(TEXT("MatchMaking_Actor"));
	}
};


