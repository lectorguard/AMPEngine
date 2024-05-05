#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Event/EventSystem.h"
#include "Components/NetworkTCP.h"
#include "Actors/Ground.h"

class ReceiveGroundInfo : public amp::Task
{

public:
	amp::NetworkTCP * networking = nullptr;

	ReceiveGroundInfo() {};
	virtual ~ReceiveGroundInfo() {};

	ReceiveGroundInfo(const ReceiveGroundInfo&) = delete;
	ReceiveGroundInfo(ReceiveGroundInfo&&) = delete;


	//EVENTS
	//INPUTS
	TEXT setReceiveGroundNetworkInfo = TEXT("setReceiveGroundNetworkInfo"); //One Param:  amp::NetworkTCP* network

	//Receive New Data

	virtual void Init() override;

	void SetNetwork(amp::NetworkTCP* network);

	virtual void Reset() override;

	virtual void Update() override;
};


