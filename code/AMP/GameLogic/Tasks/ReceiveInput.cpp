#include "Tasks/ProgressBars.h"
#include "ReceiveInput.h"

void ReceiveInput::Init()
{
	engine->events.subscribeEvent(startReceiveInput, &ReceiveInput::StartReceiveInput, this);
	engine->events.subscribeEvent(stopReceiveInput, &ReceiveInput::End, this);
	engine->events.subscribeEvent(ReceiveInputReset, &ReceiveInput::Reset, this);
}

void ReceiveInput::StartReceiveInput(amp::NetworkUDP* network)
{
	assert(network);
	this->networking = network;
	AddUpdate();
}

void ReceiveInput::Update()
{
	auto bson = networking->getReceivedInfo();
	if (bson.size() > 0) {
		input = CurrentInput(bson);
		if(input.getFlags() != uint8_t(0) && input.getFlags() != lastFlags)CallInputEvents();
		lastFlags = input.getFlags();
		engine->events.publishEvent(TEXT("sendInputFlags"), input.getFlags());
		ProcessSound(input.soundflags);
		if (isFirst)CallFirstNetworkEvent();
	}
}

void ReceiveInput::CallFirstNetworkEvent()
{
	engine->events.publishEvent(TEXT("NetworkingIsRunning"));
	isFirst = false;
}

void ReceiveInput::CallInputEvents()
{
	if (input.attackThrowForce_1 != glm::vec3(0)) {
		engine->events.publishEvent(throwAttackBottle, input.attackThrowForce_1);
	}
	if (input.defendingMove_0 != Move::NoMove) {
		engine->events.publishEvent(onTouchDirCalculated, input.defendingMove_0);
	}
	if (input.touchedIcons_2 > 0) {
		for (int i = 0; i < input.touchedIcons_2; ++i) {
			engine->events.publishEvent(TEXT("updatePlayerProgress"), Player::Two);
		}
	}
}

void ReceiveInput::End()
{
	RemoveUpdate();
	networking = nullptr;
	isFirst = true;
}

void ReceiveInput::Reset()
{
	ResetUpdate();
	networking = nullptr;
	isFirst = true;
}

void ReceiveInput::ProcessSound(uint8_t soundFlags)
{
	engine->events.publishEvent(TEXT("setSoundFlags"), soundFlags);
}

