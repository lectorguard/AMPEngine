#include "pch.h"
#include "Sound/SoundPool.h"
#include "Running.h"

void Running::Init()
{
	engine->events.subscribeEvent(startRunning, &Running::StartRunning, this);
	engine->events.subscribeEvent(stopRunning, &Running::End, this);
	engine->events.subscribeEvent(RunningReset, &Running::Reset, this);
}

void Running::StartRunning(bool isForward)
{
	assert(!IsUpdateActive());
	this->isForward = isForward;
	AddUpdate();
}


void Running::Update()
{
	UpdateCharacterSpeed();
}

void Running::UpdateCharacterSpeed()
{
	character->ResetAngularVelocity();
	auto vec = character->getLinearVelocity();
	if (isForward) {
		if (vec.x > MaxForwardVelocity) {
			vec.x = MaxForwardVelocity;
			character->ResetLinearVelocity(vec);
		}
		else {
			character->applyForceToCenter(glm::vec3(ForwardForce, 0, 0));
		}
	}
	else {
		if (vec.x < -MaxForwardVelocity) {
			vec.x = -MaxForwardVelocity;
			character->ResetLinearVelocity(vec);
		}
		else {
			character->applyForceToCenter(glm::vec3(-ForwardForce, 0, 0));
		}
	}
}

void Running::End()
{
	assert(IsUpdateActive());
	RemoveUpdate();
	character->ResetLinearVelocity();
}

void Running::Reset()
{
	ResetUpdate();
	isForward = true;
}
