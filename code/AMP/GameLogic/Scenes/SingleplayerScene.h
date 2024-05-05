#pragma once
#include "Scene.h"
#include "AMP_Engine.h"
#include "Static/Camera.h"
#include "Graphics/CubeMap.h"
#include "Actors/PickUpActor.h"
#include "Actors/ThrowingActor.h"
#include "Actors/Ground.h"
#include "Actors/Attacker_Player.h"
#include "Actors/Defender_CPU.h"
#include <Actors\Defender_Player.h>
#include "Actors\Attacker_CPU.h"
#include "Actors/PvE_Game.h"
#include "Actors/ThrowingUI.h"
#include "Actors/Environment.h"
#include "Utility/DebugAndTesting.h"
#include "Actors/GameSound.h"


class SingleplayerScene : public amp::Scene
{
public:
	PickUpActor* anim = nullptr;
	ThrowingActor* bottleActor = nullptr;
	Ground* ground = nullptr;
	//Player
	Attacker_Player* attackPlayer = nullptr;
	Defender_CPU* defenderCPU = nullptr;
	Attacker_CPU* attackerCPU = nullptr;
	Defender_Player* defenderPlayer = nullptr;
	//Game
	PvE_Game* pveGame = nullptr;
	Environment* env = nullptr;
	ThrowingUI* throwUI = nullptr;
	amp::DebugAndTesting* test = nullptr;
	GameSound* gameSound = nullptr;

public:
	SingleplayerScene(amp::AMP_Engine* engine) :amp::Scene(engine) {		
	}

protected:
	void load() override
	{
		anim = CreateActor<PickUpActor>(TEXT("PickUpActor"));
		defenderCPU = CreateActor<Defender_CPU>(TEXT("Defender_CPU"));
		defenderPlayer = CreateActor<Defender_Player>(TEXT("Defender_Player"));
		attackerCPU = CreateActor<Attacker_CPU>(TEXT("Attacker_CPU"));
		attackPlayer = CreateActor<Attacker_Player>(TEXT("attackPlayer"));
		pveGame = CreateActor<PvE_Game>(TEXT("pveGame"));
		bottleActor = CreateActor<ThrowingActor>(TEXT("ThrowingActor"));
		// Environment
		env = CreateActor<Environment>(TEXT("environment"),amp::Type::Dynamic);
		ground = CreateUIActor<Ground>(TEXT("GroundActor"));
		ground->SetActive(false);
		// Sound and UI
		gameSound = CreateActor<GameSound>(TEXT("gameSound"));
		throwUI = CreateUIActor<ThrowingUI>(TEXT("ThrowingUI"));

		// For testing
		//test = CreateUIActor<amp::DebugAndTesting>(TEXT("test_actor"), amp::Type::Dynamic);
	}
};

