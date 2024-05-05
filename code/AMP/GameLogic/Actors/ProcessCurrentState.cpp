#include "Tasks/ReceiveGameState.h"
#include "Tasks/SendInput.h"
#include "Tasks/ReceiveGroundInfo.h"
#include "ProcessCurrentState.h"

ProcessCurrentState::~ProcessCurrentState()
{
	controller->clear();
}

void ProcessCurrentState::OnActivate()
{

}

void ProcessCurrentState::OnDeactivate()
{

}

void ProcessCurrentState::AfterWindowStartup()
{
	engine->events.subscribeEvent(startReceiveData, &ProcessCurrentState::StartReceivingState, this);
	controller->AddTask<ReceiveGameState>();
	controller->AddTask<SendInput>();
	controller->AddTask<ReceiveGroundInfo>();
}

void ProcessCurrentState::StartReceivingState(std::string ipOther)
{
	auto recvAddrInfo = amp::NetworkHelper::getAdressInfo("0.0.0.0", 57368);
	networkUDP->SetUpReceiving(recvAddrInfo);
	engine->events.publishEvent(TEXT("startReceiveGameState"), networkUDP);

	auto sendInputInfo = amp::NetworkHelper::getAdressInfo(ipOther.c_str(), 57369);
	networkUDP->SetUpSending(sendInputInfo);
	engine->events.publishEvent(TEXT("startSendInput"), networkUDP);

	auto recvAddrGroundInfo = amp::NetworkHelper::getAdressInfo("0.0.0.0", 57367);
	networkTCP->SetUpReceiving(recvAddrGroundInfo);
	engine->events.publishEvent(TEXT("setReceiveGroundNetworkInfo"), networkTCP);
}

void ProcessCurrentState::Update()
{
	controller->CallUpdateForTasks();
}

void ProcessCurrentState::BeforeSystemShutdown()
{

}

