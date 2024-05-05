#pragma once
#include "GameUtility.h"
#include "Actor.h"
#include "AMP_Engine.h"
#include "PickUpActor.h"
#include "ThrowingActor.h"
#include "Static/Camera.h"
#include "Components/NetworkUDP.h"
#include "Components/NetworkTCP.h"
#include "Utility/NetworkHelper.h"

class SendCurrentState : public amp::Actor
{
public:
	amp::NetworkUDP* networkUDP = nullptr;
	amp::NetworkTCP* networkTCP = nullptr;
	amp::TaskController* controller = nullptr;


	SendCurrentState() {
		networkUDP = AddComponent<amp::NetworkUDP>(TEXT("udp_send_state"));
		networkTCP = AddComponent<amp::NetworkTCP>(TEXT("tcp_send_data"));
		controller = CreateTaskController();
	};
	SendCurrentState(const SendCurrentState&) = delete;
	SendCurrentState(SendCurrentState&&) = delete;
	virtual ~SendCurrentState();

	PickUpActor* pickUpActor = nullptr;
	ThrowingActor* throwingActor = nullptr;

	//EVENTS
	//INPUT
	TEXT startSendData = TEXT("startSendData");
	

	virtual void OnActivate() override;

	virtual void OnDeactivate() override;

	virtual void AfterWindowStartup() override;

	void StartSendingState(std::string ipOther);

	virtual void Update() override;

	virtual void BeforeSystemShutdown() override;
};