#pragma once
#include "Actor.h"
#include "AMP_Engine.h"
#include "PickUpActor.h"
#include "Static/Camera.h"
#include "Tasks/CharacterTouch.h"
#include "GameUtility.h"
#include "Ground_Client.h"
#include "utility"

class Defender_Streamed : public amp::Actor
{
public:
	Defender_Streamed() {};
	Defender_Streamed(const Defender_Streamed&) = delete;
	Defender_Streamed(Defender_Streamed&&) = delete;
	virtual ~Defender_Streamed();

	PickUpActor* pickUpActor = nullptr;
	amp::DynamicMesh* character = nullptr;
	Ground_Client* ground = nullptr;

	//EVENTS
	//INPUTS
	TEXT startStreamDefend = TEXT("startStreamedDefender");
	TEXT resetStreamDefend = TEXT("resetStreamedDefender");
	//OUTPUTS
	TEXT endStreamDefend = TEXT("endStreamDefender");

	virtual void OnActivate() override;

	virtual void OnDeactivate() override;

	virtual void AfterWindowStartup() override;

	void OnPostInit();

	void StartDefending();

	void OnRunToBottle();

	void InitCamera();

	void ResetDefendingStream();

	void OnGoBackToStart();


	//PickUp Actor EVENTS
	void OnDirCalculated(Move m);
	void OnEndTouch();
	void StartCharacterTouch();


private:
	DefendingStages currentStage = WaitingForHit;
	bool inputInverted = true;

};


