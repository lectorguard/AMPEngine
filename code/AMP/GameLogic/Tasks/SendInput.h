#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "GameUtility.h"
#include "Components/NetworkUDP.h"

class SendInput : public amp::Task
{

public:
	amp::NetworkUDP* networking = nullptr;

	SendInput() {
	};
	virtual ~SendInput() {};

	SendInput(const SendInput&) = delete;
	SendInput(SendInput&&) = delete;

	//Server is always Player ONE, client Player TWO


	//EVENTS
	//INPUTS
	TEXT acceptNewGameStage = TEXT("acceptNewGameStage");    //One Param: GameStagesOffline stage
	TEXT acceptNewAttackThrowForce = TEXT("acceptNewAttackThrowForce"); //One Param: glm::vec3 force
	TEXT acceptNewDefendMove = TEXT("acceptNewDefendMove"); //One Param: Move m

	TEXT SendInputReset = TEXT("SendInputReset");	//No Params: RESET has always 0 params
	TEXT startSendInput = TEXT("startSendInput");   //One Param Direction
	TEXT stopSendInput = TEXT("stopSendInput");		//No Param
	TEXT updateNetFlags = TEXT("updateNetFlags"); //One Param: unsigned char flags
	TEXT clearInputFlags = TEXT("clearInputFlags"); //One Param: uint8_t flags
	TEXT acceptSoundFlags = TEXT("acceptSoundFlags"); //One Param: uint8_t soundFlags
	//


	virtual void Init() override;

	void StartSendingInput(amp::NetworkUDP* network);

	virtual void Update() override;

	virtual void End() override;

	virtual void Reset() override;

	void AcceptNewAttackThrowForce(glm::vec3 force);
	void AcceptNewDefendMove(Move m);
	void OnIconTouch();
	void UpdateThrowFlags(unsigned char flags);
	void ClearInputFlags(uint8_t flags);
	void AcceptSoundFlags(uint8_t flags);
private:
	CurrentInput input;
	CurrentInput lastInput;
	unsigned char networkFlags = 0;
};

