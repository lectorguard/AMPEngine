#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/DynamicMesh.h"

class Running : public amp::Task
{

public:
	amp::DynamicMesh * character = nullptr;
	Running(amp::DynamicMesh* character){
		this->character = character;
	};
	virtual ~Running() {};

	Running(const Running&) = delete;
	Running(Running&&) = delete;

	//RunningVariables
	const float ForwardForce = 18000.0f;
	const float MaxForwardVelocity = 35.0f;

	//EVENTS
	//INPUTS
	TEXT RunningReset = TEXT("RunningReset"); //No Params: RESET has always 0 params
	TEXT startRunning = TEXT("startRunning");   //One Param Direction
	TEXT stopRunning = TEXT("stopRunning");		//No Param

	

	virtual void Init() override;


	void StartRunning(bool isForward);


	virtual void Update() override;

	void UpdateCharacterSpeed();


	virtual void End() override;


	virtual void Reset() override;

private:
	bool isForward = true;
};

