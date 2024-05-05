#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "GameUtility.h"
#include "Components/NetworkUDP.h"

class ReceiveInput : public amp::Task
{

public:
	amp::NetworkUDP* networking = nullptr;

	ReceiveInput() {
	};
	virtual ~ReceiveInput() {};

	ReceiveInput(const ReceiveInput&) = delete;
	ReceiveInput(ReceiveInput&&) = delete;

	//EVENTS
	//INPUTS
	TEXT ReceiveInputReset = TEXT("ReceiveInputReset");	//No Params: RESET has always 0 params
	TEXT startReceiveInput = TEXT("startReceiveInput");   //One Param Direction
	TEXT stopReceiveInput = TEXT("stopReceiveInput");		//No Param

	//OUTPUTS
	TEXT throwAttackBottle = TEXT("onThrowInput"); //One Param: glm::vec3 bottleForce
	TEXT onTouchDirCalculated = TEXT("onDirectionCalculated"); //One Param: Move defenseMove

	virtual void Init() override;

	void StartReceiveInput(amp::NetworkUDP* network);

	virtual void Update() override;

	void CallFirstNetworkEvent();

	void CallInputEvents();

	virtual void End() override;

	virtual void Reset() override;

	void ProcessSound(uint8_t soundFlags);

private:
	CurrentInput input;
	uint8_t lastFlags = 0u;
	bool isFirst = true;
};


