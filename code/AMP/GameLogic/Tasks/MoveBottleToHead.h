#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/DynamicMesh.h"

class MoveBottleToHead : public amp::Task
{
public:
	amp::DynamicMesh* bottle = nullptr;
	amp::DynamicMesh* character = nullptr;
	MoveBottleToHead(amp::DynamicMesh* targetBottle) {
		assert(targetBottle);
		this->bottle = targetBottle;
	};
	virtual ~MoveBottleToHead() {};

	MoveBottleToHead(const MoveBottleToHead&) = delete;
	MoveBottleToHead(MoveBottleToHead&&) = delete;

	const float HeightOverCharacter = 25.0f;
	const float speedInMeterPerSecond = 45.0f;

	//EVENTS
	//INPUTS
	TEXT MoveBottleToHeadReset = TEXT("MoveBottleToHeadReset");		//No Params: RESET has always 0 params
	TEXT startMoveBottleToHead = TEXT("startMoveBottleToHead");   //One Param: amp::DynamicMesh* character
	TEXT endMoveBottleToHead = TEXT("endMoveBottleToHead");		  //No Param
	//OUTPUT
	TEXT onBottleHit = TEXT("onBottleHit");					  //No Params

	virtual void Init() override;

	void StartMoveBottle(amp::DynamicMesh* character);

	virtual void Update() override;

	virtual void End() override;

	virtual void Reset() override;
private:
	bool hasReachedTarget = false;
};
