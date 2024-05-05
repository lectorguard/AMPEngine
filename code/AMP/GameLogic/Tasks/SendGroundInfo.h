#pragma once
#include "TaskController.h"
#include "AMP_Engine.h"
#include "Components/NetworkTCP.h"
#include "Actors/Ground.h"

class SendGroundInfo : public amp::Task
{

public:
	amp::NetworkTCP* networking = nullptr;
	GroundInfo groundInfo;

	SendGroundInfo() {
	};
	virtual ~SendGroundInfo() {};

	SendGroundInfo(const SendGroundInfo&) = delete;
	SendGroundInfo(SendGroundInfo&&) = delete;


	//EVENTS
	//INPUTS
	TEXT setGroundInfo = TEXT("setGroundInfo"); //One Param: GroundInfo info
	TEXT setGroundNetworkInfo = TEXT("setGroundNetworkInfo"); //One Param:  amp::NetworkTCP* network
	//Receive New Data

	virtual void Init() override;

	void SetNetwork(amp::NetworkTCP* network);

	void SetGroundInfo(GroundInfo info);

	void TryStartSend();

	void StartSending();

	virtual void Reset() override;

private:
	bool hasNetwork = false;
	bool hasGroundData = false;
};


