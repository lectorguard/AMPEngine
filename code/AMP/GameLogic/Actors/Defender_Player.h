#pragma once
#include "Actor.h"
#include "AMP_Engine.h"
#include "PickUpActor.h"
#include "Static/Camera.h"
#include "Tasks/CharacterTouch.h"
#include "GameUtility.h"
#include "Ground.h"
#include "utility"

class Defender_Player : public amp::Actor
{
public:
	Defender_Player() {};
	Defender_Player(const Defender_Player&) = delete;
	Defender_Player(Defender_Player&&) = delete;
	virtual ~Defender_Player();

	PickUpActor* pickUpActor = nullptr;
	amp::DynamicMesh* character = nullptr;
	amp::Animation* characterAnim = nullptr;
	Ground* ground = nullptr;

	const float X_DeltaFallback = 28.0f;
	const float JumpForce = 11.0f;
	const float HitForce = 300000.0f;

	//EVENTS
	//INPUTS
	TEXT startDefendPlayer = TEXT("startDefendPlayer");
	TEXT resetDefendPlayer = TEXT("resetDefendPlayer");
	//OUTPUTS
	TEXT endDefendPlayer = TEXT("endDefendPlayer");

	virtual void OnActivate() override;

	virtual void SubscribeGameEvents();

	virtual void OnDeactivate() override;

	virtual void AfterWindowStartup() override;

	void OnPostInit();
	
	virtual void StartDefending();

	void StartCollissionCheck();

	void InitCamera();

	void Jump(glm::vec3 upForce, float targetZlocation);

	void StartRespawnAndOrientationCheck();

	void OnPlayerReachedStartLine();

	void ResetDefendingPlayer();

	virtual void ResetLocalVariables();

	void OnCollissionGround();

	void OnCharacterRelocate();

	virtual void Update() override;


	//PickUp Actor EVENTS
	virtual void OnDirCalculated(Move m);
	void OnEndTouch();
	virtual void OnBottleHit();

	void StartCharacterIsCloseToBottleCheck();

	void StartInputCheck();

	virtual void StartCharacterTouch();

	void StartRunning(bool isForward,float Zlocation);
	void StopRunning();

	virtual void OnRotateFinished();

	void GoBackToStartLine();

	void OnCollisionBody();
	void LaneChangeEnded(float targetZlocation);
	void OnMeshesClose();

	void ResetBottleSpectator();

protected:
	glm::vec3 startLocation = glm::vec3(-190.59f, 0.0f, 0.0f);
	
	float lastLaneLocation_Z = 0.0f;

	bool touchEnded = true;
	bool laneChangeEnded = true;
	bool canJump = true;
	DefendingStages currentStage = WaitingForHit;

};

