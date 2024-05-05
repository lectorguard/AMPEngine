#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "GameUtility.h"
#include "Actors/PickUpActor.h"
#include "Actors/ThrowingActor.h"
#include "Tasks/ProgressBars.h"
#include "Components/NetworkUDP.h"

class SendGameState : public amp::Task
{

public:
	PickUpActor* pickUpActor = nullptr;
	ThrowingActor* throwingActor = nullptr;
	amp::NetworkUDP* networking = nullptr;

	SendGameState() {
	};
	virtual ~SendGameState() {};

	SendGameState(const SendGameState&) = delete;
	SendGameState(SendGameState&&) = delete;

	//Server is always Player ONE, client Player TWO


	//EVENTS
	//INPUTS
	TEXT SendGameStateReset = TEXT("SendGameStateReset");	//No Params: RESET has always 0 params
	TEXT startSendGameState = TEXT("startSendGameState");   //One Param Direction
	TEXT stopSendGameState = TEXT("stopSendGameState");		//No Param

	//Update state
	TEXT updateAttackingStage = TEXT("updateAttackingStage"); //One Param: AttackingStages stage
	TEXT updateDefendingStage = TEXT("updateDefendingStage"); //One Param: DefendingStages stage
	TEXT updateGameState	  = TEXT("updateGameState");	  //One Param: GameStagesOffline stage

	TEXT sendInputFlags = TEXT("sendInputFlags"); //One Param: uint8_t flags
	
	//Receive New Data

	virtual void Init() override;

	void PostInit();

	void StartSendingGameState(amp::NetworkUDP* network);

	virtual void Update() override;

	void SetUpCurrentState(CurrentState& state);

	virtual void End() override;

	virtual void Reset() override;

	void OnChangingProgressServer(Player player, float currentProgress);

	void UpdateAttackingStage(AttackingStages stage);

	void UpdateDefendingStage(DefendingStages stage);

	void UpdateGameState(GameStagesOffline stage);

	void SendInputFlags(uint8_t flags);

	void AddSoundFlags(uint8_t flags);

	void SetSoundFlags(uint8_t flags);

private:
	CurrentState state;
	float drunkLevelClient = 1.0f;
	float drunkLevelServer = 1.0f;
	uint8_t soundFlags = 0u;
};

