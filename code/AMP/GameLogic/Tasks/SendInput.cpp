#include "SendInput.h"

void SendInput::Init()
{
	//Update input variables
	engine->events.subscribeEvent(acceptNewDefendMove, &SendInput::AcceptNewDefendMove, this);
	engine->events.subscribeEvent(acceptNewAttackThrowForce, &SendInput::AcceptNewAttackThrowForce, this);
	engine->events.subscribeEvent(TEXT("onTouchIcon"), &SendInput::OnIconTouch, this);

	//Task events
	engine->events.subscribeEvent(startSendInput, &SendInput::StartSendingInput, this);
	engine->events.subscribeEvent(stopSendInput, &SendInput::End, this);
	engine->events.subscribeEvent(SendInputReset, &SendInput::Reset, this);
	engine->events.subscribeEvent(clearInputFlags, &SendInput::ClearInputFlags, this);
	engine->events.subscribeEvent(acceptSoundFlags, &SendInput::AcceptSoundFlags, this);
}

void SendInput::StartSendingInput(amp::NetworkUDP* network)
{
	assert(network);
	this->networking = network;
	AddUpdate();
}

void SendInput::Update()
{
	networking->Send(input.bsify());
}

void SendInput::End()
{
	RemoveUpdate();
	networking = nullptr;
}

void SendInput::Reset()
{
	ResetUpdate();
	networking = nullptr;
}

void SendInput::AcceptNewAttackThrowForce(glm::vec3 force)
{
	input.attackThrowForce_1 = force;
}

void SendInput::AcceptNewDefendMove(Move m)
{
	input.defendingMove_0 = m;
}

void SendInput::OnIconTouch()
{
	input.touchedIcons_2++;
}

void SendInput::UpdateThrowFlags(unsigned char flags)
{
	networkFlags = flags;
}

void SendInput::ClearInputFlags(uint8_t flags)
{
	if (isFlaggedIndex(flags, 0)) {
		input.defendingMove_0 = Move::NoMove;
	}
	if(isFlaggedIndex(flags,1)){
		input.attackThrowForce_1 = glm::vec3(0);
	}
	if (isFlaggedIndex(flags, 2)) {
		input.touchedIcons_2 = 0;
	}
}

void SendInput::AcceptSoundFlags(uint8_t flags)
{
	input.soundflags = flags;
}

