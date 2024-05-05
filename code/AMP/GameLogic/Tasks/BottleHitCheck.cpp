#include "pch.h"
#include "Sound/SoundPool.h"
#include "BottleHitCheck.h"
#include "cstdlib"

void BottleHitCheck::Init()
{
	engine->events.subscribeEvent(startBottleHitCheck, &BottleHitCheck::StartHitCheck, this);
	engine->events.subscribeEvent(endBottleHitCheck, &BottleHitCheck::End, this);
	engine->events.subscribeEvent(BottleHitCheckReset, &BottleHitCheck::Reset, this);
}

void BottleHitCheck::StartHitCheck()
{
	if (IsUpdateActive())throw "error you should call end before start";
	AddUpdate();
}

void BottleHitCheck::Update()
{
	if (bottle->getLocation().y < YHitHeight || glm::length(bottle->getLinearVelocity()) > MaxBottleForce) {
		End();
		engine->events.publishEvent(onBottleHit);
	}
}

void BottleHitCheck::End()
{
	RemoveUpdate();
}

void BottleHitCheck::Reset()
{
	ResetUpdate();
}
