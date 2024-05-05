#include "pch.h"
#include "BottleValidityCheck.h"

void BottleValidityCheck::Init()
{
	engine->events.subscribeEvent(startBottleValidityCheck, &BottleValidityCheck::StartValidityCheck, this);
	engine->events.subscribeEvent(stopBottleValidityCheck, &BottleValidityCheck::BottleCheckEnd, this);
	engine->events.subscribeEvent(BottleValidityCheckReset, &BottleValidityCheck::Reset, this);
}

void BottleValidityCheck::StartValidityCheck(amp::DynamicMesh* mesh, const unsigned char flags)
{
	auto info = BottleCheckInfo(mesh);
	info.timeAfterThrow = TIME::GetTime();
	info.flags = flags;
	bottles.Add(info);
	AddSharedUpdate();
}

void BottleValidityCheck::Update()
{
	bottles.Loop([&](int i, BottleCheckInfo b) {Checking(b,i); });
}

void BottleValidityCheck::Checking(BottleCheckInfo info, int index)
{
	if(isFlagged(info,VelocityChecking))VelocityCheck(info);
	if(isFlagged(info,YChecking))		YCheck(info);
	if(isFlagged(info,TimeChecking))	TimeCheck(info);
}

bool BottleValidityCheck::isFlagged(BottleCheckInfo& info, unsigned char flag)
{
	return (info.flags & flag) == flag;
}

void BottleValidityCheck::TimeCheck(BottleCheckInfo info)
{
	if (info.timeAfterThrow > 0 && info.timeAfterThrow + resetTimeInSeconds < TIME::GetTime()) {
		info.timeAfterThrow = -1.0f;
		engine->events.publishEvent(bottleInvalid,info.bottle, InvalidType::Time);
	}
}

void BottleValidityCheck::YCheck(BottleCheckInfo info)
{
	if (info.bottle->getLocation().y < resetYLocation) {
		engine->events.publishEvent(bottleInvalid, info.bottle, InvalidType::YLocation);
	}
}

void BottleValidityCheck::VelocityCheck(BottleCheckInfo info)
{
	auto velocity = glm::length(info.bottle->getLinearVelocity());
	if (velocity > leastVelocity)info.maxThrowVelocity = velocity;
	if (glm::length(info.bottle->getLinearVelocity()) < leastVelocity && info.maxThrowVelocity > leastVelocity) {
		info.maxThrowVelocity = 0.0f;
		engine->events.publishEvent(bottleInvalid, info.bottle, InvalidType::Velocity);
	}
}

void BottleValidityCheck::BottleCheckEnd(amp::DynamicMesh* mesh)
{
	RemoveSharedUpdate();
	bottles.Remove(BottleCheckInfo(mesh));
}

void BottleValidityCheck::Reset()
{
	bottles.clear();
	ResetUpdate();
}

