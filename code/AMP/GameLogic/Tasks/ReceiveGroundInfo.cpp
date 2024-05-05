#include "ReceiveGroundInfo.h"

void ReceiveGroundInfo::Init()
{
	engine->events.subscribeEvent(setReceiveGroundNetworkInfo, &ReceiveGroundInfo::SetNetwork, this);
}

void ReceiveGroundInfo::SetNetwork(amp::NetworkTCP* network)
{
	assert(network);
	networking = network;
	AddUpdate();
}

void ReceiveGroundInfo::Reset()
{
	ResetUpdate();
	networking = nullptr;
}

void ReceiveGroundInfo::Update()
{
	auto bson = networking->getReceivedInfo(true);
	if (bson.size() > 0) {
		networking->clearReceivedInfo();
		GroundInfo data = GroundInfo(bson);
		engine->events.publishEvent(TEXT("processServerGroundInfo"), data);
		networking->smoothCloseReceiving();
		RemoveUpdate();
	}
}

