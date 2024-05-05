#pragma once
#include "Scene.h"
#include "AMP_Engine.h"
#include "Static/Camera.h"
#include "Graphics/CubeMap.h"
#include "Actors/PickUpActor.h"
#include "Actors/ThrowingActor.h"
#include "Actors/Ground_Client.h"
#include "Actors/ThrowingUI.h"
#include "Actors/Environment.h"
#include "Actors/ProcessCurrentState.h"
//Player and Game
#include "Actors/Attacker_Streamed.h"
#include "Actors/Defender_Streamed.h"
#include "Actors/PvP_Game_Streamed.h"
//Sound
#include "Actors/GameSound.h"


class ClientScene : public amp::Scene
{
public:
	PickUpActor* anim = nullptr;
	amp::CubeMap* cubeMap = nullptr;
	ThrowingActor* bottleActor = nullptr;
	Ground_Client* ground = nullptr;
	//Game and Player
	Attacker_Streamed* attacker = nullptr;
	Defender_Streamed* defender = nullptr;
	PvP_Game_Streamed* game = nullptr;
	
	Environment* env = nullptr;
	ThrowingUI* throwUI = nullptr;
	ProcessCurrentState* processState = nullptr;
	GameSound* gameSound = nullptr;
public:
	ClientScene(amp::AMP_Engine* engine) :amp::Scene(engine) {
		
	}

protected:
	void load() override
	{
		processState = CreateActor<ProcessCurrentState>(TEXT("processNetworkState"));
		anim = CreateActor<PickUpActor>(TEXT("PickUpActor"));
		bottleActor = CreateActor<ThrowingActor>(TEXT("ThrowingActor"));
		//Player And Game
		attacker = CreateActor<Attacker_Streamed>(TEXT("Client_Attacker_Streamed"));
		defender = CreateActor<Defender_Streamed>(TEXT("Client_Defender_Streamed"));
		game = CreateActor<PvP_Game_Streamed>(TEXT("Client_Game_PvP_Streamed"));
		// Environment
		env = CreateActor<Environment>(TEXT("environment"),amp::Type::Static);
		ground = CreateUIActor<Ground_Client>(TEXT("GroundActor_Client"));
		ground->SetActive(false);
		// Sound and UI
		gameSound = CreateActor<GameSound>(TEXT("gameSound"));
		throwUI = CreateUIActor<ThrowingUI>(TEXT("ThrowingUI"));
	}
};


