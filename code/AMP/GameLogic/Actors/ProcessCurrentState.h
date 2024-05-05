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

class ProcessCurrentState : public amp::Actor
{
public:
	amp::NetworkUDP * networkUDP = nullptr;
	amp::NetworkTCP* networkTCP = nullptr;
	amp::TaskController* controller = nullptr;

	ProcessCurrentState() {
		networkUDP = AddComponent<amp::NetworkUDP>(TEXT("udp_receive_state"));
		networkTCP = AddComponent<amp::NetworkTCP>(TEXT("tcp_receive_state"));
		controller = CreateTaskController();
	};
	ProcessCurrentState(const ProcessCurrentState&) = delete;
	ProcessCurrentState(ProcessCurrentState&&) = delete;
	virtual ~ProcessCurrentState();

	PickUpActor* pickUpActor = nullptr;
	ThrowingActor* throwingActor = nullptr;

	//EVENTS
	//INPUT
	TEXT startReceiveData = TEXT("startReceiveData");

	virtual void OnActivate() override;

	virtual void OnDeactivate() override;

	virtual void AfterWindowStartup() override;

	void StartReceivingState(std::string ipOther);

	virtual void Update() override;

	virtual void BeforeSystemShutdown() override;
};
