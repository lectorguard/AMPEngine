#include "SendGroundInfo.h"

void SendGroundInfo::Init()
{
	engine->events.subscribeEvent(setGroundInfo, &SendGroundInfo::SetGroundInfo, this);
	engine->events.subscribeEvent(setGroundNetworkInfo, &SendGroundInfo::SetNetwork, this);
}

void SendGroundInfo::SetNetwork(amp::NetworkTCP* network)
{
	networking = network;
	hasNetwork = true;
	TryStartSend();
}

void SendGroundInfo::SetGroundInfo(GroundInfo info)
{
	this->groundInfo = info;
	hasGroundData = true;
	TryStartSend();
}

void SendGroundInfo::TryStartSend()
{
	if (hasNetwork && hasGroundData) {
		StartSending();
	}
}

void SendGroundInfo::StartSending()
{
	assert(networking);
	networking->Send(groundInfo.bsify(), true);
}


void SendGroundInfo::Reset()
{
	
}
