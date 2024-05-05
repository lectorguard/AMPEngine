#include "Tasks/SendGameState.h"
#include "Tasks/ReceiveInput.h"
#include "Tasks/SendGroundInfo.h"
#include "SendCurrentState.h"

SendCurrentState::~SendCurrentState()
{
	controller->clear();
}

void SendCurrentState::OnActivate()
{

}

void SendCurrentState::OnDeactivate()
{

}

void SendCurrentState::AfterWindowStartup()
{
	engine->events.subscribeEvent(startSendData, &SendCurrentState::StartSendingState, this);
	controller->AddTask<SendGameState>();
	controller->AddTask<ReceiveInput>();
	controller->AddTask<SendGroundInfo>();
}

void SendCurrentState::StartSendingState(std::string ipOther)
{
	auto addrSendStateInfo = amp::NetworkHelper::getAdressInfo(ipOther.c_str(), 57368);
	networkUDP->SetUpSending(addrSendStateInfo);
	engine->events.publishEvent(TEXT("startSendGameState"),networkUDP);

	auto addrRecvInputInfo = amp::NetworkHelper::getAdressInfo("0.0.0.0", 57369);
	networkUDP->SetUpReceiving(addrRecvInputInfo);
	engine->events.publishEvent(TEXT("startReceiveInput"),networkUDP);

	auto addrTCPSendInfo = amp::NetworkHelper::getAdressInfo(ipOther.c_str(), 57367);
	networkTCP->SetUpSending(addrTCPSendInfo);
	engine->events.publishEvent(TEXT("setGroundNetworkInfo"), networkTCP);

	engine->events.publishEvent(TEXT("sendServerGroundInfo"));
}

void SendCurrentState::Update()
{
	controller->CallUpdateForTasks();
}

void SendCurrentState::BeforeSystemShutdown()
{

}
