#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "GameUtility.h"
#include "Actors/PickUpActor.h"
#include "Actors/ThrowingActor.h"
#include "Tasks/ProgressBars.h"
#include "Components/NetworkUDP.h"

class ReceiveGameState : public amp::Task
{

public:
	PickUpActor* pickUpActor = nullptr;
	ThrowingActor* throwingActor = nullptr;
	amp::NetworkUDP* networking = nullptr;

	ReceiveGameState() {
	};
	virtual ~ReceiveGameState() {};

	ReceiveGameState(const ReceiveGameState&) = delete;
	ReceiveGameState(ReceiveGameState&&) = delete;

	//Server is always Player ONE, client Player TWO


	//EVENTS
	//INPUTS
	TEXT receiveGameStateReset = TEXT("ReceiveGameStateReset");	//No Params: RESET has always 0 params
	TEXT startReceiveGameState = TEXT("startReceiveGameState");   //One Param Direction
	TEXT stopReceiveGameState = TEXT("stopReceiveGameState");		//No Param

	TEXT setBottleCounter = TEXT("setBottleCounter"); // One Param : int counter
	


	virtual void Init() override;

	void LoadSounds();

	void PostInit();

	void StartReceivingGameState(amp::NetworkUDP* network);

	virtual void Update() override;

	void CallFirstNetworkEvent();

	void UpdateGameScene(CurrentState& s);

	void SetAnimation(CurrentState& s);

	void UpdateSpotlight(CurrentState& s);

	void UpdateDefendingPlayer(CurrentState& s);

	void SetBottles(CurrentState& s);

	void ClearExecutedInputFlags(CurrentState& s);

	void SetNewRBTrans(amp::DynamicMesh* mesh, glm::mat4 trans);

	void UpdateGameState(CurrentState& s);

	void UpdateProgress(CurrentState& s);

	void UpdateSound(CurrentState& s);

	void CallDefendEvent(DefendingStages ds);

	void CallAttackEvent(AttackingStages as);

	virtual void End() override;

	virtual void Reset() override;

	void SetBottleCounter(int counter);

	void EndSounds();

	CurrentState lastGameState;
	const int numLastDefTrans = 5;
	std::vector<glm::mat4> lastDefendingTransforms;
	int bottleCounter = 0;
	bool isFirst = true;
private:
	uint8_t soundFlags = 0u;
	int hitSoundIndex[4] = { 0 };
	int rollSoundIndex[2] = { 0 };
	int jumpSoundIndex = 0;
	int fallbackSoundIndex = 0;
};


