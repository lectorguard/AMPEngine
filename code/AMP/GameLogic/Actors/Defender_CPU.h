#pragma once
#include "Actor.h"
#include "AMP_Engine.h"
#include "PickUpActor.h"
#include "Static/Camera.h"
#include "GameUtility.h"

class Defender_CPU : public amp::Actor
{
public:
	Defender_CPU() {};
	Defender_CPU(const Defender_CPU&) = delete;
	Defender_CPU(Defender_CPU&&) = delete;
	virtual ~Defender_CPU() {};

	PickUpActor* pickUpActor = nullptr; 
	amp::DynamicMesh* character = nullptr;
	amp::Animation* characterAnim = nullptr;

	//EVENTS
	//INPUT
	TEXT startDefendingCPU = TEXT("startDefendingCPU");
	TEXT resetDefendingCPU = TEXT("resetDefendingCPU");
	//OUTPUT
	TEXT endDefendingCPU = TEXT("endDefendingCPU");

	virtual void OnActivate() override;

	virtual void OnDeactivate() override;

	virtual void AfterWindowStartup() override;

	void StartDefendingCPU();

	void OnPostInit();

	void OnMeshesClose();

	void ResetDefendingCPU();

	void ResetLocalVariables();

	virtual void Update() override;

	void MoveCharacter(glm::vec3 dirForce);

	void SetForwardVectorToBottle();

	void CheckReachedEndLine();

	void OnBottleHit();

	void OnRotateFinished();

	void OnBottleLocationUpdate(glm::vec3 loc);

	void OnCollisionBody();

	void OnRelocate();

private:
	glm::vec3 forwardForce = glm::vec3(15, 0, 0);
	glm::vec3 backwardForce = glm::vec3(-15, 0, 0);
	DefendingStages currentStage = WaitingForHit;

	float X_Fallback = 50.0f;
};
