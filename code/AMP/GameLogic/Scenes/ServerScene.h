#pragma once
#include "Scene.h"
#include "AMP_Engine.h"
#include "Static/Camera.h"
#include "Graphics/CubeMap.h"
#include "Actors/PickUpActor.h"
#include "Actors/ThrowingActor.h"
#include "Actors/Ground.h"
#include "Actors/Attacker_Player.h"
#include "Actors/Attacker_Network.h"
#include "Actors/Defender_Player.h"
#include "Actors/Defender_Network.h"
#include "Actors/PvP_Game.h"
#include "Actors/ThrowingUI.h"
#include "Actors/Environment.h"
#include "Actors/SendCurrentState.h"
#include "Actors/GameSound.h"


class ServerScene : public amp::Scene
{
public:
	GameSound* gameSound = nullptr;
	PickUpActor* anim = nullptr;
	ThrowingActor* bottleActor = nullptr;
	Ground* ground = nullptr;
	//Player
	Attacker_Player* attackPlayer = nullptr;
	Attacker_Network* attackNetwork = nullptr;
	Defender_Player* defenderPlayer = nullptr;
	Defender_Network* defenderNetwork = nullptr;
	//Game
	PvP_Game* game = nullptr;
	Environment* env = nullptr;
	ThrowingUI* throwUI = nullptr;
	SendCurrentState* sendState = nullptr;
	ServerScene(amp::AMP_Engine* engine) : amp::Scene(engine) {};

protected:
	void load() override
	{
		sendState = CreateActor<SendCurrentState>(TEXT("sendingCurrentState"));
		anim = CreateActor<PickUpActor>(TEXT("PickUpActor"));
		defenderNetwork = CreateActor<Defender_Network>(TEXT("Server_Defender_Network"));
		defenderPlayer = CreateActor<Defender_Player>(TEXT("Server_Defender_Player"));
		attackNetwork = CreateActor<Attacker_Network>(TEXT("Server_Attacker_Network"));
		attackPlayer = CreateActor<Attacker_Player>(TEXT("Server_Attacker_Player"));
		game = CreateActor<PvP_Game>(TEXT("Server_PvP_Game"));
		bottleActor = CreateActor<ThrowingActor>(TEXT("ThrowingActor"));
		// Environment
		env = CreateActor<Environment>(TEXT("environment"),amp::Type::Static);
		ground = CreateUIActor<Ground>(TEXT("GroundActor"));
		ground->SetActive(false);
		//Sound and UI
		gameSound = CreateActor<GameSound>(TEXT("gameSound"));
		throwUI = CreateUIActor<ThrowingUI>(TEXT("ThrowingUI"));
	};
};


