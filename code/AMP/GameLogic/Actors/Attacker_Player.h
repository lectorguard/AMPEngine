#pragma once
#include "Actor.h"
#include "AMP_Engine.h"
#include "ThrowingActor.h"
#include "Static/Camera.h"
#include "Tasks/BottleValidityCheck.h"
#include "GameUtility.h"
#include "Tasks/ProgressBars.h"

class Attacker_Player : public amp::Actor
{
public:
	Attacker_Player(){};
	Attacker_Player(const Attacker_Player&) = delete;
	Attacker_Player(Attacker_Player&&) = delete;
	virtual ~Attacker_Player(){};

	ThrowingActor* throwActor = nullptr;
	amp::DynamicMesh* R_ThrowBottle = nullptr;
	amp::DynamicMesh* R_TargetBottle = nullptr;

	//EVENTS 
	//INPUT
	TEXT startAttackPlayer = TEXT("startAttackPlayer");
	TEXT resetAttackPlayer = TEXT("resetAttackPlayer");

	//Output
	TEXT endAttackPlayer = TEXT("endAttackPlayer");


	virtual void AfterWindowStartup() override;

	virtual void OnActivate() override;

	virtual void SubscribeGameEvents();

	virtual void OnDeactivate() override;
	
	void OnPostInit();

	void SetBottleToCamera();

	virtual void StartAttackPlayer();

	void ResetAttackPlayer();

	void SetCamera();

	//EVENTS

	virtual void onThrowInput(amp::DynamicMesh* bottle, glm::vec3 force);

	void onInvalidBottleThrow();

	void onHit();

	virtual void StartAttackDefendingPlayerState();

	virtual void ThrowAttackBottle();

	void onResetThrow();

	virtual void onInvalid(amp::DynamicMesh* mesh, InvalidType type);

	virtual void onIconTouch();

protected:
	AttackingStages  currentStage = NoAttack;
	bool CanThrow = false;
	bool hasHitBottle = false;
	int AttackBottleIndex = 0;

	glm::mat4 throwBottleTrans; 
};

