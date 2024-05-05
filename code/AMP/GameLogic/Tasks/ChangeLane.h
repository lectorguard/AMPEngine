#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Tasks/CharacterTouch.h"
#include "Event/EventSystem.h"
#include "Actors/PickUpActor.h"
#include "Components/DynamicMesh.h"
#include "Static/Camera.h"


class ChangeLane : public amp::Task
{

public:
	amp::DynamicMesh* character = nullptr;
	ChangeLane(amp::DynamicMesh* character) {
		this->character = character;
	}
	virtual ~ChangeLane() {};

	ChangeLane(const ChangeLane&) = delete;
	ChangeLane(ChangeLane&&) = delete;

	//EVENTS
	//INPUTS
	TEXT ChangeLaneReset = TEXT("ChangeLaneReset");			//No Params: RESET has always 0 params
	TEXT StartChange = TEXT("StartLaneChange");				//One Param: Move (enum)
	//OUTPUTS
	TEXT endLaneChange =	TEXT("EndLaneChange");				//One Param: float targetZLocation
	TEXT invalidLaneChange = TEXT("InvalidLaneChange");		//No params
	TEXT jumpEvent =		TEXT("JumpEvent");						//Two Param: glm::vec3, float TargetZLocation


	virtual void Init() override;

	virtual void Update() override;

	void FixCharacterZLocation();

	virtual void End() override;

	void InvalidLaneChange();

	void StartLaneChange(Move m);

	bool IsChangedLaneValid(float laneWidth);

	void JumpEvent(glm::vec3 upForce);

	float getTargetZLocation() { return targetZlocation; };


	virtual void Reset() override;

private:
	Move currentMove = NoMove;
	float targetZlocation = 0.0f;
	glm::vec3 currentChangingVector;
	bool isTargetLaneGreater;
	int maxLanesPerSide = 4;

	int jumpSoundIndex = -1;
	int laneChangeSoundIndex = -1;
};

