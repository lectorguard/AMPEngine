#include "pch.h"
#include "Sound/SoundPool.h"
#include "CharacterFallback.h"

void CharacterFallback::Init()
{
	engine->events.subscribeEvent(updateFallbackCheck, &CharacterFallback::UpdateFallbackCheck, this);
	engine->events.subscribeEvent(resetFallbackCheck, &CharacterFallback::ResetFallbackCheck, this);
	engine->events.subscribeEvent(CharacterFallbackReset, &CharacterFallback::Reset, this);
}

void CharacterFallback::UpdateFallbackCheck(bool isForward, float currXFallback)
{
	this->isForward = isForward;
	this->currXFallback = currXFallback;
	if(!IsUpdateActive())AddUpdate();
}

void CharacterFallback::Update()
{
	if ((isForward && character->getLocation().x < currXFallback) ||
	(!isForward && character->getLocation().x > currXFallback))
		SetFallbackLocation();
}

void CharacterFallback::SetFallbackLocation()
{
	auto vec = character->getLocation();
	vec.x = currXFallback;
	character->setLocation(vec);

}

void CharacterFallback::ResetFallbackCheck()
{
	if (IsUpdateActive())RemoveUpdate();
	currXFallback = 0.0f;
}

void CharacterFallback::Reset()
{
	ResetUpdate();
	isForward = true;
	currXFallback = 0.0f;
}
