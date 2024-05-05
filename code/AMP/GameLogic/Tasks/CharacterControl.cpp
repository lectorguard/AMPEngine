#include "pch.h"
#include "CharacterControl.h"

void CharacterControl::Init()
{
	//VELOCITY
	engine->events.subscribeEvent(startVelocityCheck, &CharacterControl::StartVelocityCheck, this);
	engine->events.subscribeEvent(endVelocityCheck, &CharacterControl::EndVelocity, this);
	//ZLOCATION
	engine->events.subscribeEvent(startFixZLocation, &CharacterControl::StartFixZLocation, this);
	engine->events.subscribeEvent(endFixZLocation, &CharacterControl::EndFixZLocation, this);
	//RELOCATING
	engine->events.subscribeEvent(startRelocateWhenFalling, &CharacterControl::StartRelocateWhenFalling, this);
	engine->events.subscribeEvent(endRelocateWhenFalling, &CharacterControl::EndRelocateWhenFalling, this);
	//ORIENTATION
	engine->events.subscribeEvent(startFixOrientation, &CharacterControl::StartFixOrientation, this);
	engine->events.subscribeEvent(endFixOrientation, &CharacterControl::EndFixOrientation, this);
	engine->events.subscribeEvent(CharacterControlReset, &CharacterControl::Reset, this);
}

void CharacterControl::StartVelocityCheck(bool laneChange, bool jump)
{
	isChangeVelocity = laneChange;
	isJumpVelocity = jump;
	if ((laneChange || jump))AddSharedUpdate();
}

void CharacterControl::StartFixZLocation(float Zlocation)
{
	this->Zlocation = Zlocation;
	if (isZFix)throw "You can not call StartFixZLocation twice, call endFixZlocation first";
	isZFix = true;
	AddSharedUpdate();
}

void CharacterControl::SetIsZLocationFixed(bool isChecked)
{
	isZFix = isChecked;
}

void CharacterControl::StartFixOrientation(bool isForward)
{
	if (isFixedOrientation)throw "you need to call first endFixOrientation !!!";
	isFixedOrientation = true;
	this->isForward = isForward;
	AddSharedUpdate();
}

void CharacterControl::StartRelocateWhenFalling(float Yheight)
{
	if (isRelocating)throw "You need to call end Relocating before calling Start";
	this->triggerHeight = Yheight;
	isRelocating = true;
	AddSharedUpdate();
}

void CharacterControl::UpdateZLocation()
{
	if (isZFix) {
		auto loc = character->getLocation();
		loc.z = Zlocation;
		character->setLocation(loc);
		auto force = character->getLinearVelocity();
		force.z = 0.0f;
		character->ResetLinearVelocity(force);
	}
}

void CharacterControl::SetRunningTransform()
{
	if (isFixedOrientation)
	{
		auto rotation = glm::quat(1, 0, 0, 0);
		if (!isForward){
			glm::vec3 euler = glm::eulerAngles(rotation);
			euler.y += glm::radians(180.0f);
			rotation = glm::quat(euler);
		}
		character->setRotation(rotation);
	}
}

void CharacterControl::Update()
{
	UpdateVelocity();
	UpdateZLocation();
	HeightRespawn();
	SetRunningTransform();
}

void CharacterControl::HeightRespawn()
{
	if (isRelocating) {
		if (character->getLocation().y <= triggerHeight) {
			character->ResetAngularVelocity();
			character->ResetLinearVelocity();
			engine->events.publishEvent(onCharacterRelocate);
		}
	}
}

void CharacterControl::UpdateVelocity()
{
	auto vec = character->getLinearVelocity();
	if (isJumpVelocity && vec.y > MaxJumpVelocity)vec.y = MaxJumpVelocity;
	if (isChangeVelocity && vec.z > MaxLaneChangeVelocity)vec.z = MaxLaneChangeVelocity;
	else if (isChangeVelocity && vec.z < -MaxLaneChangeVelocity)vec.z = -MaxLaneChangeVelocity;
	character->ResetLinearVelocity(vec);
}

void CharacterControl::EndVelocity()
{
	RemoveSharedUpdate();
	isChangeVelocity = false;
	isJumpVelocity = false;
}

void CharacterControl::EndFixZLocation()
{
	RemoveSharedUpdate();
	if (!isZFix)throw "call start before end";
	isZFix = false;
	Zlocation = 0.0f;
}

void CharacterControl::EndRelocateWhenFalling()
{
	RemoveSharedUpdate();
	if (!isRelocating)throw "call start before end";
	isRelocating = false;
}

void CharacterControl::EndFixOrientation()
{
	RemoveSharedUpdate();
	if (!isRelocating)throw "call start before end";
	isFixedOrientation = false;
}

void CharacterControl::Reset()
{
	ResetUpdate();
	isChangeVelocity = false;
	isJumpVelocity = false;
	Zlocation = 0.0f;
	triggerHeight = -40.0f;
	isRelocating = false;
	isFixedOrientation = false;
	isForward = true;
	isZFix = false;
}

