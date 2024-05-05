#pragma once
#include "Actor.h"
#include "AMP_Engine.h"
#include "ThrowingActor.h"
#include "Tasks/BottleValidityCheck.h"
#include "Tasks/ProgressBars.h"
#include "GameUtility.h"

class Attacker_CPU : public amp::Actor
{
public:
	Attacker_CPU() {};
	Attacker_CPU(const Attacker_CPU&) = delete;
	Attacker_CPU(Attacker_CPU&&) = delete;
	virtual ~Attacker_CPU() {};

	ThrowingActor* throwActor = nullptr;
	amp::DynamicMesh* R_ThrowBottle = nullptr;
	amp::DynamicMesh* R_TargetBottle = nullptr;

	//EVENTS
	//INPUT ///NO PARAMS FOR ALL
	TEXT startAttackerCPU = TEXT("startAttackerCPU");
	TEXT resetAttackerCPU = TEXT("resetAttackerCPU");
	//OUTPUT
	TEXT endAttackerCPU = TEXT("endAttackerCPU");

	virtual void AfterWindowStartup() override;

	virtual void OnActivate() override;

	virtual void OnDeactivate() override;

	void OnPostInit();

	void StartAttackCPU();

	void StartThrowBottles();

	void ThrowBottle(amp::DynamicMesh* bottle,glm::vec3 force);

	void ResetAttackCPU();

	//EVENTS

	void onHit();

	void CreateDrinkingTimer();

	void TimeAttackBottleThrow();

	void ThrowAttackBottle();

	void onInvalid(amp::DynamicMesh* mesh, InvalidType type);

	void drinkBeer();

private:
	AttackingStages  currentStage = NoAttack;
	glm::vec3 bottleHitForce[3] = { glm::vec3(-5000.0f, 50.0f, 0),glm::vec3(-5000.0f, 50.0f, 1000.0f),glm::vec3(-5000.0f, 50.0f, -1000.0f) };
	int AttackBottleIndex = 0;
};

