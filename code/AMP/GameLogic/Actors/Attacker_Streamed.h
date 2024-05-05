#pragma once
#include "Actor.h"
#include "AMP_Engine.h"
#include "ThrowingActor.h"
#include "Static/Camera.h"
#include "Tasks/BottleValidityCheck.h"
#include "GameUtility.h"
#include "Ground_Client.h"
#include "Tasks/ProgressBars.h"

class Attacker_Streamed : public amp::Actor
{
public:
	Attacker_Streamed() {};
	Attacker_Streamed(const Attacker_Streamed&) = delete;
	Attacker_Streamed(Attacker_Streamed&&) = delete;
	virtual ~Attacker_Streamed() {};

	Ground_Client* ground = nullptr;
	ThrowingActor* throwActor = nullptr;
	amp::DynamicMesh* R_ThrowBottle = nullptr;
	amp::DynamicMesh* R_TargetBottle = nullptr;

	//EVENTS 
	//INPUT
	TEXT startStreamAttackPlayer = TEXT("startStreamedAttacker");
	TEXT resetStreamAttackPlayer = TEXT("resetStreamedAttacker");

	//Output
	TEXT endStreamAttackPlayer = TEXT("endStreamAttackPlayer");


	virtual void AfterWindowStartup() override;

	virtual void OnActivate() override;

	virtual void OnDeactivate() override;

	void OnPostInit();

	void SetBottleToCamera();

	void StartAttackPlayer();

	void OnAttackCenterBottle();

	void ResetAttackPlayer();

	void SetCamera();

	void OnInvalidBottleThrow();

	//EVENTS

	void onThrowBottle(amp::DynamicMesh* bottle, glm::vec3 force);

	void StartAttackDefendingPlayerState();

	void ThrowAttackBottle();

	void onResetThrow();


private:
	amp::DynamicMesh* currentBottle = nullptr;
	AttackingStages  currentStage = NoAttack;
	bool CanThrow = true;
	int AttackBottleIndex = 0;
	int bottleThrowCounter = 0;
	glm::mat4 throwBottleTransform = glm::mat4(1.0f); 
};

